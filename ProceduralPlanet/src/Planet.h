#pragma once
#include <GL/glew.h>
#include <glm.hpp>
#include <vector>
#include <future>


#include "Shader.h"
#include "NoiseFilter.h"

#include "externallibs/imgui/imgui_color_gradient.h"



struct Gradient
{
	ImGradient gradient;
	ImGradientMark* draggingMark = nullptr;
	ImGradientMark* selectedMark = nullptr;
};
struct PlanetTexture
{
	Gradient* waterGradient;
	std::vector<Gradient*> biomes;
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


