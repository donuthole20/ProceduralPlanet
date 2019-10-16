#include "Log.h"

#include <iostream>
#include <stdio.h>


void Log::LogDebug(const char* log)
{
	std::cout << __FILE__ << ":" << __LINE__ << " Log: " << log << "\n";
}

void Log::LogError(const char* error)
{
	printf("%s:%d Error: %s", __FILE__, __LINE__, error);
	std::cout << __FILE__ << ":" << __LINE__ << " Error: " << error <<"\n";
	__debugbreak();
}

void Log::LogVec3(glm::vec3 vec)
{
	std::cout << "x:" << vec.x << " y:" << vec.y << "z:" << vec.z << "\n";
}
