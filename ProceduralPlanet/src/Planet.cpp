#include "Planet.h"
#include <thread>

#include "OpenGLErrorHandler.h"
#include "Log.h"
#include "ConfigMacros.h"

Planet::Planet() :
	isBusy(false),
	shader("Planet.shader", PreCompileShaderVariable(1))//TODO clean this up
{
	meshGLIDs.reserve(6);
	directions.reserve(6);
	directions.emplace_back(0.0f, 1.0f, 0.0f);
	directions.emplace_back(0.0f, -1.0f, 0.0f);
	directions.emplace_back(-1.0f, 0.0f, 0.0f);
	directions.emplace_back(1.0f, 0.0f, 0.0f);
	directions.emplace_back(0.0f, 0.0f, 1.0f);
	directions.emplace_back(0.0f, 0.0f, -1.0f);

	shader.Bind();
	shader.SetVec3(SHADER_UNIFORM::ALBEDO, glm::vec3(1.0f));
	shader.SetFloat(SHADER_UNIFORM::AO, 1.0f);
	shader.SetFloat(SHADER_UNIFORM::METALLIC, 0.8f);
	shader.SetFloat(SHADER_UNIFORM::ROUGHNESS, 0.8f);
	shader.SetVec3(SHADER_UNIFORM::LIGHT_POSITIONS, glm::vec3(-2.0f, 4.0f, 0.0f));
	shader.SetVec3(SHADER_UNIFORM::LIGHT_COLORS, glm::vec3(30.0f));
	shader.Unbind();
}

Planet::~Planet()
{
	Unbind();
	meshGLIDs.clear();
}

