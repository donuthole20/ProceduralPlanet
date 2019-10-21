#pragma once
#include <GL/glew.h>
#include <glm.hpp>
#include <vector>

#include "TerrainFace.h"

class Planet
{
private:
	
	struct PlanetGLData
	{
		unsigned int vertexBufferID;
		unsigned int indexBufferID;
		unsigned int vertexArrayObjectID;
		unsigned int triCount;
		PlanetGLData() :
			vertexBufferID(0),
			indexBufferID(0),
			vertexArrayObjectID(0),
			triCount(0)
		{
		}
	};
	struct PlanetFacesVertices
	{
		std::vector<glm::vec3> positions;
		std::vector<float> elevations;
		std::vector<unsigned int> indices;
		PlanetFacesVertices(size_t resolution)
			:positions(std::vector<glm::vec3>(resolution* resolution, glm::vec3(0, 0, 0))),
			elevations(std::vector<float>(resolution* resolution, 0.0f)),
			indices(std::vector<unsigned int>(((resolution - 1)* (resolution - 1) * 6), 0))
		{}
	};

	Noise noise;
	size_t resolution;

	std::vector <std::vector<glm::vec3>> spherePositions;
	std::vector <std::vector<float>> elevations;
	std::vector <std::vector<unsigned int>> indices;
		
	std::vector<PlanetGLData> facesGLData;

	std::vector<INoiseSettings*> settings;

	Planet::PlanetFacesVertices CalculateSphereFacePositions(glm::vec3* localUp);
	std::vector<float> CalculateSphereFaceElevations(size_t index);
	float CalculatePointElevation(glm::vec3* point);
	std::vector <std::vector<glm::vec3>> CalculateNormals();
	void BindToGPU();

public:
	~Planet();
	Planet(size_t resolution, std::vector<INoiseSettings*> settings);
	void CreatePlanet();
	void SetResolution(size_t resolution);
	void SetSettings(std::vector<INoiseSettings> settings);
	void Draw();
	void Delete();
	void DeleteFromGPU();
};


