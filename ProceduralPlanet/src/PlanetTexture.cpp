#include "PlanetTexture.h"

PlanetTexture::PlanetTexture()
{
	waterGradient = new PlanetGradient();

	biomes.reserve(3);
	biomes.push_back(new PlanetGradient());
	biomes.push_back(new PlanetGradient());
	biomes.push_back(new PlanetGradient());
}

PlanetTexture::~PlanetTexture()
{
	delete waterGradient;
	for (size_t i = 0; i < biomes.size(); i++)
	{
		delete biomes[i];
	}
}

void PlanetTexture::Randomize()
{
	waterGradient->Randomize();
	for (size_t i = 0; i < biomes.size(); i++)
	{
		biomes[i]->Randomize();
	}
}
