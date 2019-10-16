#include <windows.h>
#include <iostream>
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <thread>



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
#ifdef RELEASE
	ShowWindow( GetConsoleWindow(), SW_HIDE );
#endif 


	Window window = Window(1366, 768);
	
	std::vector<TerrainFace> faces;
	faces.reserve(6);
	
	{
		Noise noise;
		std::vector<INoiseSettings*> noiseSettings;
		

		SimpleNoiseSettings settings1;
		settings1.strength = 0.15f;
		settings1.numberOfLayers = 4;
		settings1.baseRoughness = 1.15f;
		settings1.roughness = 2.2f;
		settings1.persistence = 0.5f;
		settings1.center = glm::vec3(1.11f,0.92f,-0.39f);
		settings1.minValue = 0.98f;
		noiseSettings.push_back(&settings1);
		
		SimpleNoiseSettings settings2;
		settings2.strength = 0.36f;
		settings2.numberOfLayers = 5;
		settings2.baseRoughness = 1.08f;
		settings2.roughness = 2.34f;
		settings2.persistence = 0.5f;
		settings2.center = glm::vec3(0.0f);
		settings2.minValue = 1.25f;
		noiseSettings.push_back(&settings2);

		RigidNoiseSettings settings3;
		settings3.strength = 1.0f;
		settings3.numberOfLayers = 4;
		settings3.baseRoughness = 1.59f;
		settings3.roughness = 3.3f;
		settings3.persistence = 0.5f;
		settings3.center = glm::vec3(0.0f);
		settings3.minValue = 0.37;
		settings3.weightMultiplier = 0.78f;
		noiseSettings.push_back(&settings3);
		

		std::vector<glm::vec3> directions;
		directions.reserve(faces.size());
		directions.emplace_back(0.0f, 1.0f, 0.0f);
		directions.emplace_back(0.0f, -1.0f, 0.0f);
		directions.emplace_back(-1.0f, 0.0f, 0.0f);
		directions.emplace_back(1.0f, 0.0f, 0.0f);
		directions.emplace_back(0.0f, 0.0f, 1.0f);
		directions.emplace_back(0.0f, 0.0f, -1.0f);

		size_t resolution = 100;
		

		//Not threaded
		/*
		for (int i = 0; i < directions.size(); i++)
		{
			faces.emplace_back(resolution);
			faces[i].createMesh(&directions[i], &noise, &noiseSettings);
			faces[i].bindToGPU();
				
		}
		*/

		//threaded
		std::vector<std::thread> workers;
		workers.reserve(faces.size());
		for (int i = 0; i < directions.size(); i++)
		{
			faces.emplace_back(resolution);
			workers.emplace_back([&faces,i, &directions, &noise, &noiseSettings] {
				faces[i].createMesh(&directions[i], &noise, &noiseSettings);
				});
		}

		for (int i = 0; i < faces.size(); i++)
		{
			workers[i].join();
			faces[i].bindToGPU();

		}
		workers.clear();
		
	

		noiseSettings.clear();
		directions.clear();
	}


	using namespace std::placeholders;
	Camera camera = Camera(45.0f, window.getAspectRatio(), 0.01f, 100.0f);
	Input* inputManager = window.getInputManger();
	//inputManager->registerKeyInputCallback(&camera);
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
			faces[i].draw();
		}

		window.swapBuffers();
	}

	return 0;
}
