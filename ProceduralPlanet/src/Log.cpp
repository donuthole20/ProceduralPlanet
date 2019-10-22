#include "Log.h"

#include <iostream>
#include "ConfigMacros.h"

void Log::LogDebug(const char* log)
{
#if ENABLE_LOG == 1
	std::cout << __FILE__ << ":" << __LINE__ << " Log: " << log << "\n";
#endif
}

void Log::LogError(std::string error , bool willTriggerBreakPoint)
{
#if ENABLE_LOG == 1
	std::cout << __FILE__ << ":" << __LINE__ << " Error: " << error <<"\n";
	if (willTriggerBreakPoint)
	{
		__debugbreak();
	}
#endif
}

void Log::LogVec3(glm::vec3 vec)
{
#if ENABLE_LOG == 1
	std::cout << "x:" << vec.x << " y:" << vec.y << "z:" << vec.z << "\n";
#endif
}
