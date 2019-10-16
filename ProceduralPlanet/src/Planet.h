#pragma once
#include <GL/glew.h>
#include <glm.hpp>

class Planet
{
private:
	Planet();
	unsigned int vertexBufferID;
	unsigned int indexBufferID;
	unsigned int vertexArrayObjectID;
	unsigned int triCount;
public:
	~Planet();
	Planet(size_t resolution);
	void Draw();
	void Delete();
};


