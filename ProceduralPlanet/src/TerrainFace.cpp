#include "TerrainFace.h"
#include <vector>

#include "OpenGLErrorHandler.h"

TerrainFace::TerrainFace(size_t resolution)
:resolution(glm::min(glm::abs((int)resolution), 250))
{
	positions = std::vector<glm::vec3>(resolution * resolution, glm::vec3(0, 0, 0));
	indices = std::vector<unsigned int>(((resolution - 1) * (resolution - 1) * 6), 0);
}
//#include <iostream>
TerrainFace::~TerrainFace()
{
	deleteFromGPU();
}

void TerrainFace::createMesh(glm::vec3* localUp, Noise* noise, std::vector<INoiseSettings*>* noiseSettings)
{
	
	glm::vec3 axisA = glm::vec3(localUp->y, localUp->z, localUp->x);
	glm::vec3 axisB = glm::cross(*localUp, axisA);


	size_t triIndex = 0;

	std::vector<INoiseSettings*>& noiseFiltersSettings = *noiseSettings;

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
				float noiseAmount = noiseFiltersSettings[noiseFilterIndex]->evaluate(noise,pointOnUnitSphere);
				if( noiseFilterIndex ==0)
				{
					firstLayerValue = noiseAmount;
					elevation += noiseAmount;
				}
				else
				{

					elevation += (noiseAmount* firstLayerValue);
				}
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

}

void TerrainFace::bindToGPU()
{
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



void TerrainFace::draw()
{
	GLCall(glBindVertexArray(vertexArrayObjectID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID));
	GLCall(glDrawElements(GL_TRIANGLES, triCount, GL_UNSIGNED_INT, nullptr));
}

void TerrainFace::deleteFromGPU()
{
	GLCall(glDeleteVertexArrays(1,&vertexArrayObjectID));
	GLCall(glDeleteBuffers(1,&vertexBufferID));
	GLCall(glDeleteBuffers(1, &indexBufferID));
}
