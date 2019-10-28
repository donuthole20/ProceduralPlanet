#pragma once
#include <vector>
#include <glm.hpp>
class Debug
{
public:
	static void DrawTextureWithoutPadding(std::vector<glm::lowp_u8vec3> textureData, float height, float width);
};

