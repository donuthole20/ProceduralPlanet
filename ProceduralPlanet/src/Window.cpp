#include "Window.h"

#include "Log.h"

//Input Window::inputManager = Input();

Window::Window()
	:Window(720, 720)
{
	
}

Window::Window(int height, int width)
{

	/* Initialize the library */
	if (!glfwInit())
	{
		Log::LogError("GLFW Failed to initialize.");
	}

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(height, width, "Procedural Planet", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		Log::LogError("GLFW Window Failed to initialize.");
	}

	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
	aspectRatio = (float)bufferWidth / (float)bufferHeight;


	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		Log::LogError("GLEW Failed to initialize.");
	}

	glEnable(GL_DEPTH_TEST);

	glfwSetWindowUserPointer(window, this);

	inputManager = Input();

	glfwSetKeyCallback(window, keyInput_Callback);
	glfwSetCursorPosCallback(window,cursor_position_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Input* Window::getInputManger()
{
	return &inputManager;
}

void Window::keyInput_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Window* callbackWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	callbackWindow->getInputManger()->processKeyInput(key, action);
		
}

void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Window* callbackWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (callbackWindow->mouseFirstMoved)
	{
		callbackWindow->lastX = xpos;
		callbackWindow->lastY = ypos;
		callbackWindow->mouseFirstMoved = false;
	}

	callbackWindow->xChange = xpos - callbackWindow->lastX;
	callbackWindow->yChange = callbackWindow->lastY - ypos;

	callbackWindow->lastX = xpos;
	callbackWindow->lastY = ypos;

	callbackWindow->getInputManger()->processCursorPosition(callbackWindow->xChange, callbackWindow->yChange);
}

void Window::clearColor()
{
	/* Render here */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Window::swapBuffers()
{
	/* Swap front and back buffers */
	glfwSwapBuffers(window);

	/* Poll for and process events */
	glfwPollEvents();
}

bool Window::isClosing()
{
	return glfwWindowShouldClose(window);
}
void Window::closeWindow()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

Window::~Window()
{
	closeWindow();
}