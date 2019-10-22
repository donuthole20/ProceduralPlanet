#pragma once
#include <GL/glew.h>
#include <glm.hpp>
#include <vector>

#include "TerrainFace.h"

class Planet
{

private:
	std::vector<TerrainFace> faces;
public:
	Planet();
	~Planet();

	void CreatePlanet(size_t* resolution, std::vector<INoiseSettings*>* noiseSettings);

	void Draw();

	void Unbind();

};


