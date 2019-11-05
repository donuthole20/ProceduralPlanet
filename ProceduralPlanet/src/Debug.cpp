#include "Debug.h"
#include "Shader.h"
#include <GL/glew.h>
#include "OpenGLErrorHandler.h"


void Debug::DrawTextureWithoutPadding(std::vector<glm::lowp_u8vec3> textureData,float height, float width)
{
	PreCompileShaderVariable variables = PreCompileShaderVariable(1);
	Shader shader = Shader("res/shaders/Debug.shader", variables);
	shader.Bind();

	float vertices[] = 
	{
		1.0f, 1.0f,	 1.0f,1.0f,
	   -1.0f, 1.0f,	 0.0f,1.0f,
	   -1.0f,-1.0f,  0.0f,0.0f,
	    1.0f,-1.0f,  1.0f,0.0f
	};
	unsigned int indices[] =
	{
		1,2,3,
		1,3,4
	};
	unsigned int vertexArrayObjectID;
	GLCall(glGenVertexArrays(1, &vertexArrayObjectID));
	GLCall(glBindVertexArray(vertexArrayObjectID));

	unsigned int vertexBufferID;
	GLCall(glGenBuffers(1, &vertexBufferID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));//size of array * 3 for 3 floats in a vertex

	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(float) * 2) ));
	GLCall(glEnableVertexAttribArray(1));

	unsigned int indicesBufferID;
	GLCall(glGenBuffers(1, &indicesBufferID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STATIC_DRAW));

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);//Note: this is padding, default is for
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &textureData[0]);
	glGenerateMipmap(GL_TEXTURE_2D);

	GLCall(glBindVertexArray(vertexArrayObjectID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferID));
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	glBindTexture(GL_TEXTURE_2D, 0);

	shader.Unbind();

}
