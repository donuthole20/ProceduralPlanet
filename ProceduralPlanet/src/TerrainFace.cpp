#include "TerrainFace.h"
#include <vector>

#include "OpenGLErrorHandler.h"
TerrainFace::TerrainFace()
{
}
//#include <iostream>
TerrainFace::~TerrainFace()
{
	Delete();
}

TerrainFace::TerrainFace(size_t resolution, glm::vec3* localUp, Noise* noise, std::vector<NoiseSettings>* noiseSettings)
{
	resolution = glm::min(glm::abs((int)resolution), 250);
	glm::vec3 axisA = glm::vec3(localUp->y, localUp->z, localUp->x);
	glm::vec3 axisB = glm::cross(*localUp, axisA);

	std::vector<glm::vec3> positions(resolution * resolution, glm::vec3(0, 0, 0));
	std::vector<unsigned int> indices(((resolution - 1) * (resolution - 1) * 6), 0);

	size_t triIndex = 0;

	std::vector<NoiseSettings>& noiseFiltersSettings = *noiseSettings;

	for (size_t y = 0; y < resolution; y++)
	{
		for (size_t x = 0; x < resolution; x++)
		{
			size_t i = x + y * resolution;
			glm::vec2 percent = glm::vec2(x, y);
			percent /= (resolution - 1);
			glm::vec3 pointOnUnitCube = *localUp + (percent.x - 0.5f) * 2 * axisA + (percent.y - 0.5f) * 2 * axisB;
			glm::vec3 pointOnUnitSphere = glm::normalize(pointOnUnitCube);
			float elevation = 0;
			float firstLayerValue = 0;
			for (size_t noiseFilterIndex = 0; noiseFilterIndex < noiseFiltersSettings.size(); noiseFilterIndex++)
			{
				float noiseAmount = 0;
				float frequency = noiseFiltersSettings[noiseFilterIndex].baseRoughness;
				float amplitude = 1;
				for (size_t layerIndex = 0; layerIndex < noiseFiltersSettings[noiseFilterIndex].numberOfLayers; layerIndex++)
				{
					float v = noise->evaluate(pointOnUnitSphere * frequency + noiseFiltersSettings[noiseFilterIndex].center);
					noiseAmount += (v + 1) * 0.5f * amplitude;
					frequency *= noiseFiltersSettings[noiseFilterIndex].roughness;
					amplitude *= noiseFiltersSettings[noiseFilterIndex].persistence;
				}

				noiseAmount = glm::max(0.0f, noiseAmount - noiseFiltersSettings[noiseFilterIndex].minValue);
				noiseAmount *= noiseFiltersSettings[noiseFilterIndex].strength;
				
				if (noiseFilterIndex == 0)//TODO: might need to implement bool if you want layer to use mask
				{
					if (noiseAmount == 0)
					{
						break;
					}
				}

				elevation += noiseAmount;
			}

			positions[i] = pointOnUnitSphere * (1+elevation);
			//std::cout << glm::distance(glm::vec3(0.0), positions[i]) << "\n";
			//std::cout << pointOnUnitSphere.x << "," << pointOnUnitSphere.y << "," << pointOnUnitSphere.z << std::endl;
			if (x != resolution - 1 && y != resolution - 1)
			{
				indices[triIndex] = i;
				indices[triIndex + 1] = i + resolution + 1;
				indices[triIndex + 2] = i + resolution;

				indices[triIndex + 3] = i;
				indices[triIndex + 4] = i + 1;
				indices[triIndex + 5] = i + resolution + 1;
				triIndex += 6;
			}
		}
	}

	/*
	float a = *(float*)((char*)&positions[0] + (20));
	std::cout << a << ":" << positions[1].z << std::endl;


	for (size_t i = 0; i < positions.size(); i++)
	{
		float a = *(float*)((char*)&positions[0] + (4*(2+ (3*i) )));
		bool isEqual = a == positions[i].z;
		const char* text = (isEqual ? "True" : "False");
		//std::cout << text << std::endl;
		std::cout << positions[i].x<<":"<< positions[i].y<<":"<< positions[i].z << std::endl;
	}
	*///Note: way to test if the vector is tightly packed in memory



	GLCall(glGenVertexArrays(1, &vertexArrayObjectID));
	GLCall(glBindVertexArray(vertexArrayObjectID));

	GLCall(glGenBuffers(1, &vertexBufferID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size() * 3, &positions[0], GL_STATIC_DRAW));//size of array * 3 for 3 floats in a vertex
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0));
	GLCall(glEnableVertexAttribArray(0));

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));
	positions.clear();

	triCount = indices.size();
	GLCall(glGenBuffers(1, &indexBufferID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW));


	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	indices.clear();

}

void TerrainFace::Draw()
{
	GLCall(glBindVertexArray(vertexArrayObjectID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID));
	GLCall(glDrawElements(GL_TRIANGLES, triCount, GL_UNSIGNED_INT, nullptr));
}

void TerrainFace::Delete()
{
	GLCall(glDeleteVertexArrays(1,&vertexArrayObjectID));
	GLCall(glDeleteBuffers(1,&vertexBufferID));
	GLCall(glDeleteBuffers(1, &indexBufferID));
}
