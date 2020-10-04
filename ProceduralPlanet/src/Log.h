#pragma once
#include <glm.hpp>
#include <string>
class Log
{

public:
	static void LogDebug(const char* log);
	static void LogDebug(float log);
	static void LogError(std::string error, bool willTriggerBreakPoint = true);
	static void LogVec3(glm::vec3 vec);
private:
	Log();
};

