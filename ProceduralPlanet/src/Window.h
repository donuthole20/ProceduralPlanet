#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Input.h"
class Window
{

private:
	bool keys[1024];
	float aspectRatio;
	GLFWwindow* window;
	float lastX, lastY, xChange, yChange;
	bool mouseFirstMoved;
	Input inputManager;
	Camera* currentCamera;

	static void KeyInput_Callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void Cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void Framebuffer_size_callback(GLFWwindow* window, int width, int height);
public:
	Window();
	Window(int height, int width);
	float GetCurrentTime() { return (float)glfwGetTime(); };
	Input* GetInputManger();
	float GetAspectRatio() { return aspectRatio; };
	void UpdateAspectRatio();
	void ClearColor();
	void SwapBuffers();
	bool IsClosing();
	void CloseWindow();
	void SetCurrentCamera(Camera* camera) { currentCamera = camera; };
	~Window();
};

