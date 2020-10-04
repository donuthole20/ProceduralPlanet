#pragma once
#include <GL/glew.h>
#include <glm.hpp>
#include <vector>
#include <future>


#include "Shader.h"
#include "NoiseFilter.h"
#include "PlanetTexture.h"

class Planet
{
private:
	struct VerticesData
	{
		std::vector<glm::vec3> position;
		std::vector<glm::vec3> normals;
		std::vector<float> elevations;
		std::vector<uint32_t> indices;
		VerticesData(uint32_t resolution)
		{
			position = std::vector<glm::vec3>(resolution * resolution, glm::vec3(0, 0, 0));
			normals = std::vector<glm::vec3>(resolution * resolution, glm::vec3(0, 0, 0));
			elevations = std::vector<float>(resolution * resolution, 0.0f);
			indices = std::vector<uint32_t>(((resolution - 1) * (resolution - 1) * 6), 0);
		}
	};

	struct GLIDs
	{
		uint32_t vertexBufferID;
		uint32_t indexBufferID;
		uint32_t vertexArrayObjectID;
		uint32_t triCount;
	};
private:
	bool isBusy;
	std::mutex sideCreation_Mutex;
	std::vector<VerticesData*> meshDataPerFaceToBeBinded;
	std::vector<std::future<void>> planetSideMeshFutureHandle;
	std::vector<GLIDs> meshGLIDs;
	std::vector<glm::vec3> directions;
	glm::vec2 elevationMinMax;
	Shader shader;
	uint32_t textureID;//Temp
	uint32_t resolution;
	uint32_t triCount;

	VerticesData* CreatePlanetSide(uint32_t resolution, std::vector<NoiseSettings> noiseSettings, glm::vec3 localUp);
	GLIDs BindVertexData(VerticesData* vertices);

public:

	Planet();
	~Planet();

	void CreatePlanet(uint32_t resolution, std::vector<NoiseSettings> noiseSettings);
	void SetTexture(const PlanetTexture& texture);

	void Draw();

	void Unbind();
	bool IsBusy();
	uint32_t GetTriCount();
	Shader* GetShader();
	void SetModelMatrix(glm::mat4 model);
	void SetShaderTime(float time);
};


