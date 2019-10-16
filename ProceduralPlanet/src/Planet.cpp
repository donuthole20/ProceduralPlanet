#include "Planet.h"
#include "OpenGLErrorHandler.h"
#include <vector>

Planet::~Planet()
{
	Delete();
}

Planet::Planet(size_t resolution)
{

	resolution = glm::min(glm::abs((int)resolution), 250);
	std::vector<glm::vec3> directions;
	directions.reserve(6);
	directions.emplace_back(0.0f, 1.0f, 0.0f);
	directions.emplace_back(0.0f, -1.0f, 0.0f);
	directions.emplace_back(-1.0f, 0.0f, 0.0f);
	directions.emplace_back(1.0f, 0.0f, 0.0f);
	directions.emplace_back(0.0f, 0.0f, 1.0f);
	directions.emplace_back(0.0f, 0.0f, -1.0f);

	std::vector<glm::vec3> positions(resolution * resolution * 6, glm::vec3(0, 0, 0));
	std::vector<unsigned int> indices(((resolution - 1) * (resolution - 1) * 6) * 6, 0);

	size_t triIndex = 0;
	for (int t = 0; t < directions.size(); t++)
	{
		resolution = glm::min(glm::abs((int)resolution), 250);
		glm::vec3 axisA = glm::vec3(directions[t].y, directions[t].z, directions[t].x);
		glm::vec3 axisB = glm::cross(directions[t], axisA);

		for (size_t y = 0; y < resolution; y++)
		{
			for (size_t x = 0; x < resolution; x++)
			{
				size_t i = x + y * resolution;
				i += (resolution * resolution * t);
				glm::vec2 percent = glm::vec2(x, y);
				percent /= (resolution - 1);
				glm::vec3 pointOnUnitCube = directions[t] + (percent.x - 0.5f) * 2 * axisA + (percent.y - 0.5f) * 2 * axisB;
				glm::vec3 pointOnUnitSphere = glm::normalize(pointOnUnitCube);
				positions[i] = pointOnUnitSphere;
				//std::cout << pointOnUnitSphere.x << "," << pointOnUnitSphere.y << "," << pointOnUnitSphere.z << std::endl;
				if (x != resolution - 1 && y != resolution - 1)
				{
					indices[triIndex] = i;
					indices[triIndex + 1] = i + resolution + 1;
					indices[triIndex + 2] = i + resolution;

					indices[triIndex + 3] = i;
					indices[triIndex + 4] = i + 1;
					indices[triIndex + 5] = i + resolution + 1;
					triIndex += 6;
				}
			}
		}

	}


	GLCall(glGenVertexArrays(1, &vertexArrayObjectID));
	GLCall(glBindVertexArray(vertexArrayObjectID));

	GLCall(glGenBuffers(1, &vertexBufferID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size() * 3, &positions[0], GL_STATIC_DRAW));//size of array * 3 for 3 floats in a vertex
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0));
	GLCall(glEnableVertexAttribArray(0));

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));
	positions.clear();

	triCount = indices.size();
	GLCall(glGenBuffers(1, &indexBufferID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW));


	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	indices.clear();
}

void Planet::Draw()
{
	GLCall(glBindVertexArray(vertexArrayObjectID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID));
	GLCall(glDrawElements(GL_TRIANGLES, triCount, GL_UNSIGNED_INT, nullptr));
}

void Planet::Delete()
{
	GLCall(glDeleteVertexArrays(1, &vertexArrayObjectID));
	GLCall(glDeleteBuffers(1, &vertexBufferID));
	GLCall(glDeleteBuffers(1, &indexBufferID));
}
