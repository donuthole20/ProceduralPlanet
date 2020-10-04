#include "PyramidTest.h"
#include <GL/glew.h>
#include "OpenGLErrorHandler.h"

PyramidTest::~PyramidTest()
{
	Delete();
}

PyramidTest::PyramidTest()
{
	float vertices[] =
	{
		-1.0f,-1.0f,0.0f,
		0.0f,-1.0f,1.0f,
		1.0f,-1.0f,0.0f,
		0.0f,1.0f,0.0f
	};

	uint32_t indices[] =
	{
		0,3,1,
		1,3,2,
		2,3,0,
		0,1,2
	};

	GLCall(glGenVertexArrays(1, &vertexArrayObjectID));
	GLCall(glBindVertexArray(vertexArrayObjectID));

	GLCall(glGenBuffers(1, &indexBufferID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));


	GLCall(glGenBuffers(1, &vertexBufferID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));//size of array * 3 for 3 floats in a vertex
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0));
	GLCall(glEnableVertexAttribArray(0));

	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void PyramidTest::Draw()
{
	GLCall(glBindVertexArray(vertexArrayObjectID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID));
	GLCall(glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr));

}

void PyramidTest::Delete()
{
	GLCall(glDeleteVertexArrays(1, &vertexArrayObjectID));
	GLCall(glDeleteBuffers(1, &vertexBufferID));
	GLCall(glDeleteBuffers(1, &indexBufferID));
}
