#include <windows.h>
#include <iostream>
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <future>
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
#include "externallibs/imgui/imgui_color_gradient.h"


int main(void)
{
	std::cout << "size_t:" << sizeof(size_t) << " unsigned int:" << sizeof(unsigned int) << "\n";
#ifdef RELEASE
	ShowWindow( GetConsoleWindow(), SW_HIDE );
#endif 

	Window window = Window(1366, 768);
	
	unsigned int resolution = 50;

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
	planet.CreatePlanet(resolution, noiseSettings);
		

	Camera camera = Camera(45.0f, window.GetAspectRatio(), 0.01f, 100.0f);
	Input* inputManager = window.GetInputManger();//TODO inconsistent input
	window.SetCurrentCamera(&camera);
#if ENABLE_INPUT == 1
	inputManager->registerMousePositionInputCallback(&camera);
	inputManager->registerKeyInputCallback(&camera);
#endif 

	glm::mat4 model(1.0f);
	model = glm::translate(model,glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));

	Shader shader = Shader("res/shaders/Planet.shader");
	camera.AddShader(&shader);
	planet.AddShader(&shader);

	shader.UseShader();
	shader.SetVec3(SHADER_UNIFORM::ALBEDO, glm::vec3(1.0f));
	shader.SetFloat(SHADER_UNIFORM::AO, 1.0f);
	shader.SetFloat(SHADER_UNIFORM::METALLIC, 0.8f);
	shader.SetFloat(SHADER_UNIFORM::ROUGHNESS, 0.8f);
	shader.SetVec3(SHADER_UNIFORM::LIGHT_POSITIONS, glm::vec3(-2.0f, 4.0f, 0.0f));
	shader.SetVec3(SHADER_UNIFORM::LIGHT_COLORS, glm::vec3(30.0f));


	float lastTime = 0.0f;

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	int counter = 0;
	bool isContinousUpdate = true;
	bool isEdited = false;

	PlanetTexture planetTexture;
	//temp
	planetTexture.biomes.reserve(3);
	planetTexture.biomes.emplace_back();
	planetTexture.biomes.emplace_back();
	planetTexture.biomes.emplace_back();

	
	 struct GradientMarks
	 {
		 ImGradientMark* draggingMark = nullptr;
		 ImGradientMark* selectedMark = nullptr;
	 };
	 std::vector<GradientMarks> biomesMarks(planetTexture.biomes.size(), GradientMarks());
	
	 planet.SetTexture(planetTexture);
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
		
		model = glm::rotate(model, glm::radians(0.2f), glm::vec3(0.0f, 1.0f, 0.0f));
		shader.SetMat4x4(SHADER_UNIFORM::MODEL, model);
		shader.SetFloat(SHADER_UNIFORM::TIME, window.GetCurrentTime());

		{
			ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
			ImGui::Begin("Settings", NULL,window_flags);
			if (isContinousUpdate)
			{
				counter++;
			}
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Triangle count: %d", planet.GetTriCount());//TODO: store this
			
			ImGui::NewLine();
			ImGui::Checkbox("Continuos Update", &isContinousUpdate);
			ImGui::NewLine();

			bool isTextureEdited = false;
			for (unsigned int i = 0; i < planetTexture.biomes.size(); i++)
			{
				std::string header = "Biome(" + std::to_string(i) + "):";
				if ((ImGui::CollapsingHeader(header.c_str())))
				{
					isTextureEdited |= ImGui::GradientEditor(&planetTexture.biomes[i], biomesMarks[i].draggingMark, biomesMarks[i].selectedMark);
				}
				else
				{
					biomesMarks[i].draggingMark = nullptr;
					biomesMarks[i].selectedMark = nullptr;
				}
			}
			if (isTextureEdited)
			{
				planet.SetTexture(planetTexture);
				isTextureEdited = false;
			}
			

			/*
			static float debugShaderFloat = 0.0f;
			ImGui::SliderFloat("Debug Float",&debugShaderFloat, 0.0f, 1.0f);
			shader.SetFloat(SHADER_UNIFORM::DEBUG_FLOAT, debugShaderFloat);
			*/
			ImGui::NewLine();
			if ( ImGui::Button("Generate")||(isContinousUpdate && counter >=60 && isEdited && !planet.IsBusy()))
			{
				planet.CreatePlanet(resolution, noiseSettings);
				planet.SetTexture(planetTexture);//Note: to resize texture
				isEdited = false;
				counter = 0;
			}
			isEdited |= ImGui::SliderInt("Resolution", (int*)(&resolution), 2, 1000);
			for (unsigned int i = 0; i < noiseSettings.size(); i++)
			{
				std::string index = std::to_string(i);
				std::string noiseType;
				switch(noiseSettings[i]->type)
				{
				case NoiseType::Simple:
					noiseType = "Simple";
					break;
				case NoiseType::Rigid:
					noiseType = "Rigid";
					break;
				}
				
				std::string header = noiseType + " Noise-" + index;
				if (ImGui::CollapsingHeader(header.c_str()))
				{
					isEdited |=ImGui::SliderInt(std::string("Number of Iteration###1" + index).c_str(), (int*)&noiseSettings[i]->numberOfLayers, 1, 10);
					isEdited |=ImGui::SliderFloat(std::string("Base Roughness###2" + index).c_str(), &noiseSettings[i]->baseRoughness, 0.0f, 5.0f);
					isEdited |=ImGui::SliderFloat(std::string("Strength###3" + index).c_str(), &noiseSettings[i]->strength, 0.0f, 5.0f);
					isEdited |=ImGui::SliderFloat(std::string("Roughness###4" + index).c_str(), &noiseSettings[i]->roughness, 0.0f, 5.0f);
					isEdited |=ImGui::SliderFloat(std::string("Persistence###5" + index).c_str(), &noiseSettings[i]->persistence, 0.0f, 5.0f);
					isEdited |=ImGui::SliderFloat(std::string("Min Value###6" + index).c_str(), &noiseSettings[i]->minValue, 0.0f, 5.0f);
					isEdited |=ImGui::SliderFloat3(std::string("Center###7" + index).c_str(), &noiseSettings[i]->center.x, 0.0f, 10.0f);
					if (noiseSettings[i]->type == NoiseType::Rigid)
					{
						isEdited |= ImGui::SliderFloat(std::string("Weight Multiplier###8" + index).c_str(), &((RigidNoiseSettings*)noiseSettings[i])->weightMultiplier, 0.0f, 5.0f);
					}
				}
			}
				
			ImGui::End();
		}


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());//SLowwwww

		planet.Draw();

		
		window.SwapBuffers();
	}

	return 0;
}
