#pragma once
#include <GL/glew.h>
#include <glm.hpp>
#include <vector>
#include <future>


#include "Shader.h"
#include "NoiseFilter.h"

#include "externallibs/imgui/imgui_color_gradient.h"

class Planet
{
private:
	struct VerticesData
	{
		std::vector<glm::vec3> vertices;
		std::vector<unsigned int> indices;
		VerticesData(size_t resolution)
		{
			vertices = std::vector<glm::vec3>(resolution * resolution, glm::vec3(0, 0, 0));
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
	std::vector<Shader*> shaders;
	unsigned int textureID;//Temp
	size_t resolution;
	size_t triCount;


	VerticesData* CreatePlanetSide(size_t resolution, std::vector<INoiseSettings*> noiseSettings, glm::vec3 localUp);
	GLIDs BindPlanetSide(VerticesData* vertices);

public:

	Planet();
	~Planet();

	void CreatePlanet(size_t resolution, std::vector<INoiseSettings*> noiseSettings);
	void SetTexture(ImGradient& gradient);

	void Draw();

	void Unbind();
	bool IsBusy() { return isBusy; };
	void AddShader(Shader* shader) { shaders.push_back(shader); };
	size_t GetTriCount();

};


