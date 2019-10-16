#include <iostream>
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>



#include "TerrainFace.h"
#include "Shader.h" 
#include "Log.h"
#include "Camera.h"
#include "Window.h"
#include "Input.h"
#include "PyramidTest.h"
#include "Noise.h"


int main(void)
{
	Window window = Window(1366, 768);
	
	std::vector<TerrainFace> faces;
	faces.reserve(6);
	
	{
		Noise noise;
		std::vector<NoiseSettings> noiseSettings;
		
		noiseSettings.emplace_back();
		noiseSettings[0].strength = 0.12f;
		noiseSettings[0].numberOfLayers = 4;
		noiseSettings[0].baseRoughness = 0.55f;
		noiseSettings[0].roughness = 2.34f;
		noiseSettings[0].persistence = 0.54f;
		noiseSettings[0].center = glm::vec3(0.0f);
		noiseSettings[0].minValue = 1.06f;
		
		
		noiseSettings.emplace_back();
		noiseSettings[1].strength = 0.36f;
		noiseSettings[1].numberOfLayers = 5;
		noiseSettings[1].baseRoughness = 1.08f;
		noiseSettings[1].roughness = 2.34f;
		noiseSettings[1].persistence = 0.5f;
		noiseSettings[1].center = glm::vec3(0.0f);
		noiseSettings[1].minValue = 1.25f;
		

		std::vector<glm::vec3> directions;
		directions.reserve(6);
		directions.emplace_back(0.0f, 1.0f, 0.0f);
		directions.emplace_back(0.0f, -1.0f, 0.0f);
		directions.emplace_back(-1.0f, 0.0f, 0.0f);
		directions.emplace_back(1.0f, 0.0f, 0.0f);
		directions.emplace_back(0.0f, 0.0f, 1.0f);
		directions.emplace_back(0.0f, 0.0f, -1.0f);

		size_t resolution = 100;

		for (int i = 0; i < directions.size(); i++)
		{
			faces.emplace_back(resolution, &directions[i],&noise, &noiseSettings);
		}

		directions.clear();
	}


	using namespace std::placeholders;
	Camera camera = Camera(45.0f, window.getAspectRatio(), 0.01f, 100.0f);
	Input* inputManager = window.getInputManger();
	inputManager->registerKeyInputCallback(&camera);
	inputManager->registerMousePositionInputCallback(&camera);

	glm::mat4 model(1.0f);
	model = glm::translate(model,glm::vec3(0.0f, 0.0f, -1.9f));
	model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));

	Shader shader = Shader("res/shaders/Planet.shader");
	shader.UseShader();

	/* Loop until the user closes the window */
	while (!window.isClosing())
	{
		window.clearColor();

		shader.SetModelMatrix(model);
		shader.SetViewMatrix(camera.GetViewMatrix());
		shader.SetProjectionMatrix(camera.GetProjectionMatrix());
		
		for (int i = 0; i < faces.size(); i++)
		{
			faces[i].Draw();
		}

		window.swapBuffers();
	}

	return 0;
}
