#pragma once
#include <glm.hpp>

class Shader
{
private:
	Shader();
	unsigned int shaderProgramID;
	unsigned int CompileShader(unsigned int type, const char* source);
	unsigned int projectionUniformID;
	unsigned int viewUniformID;
	unsigned int modelUniformID;
public:
	~Shader();
	Shader(const char* filepath);
	void UseShader();
	void DeleteShader();
	void SetProjectionMatrix(glm::mat4 projection);
	void SetViewMatrix(glm::mat4 view);
	void SetModelMatrix(glm::mat4 model);
};

