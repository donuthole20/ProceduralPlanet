#include "Planet.h"
#include <thread>

#include "OpenGLErrorHandler.h"
#include "Log.h"
#include "ConfigMacros.h"

Planet::Planet()
{
	meshGLIDs.reserve(6);
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
	Unbind();
	meshGLIDs.clear();
}


void Planet::CreatePlanet(unsigned int resolution, std::vector<INoiseSettings*> noiseSettings)
{
	if (isBusy)
	{
		return;
	}

	isBusy = true;
	this->resolution = resolution;
	elevationMinMax = glm::vec2(0.0f);

#if THREADED == 1
	meshDataPerFace.clear();
	meshDataPerFace.reserve(6);
	planetSideMeshFutureHandle.clear();
	planetSideMeshFutureHandle.reserve(6);
	for (int i = 0; i < directions.size(); i++)
	{
		planetSideMeshFutureHandle.push_back(std::async(std::launch::async, [] 
			(Planet* planet, unsigned int resolution, std::vector<INoiseSettings*> settings,
			std::vector<Planet::VerticesData*>* futureData, glm::vec3 direction, std::mutex* mutex)
			{
				Planet::VerticesData* face = planet->CreatePlanetSide(resolution, settings, direction);
				std::lock_guard < std::mutex > lock(*mutex);
				futureData->push_back(face);
			}, 
			this, resolution, noiseSettings, &meshDataPerFace, directions[i], &sideCreation_Mutex));
	}
	

#else
	//Not threaded
	for (int i = 0; i < directions.size(); i++)
	{
		Planet::VerticesData* face = CreatePlanetSide(resolution, noiseSettings, directions[i]);
		meshGLIDs.push_back(BindPlanetSide(face));
	}
	isBusy = false;
	triCount = meshGLIDs[0].triCount * 6;

#endif 
}

void Planet::SetTexture(PlanetTexture& texture)
{
	if (!textureID)
	{
	//	glDeleteTextures(1, &textureID);
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	for (unsigned int i = 0; i < shaders.size(); i++)
	{
		shaders[i]->SetInt(SHADER_UNIFORM::BIOMES_COUNT, texture.biomes.size());
	}

	std::vector<glm::lowp_u8vec3> textureData;
	unsigned int highestMarkCount = 0;
	for (unsigned int i = 0; i < texture.biomes.size(); i++)
	{
		if (highestMarkCount < texture.biomes[i].getMarks().size())
		{
			highestMarkCount = texture.biomes[i].getMarks().size();
		}
		
	}
	unsigned int height = resolution * highestMarkCount;
	unsigned int width = texture.biomes.size();
	textureData.reserve((size_t)height * (size_t)width);
	
	for (int y = 0; y < height; y++)
	{
		double pos = (float)(y + 1) / (float)height;
		for (int x = 0; x < width; x++)
		{
			float color[3];
			texture.biomes[x].getColorAt(pos, color);
			ImColor tintedColor = ImColor(color[0] * 255, color[1] * 255, color[2] * 255);
			textureData.emplace_back(tintedColor.Value.x, tintedColor.Value.y, tintedColor.Value.z);
		}
	}
	
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER , GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);//Note: this is padding, default is for
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &textureData[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Planet::VerticesData* Planet::CreatePlanetSide(unsigned int resolution, std::vector<INoiseSettings*> noiseSettings,glm::vec3 localUp)
{
	Noise noise;
	VerticesData* data = new VerticesData(resolution);
	glm::vec3 axisA = glm::vec3(localUp.y, localUp.z, localUp.x);
	glm::vec3 axisB = glm::cross(localUp, axisA);

	unsigned int triIndex = 0;

	for (unsigned int y = 0; y < resolution; y++)
	{
		for (unsigned int x = 0; x < resolution; x++)
		{
			unsigned int i = x + y * resolution;
			glm::vec2 percent = glm::vec2(x, y);
			percent /= (resolution - 1);
			glm::vec3 pointOnUnitCube = localUp + (percent.x - 0.5f) * 2 * axisA + (percent.y - 0.5f) * 2 * axisB;
			glm::vec3 pointOnUnitSphere = glm::normalize(pointOnUnitCube);
			float elevation = 0;
			float firstLayerValue = 0;
			for (unsigned int noiseFilterIndex = 0; noiseFilterIndex < noiseSettings.size(); noiseFilterIndex++)
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
			elevation = 1 + elevation;
			
			if (i==0)
			{
				elevationMinMax = glm::vec2(elevation);
			}
			else
			{
				if (elevation < elevationMinMax.x)
				{
					elevationMinMax.x = elevation;
				}
				if (elevation > elevationMinMax.y)
				{
					elevationMinMax.y = elevation;
				}
			}
			
			data->vertices[i] = pointOnUnitSphere *elevation;

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
	for (unsigned int i = 0; i < data->indices.size(); i += 3)
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
	for (unsigned int i = 0; i < normals.size(); i++)
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
#if THREADED == 1
	if (meshDataPerFace.size() == 6)
	{
		Unbind();
		meshGLIDs.reserve(6);
		for (int i = 0; i < meshDataPerFace.size(); i++)
		{
			meshGLIDs.push_back(BindPlanetSide(meshDataPerFace[i]));
		}
		meshDataPerFace.clear();
		for (unsigned int i = 0; i < shaders.size(); i++)
		{
			shaders[i]->SetVec2(SHADER_UNIFORM::ELEVATION_MIN_MAX_POSITION, elevationMinMax);
		}
		triCount = meshGLIDs[0].triCount * 6;
		isBusy = false;
	}
#endif

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	for (int i = 0; i < meshGLIDs.size(); i++)
	{
		GLCall(glBindVertexArray(meshGLIDs[i].vertexArrayObjectID));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshGLIDs[i].indexBufferID));
		GLCall(glDrawElements(GL_TRIANGLES, meshGLIDs[i].triCount, GL_UNSIGNED_INT, nullptr));
		GLCall(glBindVertexArray(0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0));
	}

}

void Planet::Unbind()
{
	for (int i = 0; i < meshGLIDs.size(); i++)
	{
		GLCall(glBindVertexArray(0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

		GLCall(glDeleteVertexArrays(1, &meshGLIDs[i].vertexArrayObjectID));
		GLCall(glDeleteBuffers(1, &meshGLIDs[i].vertexBufferID));
		GLCall(glDeleteBuffers(1, &meshGLIDs[i].indexBufferID));
	}
	meshGLIDs.clear();
}

unsigned int Planet::GetTriCount()
{
	
	return triCount;
	
}
