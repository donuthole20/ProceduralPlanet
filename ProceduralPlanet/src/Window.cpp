#include "Window.h"

#include "Log.h"



#include "externallibs/imgui/imgui.h"
#include "externallibs/imgui/imgui_impl_glfw.h"
#include "externallibs/imgui/imgui_impl_opengl3.h"


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

	glfwSetKeyCallback(window, KeyInput_Callback);
	glfwSetCursorPosCallback(window,Cursor_position_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

}

Input* Window::GetInputManger()
{
	return &inputManager;
}

void Window::KeyInput_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Window* callbackWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	callbackWindow->GetInputManger()->ProcessKeyInput(key, action);
		
}

void Window::Cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Window* callbackWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (callbackWindow->mouseFirstMoved)
	{
		callbackWindow->lastX = (float)xpos;
		callbackWindow->lastY = (float)ypos;
		callbackWindow->mouseFirstMoved = false;
	}

	callbackWindow->xChange = (float)xpos - callbackWindow->lastX;
	callbackWindow->yChange = callbackWindow->lastY - (float)ypos;

	callbackWindow->lastX = (float)xpos;
	callbackWindow->lastY = (float)ypos;

	callbackWindow->GetInputManger()->ProcessCursorPosition(callbackWindow->xChange, callbackWindow->yChange);
}

void Window::ClearColor()
{
	/* Render here */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Window::SwapBuffers()
{
	/* Swap front and back buffers */
	glfwSwapBuffers(window);

	/* Poll for and process events */
	glfwPollEvents();
}

bool Window::IsClosing()
{
	return glfwWindowShouldClose(window);
}
void Window::CloseWindow()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

Window::~Window()
{
	CloseWindow();
}