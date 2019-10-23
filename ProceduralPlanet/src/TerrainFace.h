#pragma once

#include <GL/glew.h>
#include <glm.hpp>


#include "NoiseFilter.h"


class TerrainFace
{
	private:
		unsigned int vertexBufferID;
		unsigned int indexBufferID;
		unsigned int vertexArrayObjectID;
		unsigned int triCount;

		size_t resolution;
		std::vector<glm::vec3> positions;
		std::vector<unsigned int> indices;

		void CalculateAverageNormals();
	public:
		~TerrainFace();
		TerrainFace(size_t resolution);
		void CreateMesh(glm::vec3* localUp, Noise* noise, std::vector<INoiseSettings*>* noiseSettings);
		void BindToGPU();
		void Draw();
		void DeleteFromGPU();
};

