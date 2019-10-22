#pragma once
#include <GL/glew.h>
#include <glm.hpp>
#include <vector>

#include "TerrainFace.h"

class Planet
{

private:
	struct VerticesData
	{
		std::vector<glm::vec3> vertices;
		std::vector<unsigned int> indices;
		VerticesData(size_t resolution)
		{
			vertices = std::vector<glm::vec3>(resolution * resolution, glm::vec3(0, 0, 0));
			indices = std::vector<unsigned int>(((resolution - 1) * (resolution - 1) * 6), 0);
		}
	};

	struct GLIDs
	{
		unsigned int vertexBufferID;
		unsigned int indexBufferID;
		unsigned int vertexArrayObjectID;
		unsigned int triCount;
	};


	std::vector<GLIDs> planetSides;
public:
	Planet();
	~Planet();

	void CreatePlanet(size_t resolution, std::vector<INoiseSettings*> noiseSettings);
	VerticesData* CreatePlanetSide(size_t resolution, std::vector<INoiseSettings*> noiseSettings, glm::vec3 localUp);
	GLIDs BindPlanetSide(VerticesData* vertices);

	void Draw();

	void Unbind();

};


