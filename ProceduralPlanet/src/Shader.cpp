#include "Shader.h"
#include "OpenGLErrorHandler.h"
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <gtc/type_ptr.hpp>
#include <filesystem>

#include "Log.h"
#include "ConfigMacros.h"


Shader::Shader(const char* filename, PreCompileShaderVariable preCompiledVariables)
{
	//Parsing
	std::string filepath = "res/unserialized/shaders/" + std::string(filename);
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
			//Note: Pre compile variables
			if (line.find("#PCV_LightCount") != std::string::npos)
			{
				line.replace(line.find("#PCV_LightCount"), 15, std::to_string(preCompiledVariables.lightCount));
			}

			shaderSourceStrings[(int)parseMode] << line << "\n";
		}
	}
	stream.close();
	uint32_t vertexShaderID = CompileShader(GL_VERTEX_SHADER, shaderSourceStrings[0].str().c_str());
	uint32_t fragmentShaderID = CompileShader(GL_FRAGMENT_SHADER, shaderSourceStrings[1].str().c_str());

	shaderProgramID = glCreateProgram();
	GLCall(glAttachShader(shaderProgramID, vertexShaderID));
	GLCall(glAttachShader(shaderProgramID, fragmentShaderID));
	GLCall(glLinkProgram(shaderProgramID));
	GLCall(glValidateProgram(shaderProgramID));

	//Note: get and save the uniformIDs here
	AddUniformID(SHADER_UNIFORM::PROJECTION, "u_projectionMatrix");//TODO refactor not all shaders should have map to all uniforms
	AddUniformID(SHADER_UNIFORM::VIEW, "u_viewMatrix");
	AddUniformID(SHADER_UNIFORM::MODEL, "u_modelMatrix");
	AddUniformID(SHADER_UNIFORM::ALBEDO, "u_albedo");
	AddUniformID(SHADER_UNIFORM::METALLIC, "u_metallic");
	AddUniformID(SHADER_UNIFORM::ROUGHNESS, "u_roughness");
	AddUniformID(SHADER_UNIFORM::AO, "u_ao");
	AddUniformID(SHADER_UNIFORM::LIGHT_POSITIONS, "u_lightPositions[0]");
	AddUniformID(SHADER_UNIFORM::LIGHT_COLORS, "u_lightColors[0]");
	AddUniformID(SHADER_UNIFORM::CAMERA_POSITION, "u_camPos");
	AddUniformID(SHADER_UNIFORM::ELEVATION_MIN_MAX_POSITION, "u_elevationMinMax");
	AddUniformID(SHADER_UNIFORM::BIOMES_COUNT, "u_biomesCount");
	AddUniformID(SHADER_UNIFORM::TIME, "u_time");
	AddUniformID(SHADER_UNIFORM::DEBUG_FLOAT, "u_debugFloat");
	

	GLCall(glDeleteShader(vertexShaderID));
	GLCall(glDeleteShader(fragmentShaderID));
}

void Shader::AddUniformID(SHADER_UNIFORM uniform, const char* uniformName)
{
	uint32_t uniformID = glGetUniformLocation(shaderProgramID, uniformName);
	if (uniformID == -1)
	{
		Log::LogError("Unable to find uniform " + std::string(uniformName),false);
	}
	uniformIDs.insert(std::pair<SHADER_UNIFORM, uint32_t>(uniform, uniformID));
}

uint32_t Shader::CompileShader(uint32_t type, const char* source)
{
	uint32_t id = glCreateShader(type);
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

void Shader::Bind()
{
	GLCall(glUseProgram(shaderProgramID));
}

void Shader::Unbind()
{
	GLCall(glUseProgram(0));
}

void Shader::DeleteShader()
{
	GLCall(glDeleteProgram(shaderProgramID));
}

void Shader::SetMat4x4(SHADER_UNIFORM uniform, glm::mat4 matrix)
{
	glUniformMatrix4fv(uniformIDs[uniform], 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetVec2(SHADER_UNIFORM uniform, glm::vec2 vector)
{
	glUniform2fv(uniformIDs[uniform], 1, &vector[0]);
}

void Shader::SetVec3(SHADER_UNIFORM uniform, glm::vec3 vector)
{
	glUniform3fv(uniformIDs[uniform], 1, &vector[0]);
}

void Shader::SetFloat(SHADER_UNIFORM uniform, float value)
{
	glUniform1f(uniformIDs[uniform], value);
}

void Shader::SetInt(SHADER_UNIFORM uniform, int value)
{
	glUniform1i(uniformIDs[uniform], value);
}

Shader::~Shader()
{
	DeleteShader();
}
