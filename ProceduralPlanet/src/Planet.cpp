#include "Planet.h"
#include <vector>
#include <thread>

#include "OpenGLErrorHandler.h"
#include "Log.h"
#include "ConfigMacros.h"

Planet::Planet()
{
	faces.reserve(6);
}

Planet::~Planet()
{
	faces.clear();
	Unbind();
}

void Planet::CreatePlanet(size_t* resolution, std::vector<INoiseSettings*>* noiseSettings)
{
	Unbind();
	faces.clear();

	Noise noise;

	std::vector<TerrainFace> tempFaces;
	tempFaces.reserve(6);
	std::vector<glm::vec3> directions;
	directions.reserve(faces.size());
	directions.emplace_back(0.0f, 1.0f, 0.0f);
	directions.emplace_back(0.0f, -1.0f, 0.0f);
	directions.emplace_back(-1.0f, 0.0f, 0.0f);
	directions.emplace_back(1.0f, 0.0f, 0.0f);
	directions.emplace_back(0.0f, 0.0f, 1.0f);
	directions.emplace_back(0.0f, 0.0f, -1.0f);

#if THREADED == 1
	std::vector<std::thread> workers;
	workers.reserve(tempFaces.size());
	for (int i = 0; i < directions.size(); i++)
	{
		tempFaces.emplace_back(*resolution);
		workers.emplace_back([&tempFaces, i, &directions, &noise, &noiseSettings] {
			tempFaces[i].CreateMesh(&directions[i], &noise, noiseSettings);
			});
	}

	for (int i = 0; i < workers.size(); i++)
	{
		workers[i].join();
		tempFaces[i].BindToGPU();
	}

	faces = tempFaces;
#else
	//Not threaded
	for (int i = 0; i < directions.size(); i++)
	{
		faces.emplace_back(*resolution);
		faces[i].CreateMesh(&directions[i], &noise, noiseSettings);
		faces[i].BindToGPU();
	}

#endif 


}

void Planet::Draw()
{
	for (int i = 0; i < faces.size(); i++)
	{
		faces[i].Draw();
	}
}



void Planet::Unbind()
{
	for (int i = 0; i < faces.size(); i++)
	{
		faces[i].DeleteFromGPU();
	}
}
