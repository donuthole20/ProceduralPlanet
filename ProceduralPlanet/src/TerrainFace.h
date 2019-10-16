#pragma once

#include <GL/glew.h>
#include <glm.hpp>

#include "Noise.h"



struct NoiseSettings
{
public:
	size_t numberOfLayers = 1;
	float baseRoughness = 1;
	float strength = 1;
	float roughness = 2;
	float persistence = 0.5f;
	glm::vec3 center;
	float minValue = 1.0f;
};

class TerrainFace
{
	private:
		unsigned int vertexBufferID;
		unsigned int indexBufferID;
		unsigned int vertexArrayObjectID;
		unsigned int triCount;

	public:
		~TerrainFace();
		TerrainFace();
		TerrainFace(size_t resolution, glm::vec3* localUp,Noise* noise, std::vector<NoiseSettings>* noiseSettings);
		void Draw();
		void Delete();
};

