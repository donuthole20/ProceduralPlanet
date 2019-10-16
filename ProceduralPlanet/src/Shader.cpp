#include "Shader.h"
#include "OpenGLErrorHandler.h"
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <gtc/type_ptr.hpp>

#include "Log.h"

Shader::Shader(const char* filepath)
{
	//Parsing
	std::ifstream stream(filepath);
	if (stream.fail())
	{
		std::string error = "Cannot open shader file: " + std::string(filepath);
		Log::LogError(error.c_str());
		return;
	}

	enum class ShaderTypeParseMode {NONE = -1,VERTEX = 0,FRAGMENT = 1 };//Note: this matches with the index of array ss
	std::string line;
	std::stringstream shaderSourceStrings[2];//TODO better if heap allocated?
	ShaderTypeParseMode parseMode = ShaderTypeParseMode::NONE;
	while (std::getline(stream,line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				parseMode = ShaderTypeParseMode::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				parseMode = ShaderTypeParseMode::FRAGMENT;
			}
		}
		else
		{
			shaderSourceStrings[(int)parseMode] << line << "\n";
		}
	}
	stream.close();

	shaderProgramID = glCreateProgram();
	unsigned int vertexShaderID = CompileShader(GL_VERTEX_SHADER, shaderSourceStrings[0].str().c_str());
	unsigned int fragmentShaderID = CompileShader(GL_FRAGMENT_SHADER, shaderSourceStrings[1].str().c_str());
	 
	GLCall(glAttachShader(shaderProgramID, vertexShaderID));
	GLCall(glAttachShader(shaderProgramID, fragmentShaderID));
	GLCall(glLinkProgram(shaderProgramID));
	GLCall(glValidateProgram(shaderProgramID));

	//Note: get and save the uniformIDs here
	projectionUniformID = glGetUniformLocation(shaderProgramID, "projectionMatrix");
	viewUniformID = glGetUniformLocation(shaderProgramID, "viewMatrix");
	modelUniformID = glGetUniformLocation(shaderProgramID, "modelMatrix");

	GLCall(glDeleteShader(vertexShaderID));
	GLCall(glDeleteShader(fragmentShaderID));
}

Shader::~Shader()
{
	DeleteShader();
}

unsigned int Shader::CompileShader(unsigned int type, const char* source)
{
	unsigned int id = glCreateShader(type);
	GLCall(glShaderSource(id, 1, &source, nullptr));
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

	if (result == GL_FALSE)
	{
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = new char[length];
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		const char* shaderType;
		switch (type)
		{
		case GL_VERTEX_SHADER:
			shaderType = "vertex";
			break;
		case GL_FRAGMENT_SHADER:
			shaderType = "fragment";
			break;
		default:
			shaderType = "undefined";
			break;
		}
		std::string error = "Failed to compile " + std::string(shaderType) + " shader!";
		error = error + "\n" + message;
		Log::LogError(error.c_str());
		GLCall(glDeleteShader(id));
		delete[] message;
		return 0;
	}

	return id;
}

void Shader::UseShader()
{
	GLCall(glUseProgram(shaderProgramID));
}

void Shader::DeleteShader()
{
	GLCall(glDeleteProgram(shaderProgramID));
}

void Shader::SetProjectionMatrix(glm::mat4 projection)
{
	if (projectionUniformID == -1)
	{
		Log::LogError("Failed to get projection matrix uniform ID.");
	}
	else
	{
		glUniformMatrix4fv(projectionUniformID, 1, GL_FALSE, glm::value_ptr( projection));
	}

}
void Shader::SetViewMatrix(glm::mat4 view)
{
	if (viewUniformID == -1)
	{
		Log::LogError("Failed to get view matrix uniform ID.");
	}
	else
	{
		glUniformMatrix4fv(viewUniformID, 1, GL_FALSE, glm::value_ptr(view));
	}

}
void Shader::SetModelMatrix(glm::mat4 model)
{
	if (modelUniformID == -1)
	{
		Log::LogError("Failed to get model matrix uniform ID.");
	}
	else
	{
		glUniformMatrix4fv(modelUniformID, 1, GL_FALSE, glm::value_ptr(model));
	}

}

