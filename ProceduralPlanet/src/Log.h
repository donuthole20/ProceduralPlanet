#pragma once
#include <glm.hpp>
class Log
{

public:
	static void LogDebug(const char* log);
	static void LogError(const char* error);
	static void LogVec3(glm::vec3 vec);
private:
	Log();
};

