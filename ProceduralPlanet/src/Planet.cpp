#include "Planet.h"
#include <vector>
#include <thread>
#include <future>

#include "OpenGLErrorHandler.h"
#include "Log.h"

Planet::~Planet()
{
	Delete();
}

Planet::Planet(size_t resolution, std::vector<INoiseSettings*> settings):
	resolution(resolution),
	settings(settings),
	facesGLData(6,Planet::PlanetGLData())
{
}


void Planet::CreatePlanet()
{
	std::vector<glm::vec3> directions;
	directions.reserve(6);
	directions.emplace_back(0.0f, 1.0f, 0.0f);
	directions.emplace_back(0.0f, -1.0f, 0.0f);
	directions.emplace_back(-1.0f, 0.0f, 0.0f);
	directions.emplace_back(1.0f, 0.0f, 0.0f);
	directions.emplace_back(0.0f, 0.0f, 1.0f);
	directions.emplace_back(0.0f, 0.0f, -1.0f);

	spherePositions.clear();
	spherePositions.reserve(6);
	elevations.clear();
	elevations.reserve(6);
	elevations.clear();
	indices.reserve(6);

	for (size_t i = 0; i < directions.size(); i++)
	{
		Planet::PlanetFacesVertices vertices = CalculateSphereFacePositions(&directions[i]);
		spherePositions.push_back(vertices.positions);
		elevations.push_back(vertices.elevations);
		indices.push_back(vertices.indices);
	}

	BindToGPU();

}


Planet::PlanetFacesVertices Planet::CalculateSphereFacePositions(glm::vec3* localUp)
{
	glm::vec3 axisA = glm::vec3(localUp->y, localUp->z, localUp->x);
	glm::vec3 axisB = glm::cross(*localUp, axisA);

	size_t triIndex = 0;
	Planet::PlanetFacesVertices vertices(resolution);

	for (size_t y = 0; y < resolution; y++)
	{
		for (size_t x = 0; x < resolution; x++)
		{
			unsigned int i = x + y * resolution;
			glm::vec2 percent = glm::vec2(x, y);
			percent /= (resolution - 1);
			glm::vec3 pointOnUnitCube = *localUp + (percent.x - 0.5f) * 2 * axisA + (percent.y - 0.5f) * 2 * axisB;
			glm::vec3 pointOnUnitSphere = glm::normalize(pointOnUnitCube);

			vertices.positions[i] = pointOnUnitSphere;
			vertices.elevations[i] = CalculatePointElevation(&pointOnUnitSphere);

			if (x != resolution - 1 && y != resolution - 1)
			{
				vertices.indices[triIndex] = i;
				vertices.indices[triIndex + 1] = i + resolution + 1;
				vertices.indices[triIndex + 2] = i + resolution;

				vertices.indices[triIndex + 3] = i;
				vertices.indices[triIndex + 4] = i + 1;
				vertices.indices[triIndex + 5] = i + resolution + 1;
				triIndex += 6;
			}
		}
	}
	return vertices;
}

std::vector<float> Planet::CalculateSphereFaceElevations(size_t index)
{
	if (spherePositions.size() < 1)
	{
		Log::LogError("No positions", true);
		return std::vector<float>();
	}

	std::vector<float> elevation(spherePositions[0].size(),0.0f);
	for (size_t y = 0; y < resolution; y++)
	{
		for (size_t x = 0; x < resolution; x++)
		{
			unsigned int i = x + y * resolution;
			float elevation = CalculatePointElevation(&spherePositions[index][i]);
		}
	}
	return elevation;
}

float Planet::CalculatePointElevation(glm::vec3* point)
{
	float elevation = 0;
	float firstLayerValue = 0;
	for (size_t noiseFilterIndex = 0; noiseFilterIndex < settings.size(); noiseFilterIndex++)
	{
		float noiseAmount = settings[noiseFilterIndex]->Evaluate(&noise, *point);
		if (noiseFilterIndex == 0)
		{
			firstLayerValue = noiseAmount;
			elevation += noiseAmount;
		}
		else
		{

			elevation += (noiseAmount * firstLayerValue);
		}
	}
	elevation = (1 + elevation);
	return elevation;
}

