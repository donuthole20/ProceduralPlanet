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
	BIOMES_COUNT,
	TIME,
	DEBUG_FLOAT
};

struct PreCompileShaderVariable
{
	uint32_t lightCount;
	PreCompileShaderVariable(uint32_t vlightCount)
		:lightCount(vlightCount)
	{}
};

class Shader
{
private:
	Shader();
	uint32_t shaderProgramID;
	uint32_t CompileShader(uint32_t type, const char* source);
	std::map<SHADER_UNIFORM, uint32_t> uniformIDs;

	void AddUniformID(SHADER_UNIFORM uniform, const char* uniformName);
public:
	~Shader();
	Shader(const char* filepath, PreCompileShaderVariable lightSettings);
	void Bind();
	void Unbind();
	void DeleteShader();
	void SetMat4x4(SHADER_UNIFORM uniform, glm::mat4 matrix);
	void SetVec2(SHADER_UNIFORM uniform, glm::vec2 vector);
	void SetVec3(SHADER_UNIFORM uniform, glm::vec3 vector);
	void SetFloat(SHADER_UNIFORM uniform, float value);
	void SetInt(SHADER_UNIFORM uniform, int value);

};