void Planet::CreatePlanet(uint32_t resolution, std::vector<NoiseSettings> noiseSettings)
{
	if (isBusy)
	{
		return;
	}

	isBusy = true;
	this->resolution = resolution;
	elevationMinMax = glm::vec2(0.0f);

#if THREADED == 1
	meshDataPerFaceToBeBinded.clear();
	meshDataPerFaceToBeBinded.reserve(6);
	planetSideMeshFutureHandle.clear();
	planetSideMeshFutureHandle.reserve(6);
	for (int i = 0; i < directions.size(); i++)
	{
		planetSideMeshFutureHandle.push_back(std::async(std::launch::async, [] 
			(Planet* planet, uint32_t resolution, std::vector<NoiseSettings> settings,
			std::vector<Planet::VerticesData*>* futureData, glm::vec3 direction, std::mutex* mutex)
			{
				Planet::VerticesData* face = planet->CreatePlanetSide(resolution, settings, direction);
				std::lock_guard < std::mutex > lock(*mutex);
				futureData->push_back(face);
			}, 
			this, resolution, noiseSettings, &meshDataPerFaceToBeBinded, directions[i], &sideCreation_Mutex));
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

void Planet::SetTexture(const PlanetTexture& texture)
{
	if (!textureID)
	{
	//	glDeleteTextures(1, &textureID);//TODO do you need to delete texture every time you make one?
		glGenTextures(1, &textureID);
	}

	glBindTexture(GL_TEXTURE_2D, textureID);

	std::vector<glm::lowp_u8vec3> textureData;
	uint32_t highestMarkCount = 0;
	for (uint32_t i = 0; i < texture.biomes.size(); i++)
	{
		if (highestMarkCount < texture.biomes[i]->gradient.getMarks().size())
		{							 
			highestMarkCount = texture.biomes[i]->gradient.getMarks().size();
		}
		
	}
	uint32_t height = texture.biomes.size();
	uint32_t textureResolution = resolution * highestMarkCount;
	uint32_t width = textureResolution * 2;
	textureData.reserve((size_t)height * (size_t)width);
	
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			
			float color[3];
			if (x < textureResolution-1)
			{
				double pos = (float)(x) / ((float)textureResolution-1);
				texture.waterGradient->gradient.getColorAt(pos, color);
			}
			else
			{
				double pos = (float)(x-(textureResolution - 1))/ ((float)textureResolution - 1);
				texture.biomes[y]->gradient.getColorAt(pos, color);
			}
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

Planet::VerticesData* Planet::CreatePlanetSide(uint32_t resolution, std::vector<NoiseSettings> noiseSettings,glm::vec3 localUp)
{
	Noise noise;
	VerticesData* data = new VerticesData(resolution);
	glm::vec3 axisA = glm::vec3(localUp.y, localUp.z, localUp.x);
	glm::vec3 axisB = glm::cross(localUp, axisA);

	uint32_t triIndex = 0;

	for (uint32_t y = 0; y < resolution; y++)
	{
		for (uint32_t x = 0; x < resolution; x++)
		{
			uint32_t i = x + y * resolution;
			glm::vec2 percent = glm::vec2(x, y);
			percent /= (resolution - 1);
			glm::vec3 pointOnUnitCube = localUp + (percent.x - 0.5f) * 2 * axisA + (percent.y - 0.5f) * 2 * axisB;
			glm::vec3 pointOnUnitSphere = glm::normalize(pointOnUnitCube);
			float elevation = 0;
			float firstLayerValue = 0;
			for (uint32_t noiseFilterIndex = 0; noiseFilterIndex < noiseSettings.size(); noiseFilterIndex++)
			{
				float noiseAmount = noiseSettings[noiseFilterIndex].Evaluate(&noise, pointOnUnitSphere);
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

			data->elevations[i] = elevation;
			elevation = glm::max(0.0f, elevation);
			elevation += 1;
			data->position[i] = pointOnUnitSphere * elevation;

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

	
	for (uint32_t i = 0; i < data->indices.size(); i += 3)
	{
		uint32_t in0 = data->indices[i];
		uint32_t in1 = data->indices[i + 1];
		uint32_t in2 = data->indices[i + 2];
		glm::vec3 v1(data->position[in1] - data->position[in0]);
		glm::vec3 v2(data->position[in2] - data->position[in0]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		data->normals[in0] += normal;
		data->normals[in1] += normal;
		data->normals[in2] += normal;
	}

	/*std::vector<glm::vec3> combinedVertexData;
	combinedVertexData.reserve(data->position.size() * 2);
	for (uint32_t i = 0; i < data->position.size(); i++)
	{
		combinedVertexData.emplace_back(data->position[i]);
		combinedVertexData.emplace_back(glm::normalize(data->normals[i]));
	}
	data->position.clear();
	data->position = combinedVertexData;*/

	return data;
}

Planet::GLIDs Planet::BindVertexData(Planet::VerticesData* vertices)
{
	GLIDs id;
	GLCall(glGenVertexArrays(1, &id.vertexArrayObjectID));
	GLCall(glBindVertexArray(id.vertexArrayObjectID));

	std::vector <float> combinedVertexData;
	combinedVertexData.reserve((vertices->position.size() * 3 * 2) /*+ vertices->elevations.size()*/);
	float min = 0.0f;
	float max = 0.0f;
	for (uint32_t i =0;i< vertices->position.size();i++)
	{
		combinedVertexData.emplace_back(vertices->position[i].x);
		combinedVertexData.emplace_back(vertices->position[i].y);
		combinedVertexData.emplace_back(vertices->position[i].z);
		if (vertices->position[i].y > max)
		{
			max = vertices->position[i].y;
		}
		if (vertices->position[i].y < min)
		{
			min = vertices->position[i].y;
		}
		glm::vec3 normal = glm::normalize(vertices->normals[i]);
		combinedVertexData.emplace_back(normal.x);
		combinedVertexData.emplace_back(normal.y);
		combinedVertexData.emplace_back(normal.z);
		combinedVertexData.emplace_back(vertices->elevations[i]);
	}

	GLCall(glGenBuffers(1, &id.vertexBufferID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, id.vertexBufferID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * combinedVertexData.size(), &combinedVertexData[0], GL_STATIC_DRAW));//size of array * 3 for 3 floats in a vertex

	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(sizeof(float) * 3)));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(sizeof(float) * 6)));
	GLCall(glEnableVertexAttribArray(2));

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));
	
	id.triCount = vertices->indices.size();
	GLCall(glGenBuffers(1, &id.indexBufferID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id.indexBufferID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * vertices->indices.size(), &vertices->indices[0], GL_STATIC_DRAW));

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	return id;
}

void Planet::Draw()
{
	shader.Bind();

#if THREADED == 1
	if (meshDataPerFaceToBeBinded.size() == 6)
	{
		Unbind();
		meshGLIDs.reserve(6);
		for (int i = 0; i < meshDataPerFaceToBeBinded.size(); i++)
		{
			meshGLIDs.push_back(BindVertexData(meshDataPerFaceToBeBinded[i]));
			delete meshDataPerFaceToBeBinded[i];
		}
		
		meshDataPerFaceToBeBinded.clear();
		shader.SetVec2(SHADER_UNIFORM::ELEVATION_MIN_MAX_POSITION, elevationMinMax);
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
	shader.Unbind();
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

bool Planet::IsBusy()
{
	return isBusy;
}

uint32_t Planet::GetTriCount()
{
	
	return triCount;
	
}

Shader* Planet::GetShader()
{
	return &shader;
}

void Planet::SetModelMatrix(glm::mat4 model)
{
	shader.Bind();
	shader.SetMat4x4(SHADER_UNIFORM::MODEL, model);
	shader.Unbind();
}

void Planet::SetShaderTime(float time)
{
	shader.Bind();
	shader.SetFloat(SHADER_UNIFORM::TIME, time);
	shader.Unbind();
}
