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
	AddUniformID(SHADER_UNIFORM::PROJECTION, "u_projectionMatrix");
	AddUniformID(SHADER_UNIFORM::VIEW, "u_viewMatrix");
	AddUniformID(SHADER_UNIFORM::MODEL, "u_modelMatrix");
	AddUniformID(SHADER_UNIFORM::ALBEDO, "u_albedo");
	AddUniformID(SHADER_UNIFORM::METALLIC, "u_metallic");
	AddUniformID(SHADER_UNIFORM::ROUGHNESS, "u_roughness");
	AddUniformID(SHADER_UNIFORM::AO, "u_ao");
	AddUniformID(SHADER_UNIFORM::LIGHT_POSITIONS, "u_lightPositions[0]");
	AddUniformID(SHADER_UNIFORM::LIGHT_COLORS, "u_lightColors[0]");
	AddUniformID(SHADER_UNIFORM::CAMERA_POSITION, "u_camPos");

	GLCall(glDeleteShader(vertexShaderID));
	GLCall(glDeleteShader(fragmentShaderID));
}

void Shader::AddUniformID(SHADER_UNIFORM uniform, const char* uniformName)
{
	unsigned int uniformID = glGetUniformLocation(shaderProgramID, uniformName);
	if (uniformID == -1)
	{
		//Log::LogError("Unable to find uniform " + std::string(uniformName));
	}
	uniformIDs.insert(std::pair<SHADER_UNIFORM, unsigned int>(uniform, uniformID));
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

void Shader::SetMat4x4(SHADER_UNIFORM uniform, glm::mat4 matrix)
{
	glUniformMatrix4fv(uniformIDs[uniform], 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetVec3(SHADER_UNIFORM uniform, glm::vec3 vector)
{
	glUniform3fv(uniformIDs[uniform], 1, &vector[0]);
}

void Shader::SetFloat(SHADER_UNIFORM uniform, float value)
{
	glUniform1f(uniformIDs[uniform], value);
}


Shader::~Shader()
{
	DeleteShader();
}
