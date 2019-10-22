#pragma once

#include "Noise.h"

enum NoiseType
{
	Simple, 
	Rigid
};
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
	NoiseType type;
	virtual float Evaluate(Noise* noise, glm::vec3 point) = 0;
};

class RigidNoiseSettings :public INoiseSettings
{
public:
	RigidNoiseSettings()
	{
		type = NoiseType::Rigid;
	}
	float weightMultiplier = 0.8f;
	float Evaluate(Noise* noise, glm::vec3 point)
	{
		float noiseAmount = 0;
		float frequency = baseRoughness;
		float amplitude = 1;
		float weight = 1;
		for (size_t layerIndex = 0; layerIndex < numberOfLayers; layerIndex++)
		{
			float v = 1 - glm::abs(noise->evaluate(point * frequency + center));
			v *= v;
			v *= weight;
			weight = glm::clamp(v * weightMultiplier, 0.0f, 1.0f);
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
	SimpleNoiseSettings()
	{
		type = NoiseType::Simple;
	}
	float Evaluate(Noise* noise, glm::vec3 point)
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
