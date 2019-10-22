#include <windows.h>
#include <iostream>
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <thread>


#include "ConfigMacros.h"
#include "Log.h"

#include "TerrainFace.h"
#include "Shader.h" 
#include "Camera.h"
#include "Window.h"
#include "Input.h"
#include "PyramidTest.h"
#include "Noise.h"
#include "Planet.h"


#include "externallibs/imgui/imgui.h"
#include "externallibs/imgui/imgui_impl_glfw.h"
#include "externallibs/imgui/imgui_impl_opengl3.h"


int main(void)
{
#ifdef RELEASE
	ShowWindow( GetConsoleWindow(), SW_HIDE );
#endif 

	Window window = Window(1366, 768);

	
	size_t resolution = 50;

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
	settings3.minValue = 0.37f;
	settings3.weightMultiplier = 0.78f;
	noiseSettings.push_back(&settings3);

	Planet planet;
	planet.CreatePlanet(&resolution, &noiseSettings);
		

	Camera camera = Camera(45.0f, window.GetAspectRatio(), 0.01f, 100.0f);
	Input* inputManager = window.GetInputManger();
#if ENABLE_INPUT == 1
	inputManager->registerMousePositionInputCallback(&camera);
	inputManager->registerKeyInputCallback(&camera);
#endif 

	glm::mat4 model(1.0f);
	model = glm::translate(model,glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));

	Shader shader = Shader("res/shaders/Planet.shader");

	shader.UseShader();
	shader.SetMat4x4(SHADER_UNIFORM::PROJECTION, camera.GetProjectionMatrix());
	shader.SetVec3(SHADER_UNIFORM::ALBEDO, glm::vec3(1.0f));
	shader.SetFloat(SHADER_UNIFORM::AO, 1.0f);
	shader.SetFloat(SHADER_UNIFORM::METALLIC, 0.8f);
	shader.SetFloat(SHADER_UNIFORM::ROUGHNESS, 0.8f);
	shader.SetVec3(SHADER_UNIFORM::LIGHT_POSITIONS, glm::vec3(-2.0f, 4.0f, 0.0f));
	shader.SetVec3(SHADER_UNIFORM::LIGHT_COLORS, glm::vec3(30.0f));
#if ENABLE_INPUT == 0
	shader.SetMat4x4(SHADER_UNIFORM::VIEW, camera.GetViewMatrix());
	shader.SetVec3(SHADER_UNIFORM::CAMERA_POSITION, camera.GetPosition());
#endif 

	float lastTime = 0.0f;
	/* Loop until the user closes the window */
	while (!window.IsClosing())
	{
		window.ClearColor();
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		shader.UseShader();

#if ENABLE_INPUT == 1
		float now = window.getCurrentTime();
		float deltaTime = now - lastTime;
		lastTime = now;
		inputManager->updateKeyInput(deltaTime);
		shader.SetVec3(SHADER_UNIFORM::CAMERA_POSITION, camera.getPosition());
		shader.SetMat4x4(SHADER_UNIFORM::VIEW, camera.GetViewMatrix());
#endif 
		
		model = glm::rotate(model, glm::radians(0.2f), glm::vec3(0.8f, 1.0f, 0.0f));
		shader.SetMat4x4(SHADER_UNIFORM::MODEL, model);



		{
			ImGui::Begin("FPS COUNTER");                        
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			static int f = resolution;
			if (ImGui::Button("Generate"))
			{
				if (resolution != f)
				{
					resolution = f;
					planet.CreatePlanet(&resolution, &noiseSettings);
				}
			}
			ImGui::SliderInt("Resolution", &f, 2, 1000);
		
			ImGui::End();
		}


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());//SLowwwww

		planet.Draw();

		
		window.SwapBuffers();
	}

	return 0;
}
