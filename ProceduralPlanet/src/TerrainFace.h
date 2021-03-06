#pragma once

#include <GL/glew.h>
#include <glm.hpp>


#include "NoiseFilter.h"


class TerrainFace
{
	private:
		uint32_t vertexBufferID;
		uint32_t indexBufferID;
		uint32_t vertexArrayObjectID;
		size_t triCount;

		size_t resolution;
		std::vector<glm::vec3> positions;
		std::vector<size_t> indices;

		void CalculateAverageNormals();
	public:
		~TerrainFace();
		TerrainFace(size_t resolution);
		void CreateMesh(glm::vec3* localUp, Noise* noise, std::vector<INoiseSettings*>* noiseSettings);
		void BindToGPU();
		void Draw();
		void DeleteFromGPU();
};

