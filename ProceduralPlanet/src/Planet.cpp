#include "Planet.h"
#include <thread>

#include "OpenGLErrorHandler.h"
#include "Log.h"
#include "ConfigMacros.h"

Planet::Planet()
{
	planetSides.reserve(6);
	directions.reserve(6);
	directions.emplace_back(0.0f, 1.0f, 0.0f);
	directions.emplace_back(0.0f, -1.0f, 0.0f);
	directions.emplace_back(-1.0f, 0.0f, 0.0f);
	directions.emplace_back(1.0f, 0.0f, 0.0f);
	directions.emplace_back(0.0f, 0.0f, 1.0f);
	directions.emplace_back(0.0f, 0.0f, -1.0f);
	isBusy = false;
}

Planet::~Planet()
{
	planetSides.clear();
	Unbind();
}


void Planet::CreatePlanet(size_t resolution, std::vector<INoiseSettings*> noiseSettings)
{
	
	if (isBusy)
	{
		return;
	}

	isBusy = true;

#if THREADED == 1
	futureData.clear();
	futureData.reserve(6);
	futures.clear();
	futures.reserve(6);
	for (int i = 0; i < directions.size(); i++)
	{
		futures.push_back(std::async(std::launch::async, [] 
			(size_t resolution, std::vector<INoiseSettings*> settings,
			std::vector<Planet::VerticesData*>* futureData, glm::vec3 direction, std::mutex* mutex)
			{
				Planet::VerticesData* face = CreatePlanetSide(resolution, settings, direction);
				std::lock_guard < std::mutex > lock(*mutex);
				futureData->push_back(face);
			}, 
			resolution, noiseSettings, &futureData, directions[i], &sideCreation_Mutex));
	}

	

#else
	//Not threaded
	for (int i = 0; i < directions.size(); i++)
	{
		faces.emplace_back(*resolution);
		faces[i].CreateMesh(&directions[i], &noise, noiseSettings);
		faces[i].BindToGPU();
	}

#endif 


}

Planet::VerticesData* Planet::CreatePlanetSide(size_t resolution, std::vector<INoiseSettings*> noiseSettings,glm::vec3 localUp)
{
	Noise noise;
	VerticesData* data = new VerticesData(resolution);
	glm::vec3 axisA = glm::vec3(localUp.y, localUp.z, localUp.x);
	glm::vec3 axisB = glm::cross(localUp, axisA);


	size_t triIndex = 0;

	for (size_t y = 0; y < resolution; y++)
	{
		for (size_t x = 0; x < resolution; x++)
		{
			unsigned int i = x + y * resolution;
			glm::vec2 percent = glm::vec2(x, y);
			percent /= (resolution - 1);
			glm::vec3 pointOnUnitCube = localUp + (percent.x - 0.5f) * 2 * axisA + (percent.y - 0.5f) * 2 * axisB;
			glm::vec3 pointOnUnitSphere = glm::normalize(pointOnUnitCube);
			float elevation = 0;
			float firstLayerValue = 0;
			for (size_t noiseFilterIndex = 0; noiseFilterIndex < noiseSettings.size(); noiseFilterIndex++)
			{
				float noiseAmount = noiseSettings[noiseFilterIndex]->Evaluate(&noise, pointOnUnitSphere);
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

			data->vertices[i] = pointOnUnitSphere * (1 + elevation);
			if (x != resolution - 1 && y != resolution - 1)
			{
				data->indices[triIndex] = i;
				data->indices[triIndex + 1] = i + resolution + 1;
				data->indices[triIndex + 2] = i + resolution;
				
				data->indices[triIndex + 3] = i;
				data->indices[triIndex + 4] = i + 1;
				data->indices[triIndex + 5] = i + resolution + 1;
				triIndex += 6;
			}
		}
	}

	std::vector<glm::vec3> normals(data->vertices.size(), glm::vec3(0.0f));
	for (size_t i = 0; i < data->indices.size(); i += 3)
	{
		unsigned int in0 = data->indices[i];
		unsigned int in1 = data->indices[i + 1];
		unsigned int in2 = data->indices[i + 2];
		glm::vec3 v1(data->vertices[in1] - data->vertices[in0]);
		glm::vec3 v2(data->vertices[in2] - data->vertices[in0]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		normals[in0] += normal;
		normals[in1] += normal;
		normals[in2] += normal;
	}

	std::vector<glm::vec3> normalVector;
	normalVector.reserve(data->vertices.size() * 2);
	for (size_t i = 0; i < normals.size(); i++)
	{
		normals[i] = normalize(normals[i]);
		normalVector.emplace_back(data->vertices[i]);
		normalVector.emplace_back(normalize(normals[i]));

	}

	data->vertices.clear();
	data->vertices = normalVector;

	return data;
}

Planet::GLIDs Planet::BindPlanetSide(Planet::VerticesData* vertices)
{
	GLIDs id;
	GLCall(glGenVertexArrays(1, &id.vertexArrayObjectID));
	GLCall(glBindVertexArray(id.vertexArrayObjectID));

	GLCall(glGenBuffers(1, &id.vertexBufferID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, id.vertexBufferID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices->vertices.size() * 3, &vertices->vertices[0], GL_STATIC_DRAW));//size of array * 3 for 3 floats in a vertex

	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float) * 3)));
	GLCall(glEnableVertexAttribArray(1));

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));
	
	id.triCount = vertices->indices.size();
	GLCall(glGenBuffers(1, &id.indexBufferID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id.indexBufferID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * vertices->indices.size(), &vertices->indices[0], GL_STATIC_DRAW));

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	delete vertices;
	return id;
}

void Planet::Draw()
{
	if (futureData.size() == 6)
	{
		Unbind();
		planetSides.reserve(6);

		for (int i = 0; i < futureData.size(); i++)
		{
			planetSides.push_back(BindPlanetSide(futureData[i]));
		}
		futureData.clear();
		isBusy = false;
	}

	for (int i = 0; i < planetSides.size(); i++)
	{
		GLCall(glBindVertexArray(planetSides[i].vertexArrayObjectID));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planetSides[i].indexBufferID));
		GLCall(glDrawElements(GL_TRIANGLES, planetSides[i].triCount, GL_UNSIGNED_INT, nullptr));
		GLCall(glBindVertexArray(0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0));
	}
}



void Planet::Unbind()
{
	for (int i = 0; i < planetSides.size(); i++)
	{
		GLCall(glBindVertexArray(0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

		GLCall(glDeleteVertexArrays(1, &planetSides[i].vertexArrayObjectID));
		GLCall(glDeleteBuffers(1, &planetSides[i].vertexBufferID));
		GLCall(glDeleteBuffers(1, &planetSides[i].indexBufferID));
	}
	planetSides.clear();
}
