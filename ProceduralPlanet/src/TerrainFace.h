#pragma once

#include <GL/glew.h>
#include <glm.hpp>

#include "Noise.h"



class INoiseSettings
{
public:
	size_t numberOfLayers = 1;
	float baseRoughness = 1;
	float strength = 1;
	float roughness = 2;
	float persistence = 0.5f;
	glm::vec3 center;
	float minValue = 1.0f;
	virtual float evaluate(Noise* noise, glm::vec3 point) = 0;
};

class RigidNoiseSettings :public INoiseSettings
{
public:
	float weightMultiplier = 0.8f;
	float evaluate(Noise* noise, glm::vec3 point)
	{
		float noiseAmount = 0;
		float frequency = baseRoughness;
		float amplitude = 1;
		float weight = 1;
		for (size_t layerIndex = 0; layerIndex < numberOfLayers; layerIndex++)
		{
			float v = 1- glm::abs(noise->evaluate(point * frequency + center));
			v *= v;
			v *= weight;
			weight = glm::clamp(v * weightMultiplier,0.0f,1.0f);
			noiseAmount += v * amplitude;
			frequency *= roughness;
			amplitude *= persistence;
		}

		noiseAmount = glm::max(0.0f, noiseAmount - minValue);
		noiseAmount *= strength;
		return noiseAmount;

	}
};


class SimpleNoiseSettings :public INoiseSettings
{
public:
	float evaluate(Noise* noise, glm::vec3 point)
	{
		float noiseAmount = 0;
		float frequency = baseRoughness;
		float amplitude = 1;
		for (size_t layerIndex = 0; layerIndex < numberOfLayers; layerIndex++)
		{
			float v = noise->evaluate(point * frequency + center);
			noiseAmount += (v + 1) * 0.5f * amplitude;
			frequency *= roughness;
			amplitude *= persistence;
		}

		noiseAmount = glm::max(0.0f, noiseAmount - minValue);
		noiseAmount *= strength;
		return noiseAmount;

	}
};

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

	public:
		~TerrainFace();
		TerrainFace(size_t resolution);
		void createMesh(glm::vec3* localUp, Noise* noise, std::vector<INoiseSettings*>* noiseSettings);
		void bindToGPU();
		void draw();
		void deleteFromGPU();
};

