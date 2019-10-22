#pragma once
#include <iostream>
#include <GL/glew.h>
#include <stdio.h>
#include "ConfigMacros.h"

#define ASSERT(x) if(!(x)) __debugbreak();

#if GL_ENABLE_ERROR_CHECK == 1
#define GLCall(x)\
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
		std::cout << "[OpenGL Error] (" << error << ")\n";
		return false;
	}
	return true;
}
#endif