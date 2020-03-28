#pragma once

#include <cstdlib>
#include <time.h>  
#include "Noise.h"

enum class NoiseType
{
	Simple = 0, 
	Rigid = 1
};
static const char* NoiseTypeString[] = {"Simple","Rigid"};

class NoiseSettings
{
public:
	~NoiseSettings()
	{
	}
	size_t numberOfLayers = 1;
	float baseRoughness = 0.7f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.5f - 0.7f)));
	float strength = 0.1f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.3f - 0.1f)));
	float roughness = 2.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (3.0f - 2.0f)));
	float persistence = 0.5f;
	glm::vec3 center;
	float minValue = 1.0f;
	NoiseType type;
	float weightMultiplier = 0.8f;
	float Evaluate(Noise* noise, glm::vec3 point)
	{
		switch (type)
		{
		case NoiseType::Simple:
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

				noiseAmount =  noiseAmount - minValue;
				noiseAmount *= strength;
				return noiseAmount;
			}
		case NoiseType::Rigid:
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

				noiseAmount = noiseAmount - minValue;
				noiseAmount *= strength;
				return noiseAmount;
			}
		default:
			break;
		}
	}
	
	void Randomize()
	{
		baseRoughness = 0.7f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.5f - 0.7f)));
		strength = 0.1f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.3f - 0.1f)));
		roughness = 2.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (3.0f - 2.2f)));
		switch (type)
		{
			case NoiseType::Simple:
				minValue = 1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.3f - 1.0f)));
				break;
			case NoiseType::Rigid:
				minValue = 0.3f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.4f - 0.3f)));
				break;
		}
		
	}
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
	~RigidNoiseSettings()
	{
	}
	RigidNoiseSettings()
	{
		/*srand(time(NULL));
		strength = ((float)(rand() % 100)) / 100;
		numberOfLayers = rand() % 4;
		baseRoughness = ((float)(rand() % 200)) / 100;
		roughness = ((float)(rand() % 300)) / 100;
		persistence = ((float)(rand() % 80)) / 100;
		center = glm::vec3(0.0f);
		minValue = ((float)(rand() % 70)) / 100;
		weightMultiplier = ((float)(rand() % 100)) / 100;*/
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
	~SimpleNoiseSettings()
	{
	}
	SimpleNoiseSettings()
	{
		/*srand(time(NULL));
		strength = ((float)(rand() % 100)) / 100;
		numberOfLayers = rand() % 4;
		baseRoughness = ((float)(rand() % 200)) / 100;
		roughness = ((float)(rand() % 300)) / 100;
		persistence = ((float)(rand() % 80)) / 100;
		center = glm::vec3(0.0f);
		minValue = ((float)(rand() % 70)) / 100;*/
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