std::vector <std::vector<glm::vec3>> Planet::CalculateNormals()
{
	std::vector <std::vector<glm::vec3>> normalsPerFace(6);
	for (size_t x = 0; x < spherePositions.size(); x++)
	{
		std::async([=] {
			std::vector<glm::vec3> normals(spherePositions[x].size(), glm::vec3(0.0f));
			for (size_t i = 0; i < indices[x].size(); i += 3)
			{
				unsigned int in0 = indices[x][i];
				unsigned int in1 = indices[x][i + 1];
				unsigned int in2 = indices[x][i + 2];
				glm::vec3 v1(spherePositions[x][in1] - spherePositions[x][in0]);
				glm::vec3 v2(spherePositions[x][in2] - spherePositions[x][in0]);
				glm::vec3 normal = glm::cross(v1, v2);
				normal = glm::normalize(normal);

				normals[in0] += normal;
				normals[in1] += normal;
				normals[in2] += normal;
			}
			return normals;
			});
	}

	for (size_t x = 0; x < spherePositions.size(); x++)
	{
		workers[x].join();
	}
	return normalsPerFace;
}






void Planet::BindToGPU()
{
	for (size_t x = 0; x< spherePositions.size();x++)
	{
		std::vector<glm::vec3> normals(spherePositions[x].size(), glm::vec3(0.0f));
		for (size_t i = 0; i < indices[x].size(); i += 3)
		{
			unsigned int in0 = indices[x][i];
			unsigned int in1 = indices[x][i + 1];
			unsigned int in2 = indices[x][i + 2];
			glm::vec3 v1(spherePositions[x][in1] - spherePositions[x][in0]);
			glm::vec3 v2(spherePositions[x][in2] - spherePositions[x][in0]);
			glm::vec3 normal = glm::cross(v1, v2);
			normal = glm::normalize(normal);

			normals[in0] += normal;
			normals[in1] += normal;
			normals[in2] += normal;
		}

		std::vector<glm::vec3> vertices;
		vertices.reserve(spherePositions[x].size() * 2);
		for (size_t i = 0; i < spherePositions[x].size(); i++)
		{
			normals[i] = normalize(normals[i]);
			vertices.emplace_back(spherePositions[x][i]*elevations[x][i]);
			vertices.emplace_back(normalize(normals[i]));
		}

		GLCall(glGenVertexArrays(1, &facesGLData[x].vertexArrayObjectID));
		GLCall(glBindVertexArray(facesGLData[x].vertexArrayObjectID));

		GLCall(glGenBuffers(1, &facesGLData[x].vertexBufferID));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, facesGLData[x].vertexBufferID));
		GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size() * 3, &vertices[0], GL_STATIC_DRAW));//size of array * 3 for 3 floats in a vertex

		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0));
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float) * 3)));
		GLCall(glEnableVertexAttribArray(1));

		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLCall(glBindVertexArray(0));

		GLCall(glGenBuffers(1, &facesGLData[x].indexBufferID));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesGLData[x].indexBufferID));
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices[x].size(), &indices[x][0], GL_STATIC_DRAW));

		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		facesGLData[x].triCount = indices[x].size();
	}
}


void Planet::SetResolution(size_t resolution)
{
}

void Planet::SetSettings(std::vector<INoiseSettings> settings)
{
}


void Planet::Draw()
{
	for (size_t i = 0; i < facesGLData.size(); i++)
	{
		GLCall(glBindVertexArray(facesGLData[i].vertexArrayObjectID));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesGLData[i].indexBufferID));
		GLCall(glDrawElements(GL_TRIANGLES, facesGLData[i].triCount, GL_UNSIGNED_INT, nullptr));
	}
}

void Planet::Delete()
{
	facesGLData.clear();
	spherePositions.clear();
	elevations.clear();
	indices.clear();
}

void Planet::DeleteFromGPU()
{
	for (size_t i = 0; i < facesGLData.size(); i++)
	{
		GLCall(glDeleteVertexArrays(1, &facesGLData[i].vertexArrayObjectID));
		GLCall(glDeleteBuffers(1, &facesGLData[i].vertexBufferID));
		GLCall(glDeleteBuffers(1, &facesGLData[i].indexBufferID));
	}
}
