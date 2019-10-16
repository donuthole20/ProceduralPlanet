#pragma once
#include <iostream>
#include <GL/glew.h>
#include <stdio.h>


#define GL_ENABLE_ERROR_CHECK 0//Note: Enabling this is slow

#define ASSERT(x) if(!(x)) __debugbreak();

#if GL_ENABLE_ERROR_CHECK == 1
#define GLCall(x) GLClearError();\
		x;\
		ASSERT(GLLogCall())
#else
#define GLCall(x) x;
#endif

#if GL_ENABLE_ERROR_CHECK == 1
static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR)
	{
	}
}

static bool  GLLogCall()
{
	while (GLenum error = glGetError())
	{
		printf("[OpenGL Error] (%s)\n", error);
		std::cout << "[OpenGL Error] (" << error << ")\n";
		return false;
	}
	return true;
}
#endif