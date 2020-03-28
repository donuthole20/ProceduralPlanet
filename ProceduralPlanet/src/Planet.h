#pragma once
#include <GL/glew.h>
#include <glm.hpp>
#include <vector>
#include <future>


#include "Shader.h"
#include "NoiseFilter.h"

#include "externallibs/imgui/imgui_color_gradient.h"



struct PlanetGradient
{
	ImGradient gradient;
	ImGradientMark* draggingMark = nullptr;
	ImGradientMark* selectedMark = nullptr;
	PlanetGradient()
	{
		ImGradientMark* firstMark = gradient.getMarks().back();
		ImGradientMark* secondMark = gradient.getMarks().front();

		firstMark->color[0] = std::max(0.0f, static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.2f);
		firstMark->color[1] = std::max(0.0f, static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.2f);
		firstMark->color[2] = std::max(0.0f, static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.2f);
		firstMark->color[3] = 1.0f;

		secondMark->color[0] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		secondMark->color[1] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		secondMark->color[2] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		secondMark->color[3] = 1.0f;

		gradient.refreshCache();
	}
	void Randomize()
	{
		for (ImGradientMark* mark : gradient.getMarks())
		{
			mark->color[0] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			mark->color[1] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			mark->color[2] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			mark->color[3] = 1.0f;
		}
		gradient.refreshCache();
	}
	//TODO: Make this random
};
struct PlanetTexture
{
	PlanetGradient* waterGradient;
	std::vector<PlanetGradient*> biomes;
	~PlanetTexture()
	{
		delete waterGradient;
		for (size_t i = 0; i < biomes.size(); i++)
		{
			delete biomes[i];
		}
	}
};

class Planet
{
private:
	struct VerticesData
	{
		std::vector<glm::vec3> position;
		std::vector<glm::vec3> normals;
		std::vector<float> elevations;
		std::vector<unsigned int> indices;
		VerticesData(unsigned int resolution)
		{
			position = std::vector<glm::vec3>(resolution * resolution, glm::vec3(0, 0, 0));
			normals = std::vector<glm::vec3>(resolution * resolution, glm::vec3(0, 0, 0));
			elevations = std::vector<float>(resolution * resolution, 0.0f);
			indices = std::vector<unsigned int>(((resolution - 1) * (resolution - 1) * 6), 0);
		}
	};

	struct GLIDs
	{
		unsigned int vertexBufferID;
		unsigned int indexBufferID;
		unsigned int vertexArrayObjectID;
		unsigned int triCount;
	};
private:
	bool isBusy;
	std::mutex sideCreation_Mutex;
	std::vector<VerticesData*> meshDataPerFace;
	std::vector<std::future<void>> planetSideMeshFutureHandle;
	std::vector<GLIDs> meshGLIDs;
	std::vector<glm::vec3> directions;
	glm::vec2 elevationMinMax;
	Shader shader;
	unsigned int textureID;//Temp
	unsigned int resolution;
	unsigned int triCount;

	VerticesData* CreatePlanetSide(unsigned int resolution, std::vector<NoiseSettings> noiseSettings, glm::vec3 localUp);
	GLIDs BindVertexData(VerticesData* vertices);

public:

	Planet();
	~Planet();

	void CreatePlanet(unsigned int resolution, std::vector<NoiseSettings> noiseSettings);
	void SetTexture(PlanetTexture& texture);

	void Draw();

	void Unbind();
	bool IsBusy();
	unsigned int GetTriCount();
	Shader* GetShader();
	void SetModelMatrix(glm::mat4 model);
	void SetShaderTime(float time);
};


