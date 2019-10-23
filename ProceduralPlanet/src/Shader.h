#pragma once
#include <glm.hpp>
#include <map>


enum class SHADER_UNIFORM
{
	PROJECTION,
	VIEW,
	MODEL,
	ALBEDO,
	METALLIC,
	ROUGHNESS,
	AO,
	LIGHT_POSITIONS,
	LIGHT_COLORS,
	CAMERA_POSITION,
	ELEVATION_MIN_MAX_POSITION,
	DEBUG_FLOAT
};

class Shader
{
private:
	Shader();
	unsigned int shaderProgramID;
	unsigned int CompileShader(unsigned int type, const char* source);
	std::map<SHADER_UNIFORM, unsigned int> uniformIDs;

	void AddUniformID(SHADER_UNIFORM uniform, const char* uniformName);
public:
	~Shader();
	Shader(const char* filepath);
	void UseShader();
	void DeleteShader();
	void SetMat4x4(SHADER_UNIFORM uniform, glm::mat4 matrix);
	void SetVec2(SHADER_UNIFORM uniform, glm::vec2 vector);
	void SetVec3(SHADER_UNIFORM uniform, glm::vec3 vector);
	void SetFloat(SHADER_UNIFORM uniform, float value);

};

