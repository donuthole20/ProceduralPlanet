#include "Camera.h"
#include "InputCode.h"
#include <iostream>

#define W_KEY_BIT_LOCATION  (1 << 0)
#define S_KEY_BIT_LOCATION  (1 << 1)
#define A_KEY_BIT_LOCATION  (1 << 2)
#define D_KEY_BIT_LOCATION  (1 << 3)

Camera::Camera(float fieldOfView, float aspectRatio, float nearPlane, float farPlane):
	fieldOfView(fieldOfView),
	nearPlane(nearPlane),
	farPlane(farPlane),
	projectionMatrix(glm::perspective(fieldOfView, aspectRatio, nearPlane, farPlane)),
	position(glm::vec3(0.0f,0.0f,2.0f)),
	front(glm::vec3(0.0f, 0.0f, -1.0f)),
	up(glm::vec3(0.0f)),
	right(glm::vec3(0.0f)),
	worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
	yaw(270.0f),
	pitch(0.0f),
	movementSpeed(1),
	turnSpeed(0.05f),
	keyMap(0)
{
	CalculateDirectionVectors();
}
void Camera::CalculateDirectionVectors()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

glm::mat4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

glm::mat4 Camera::GetViewMatrix()
{
	
	return glm::lookAt(position,position+front,worldUp);
}

void Camera::HandleKeyInput(int key,int action)
{
	unsigned char bitLocation =0;

	switch (key)
	{
		case KEY_W:
			bitLocation = W_KEY_BIT_LOCATION;
			break;
		case KEY_S:
			bitLocation = S_KEY_BIT_LOCATION;
			break;
		case KEY_A:
			bitLocation = A_KEY_BIT_LOCATION;
			break;
		case KEY_D:
			bitLocation = D_KEY_BIT_LOCATION;
			break;
	}

	if (bitLocation)
	{
		if (action == PRESS)
		{
			keyMap |= bitLocation;
		}
		else if(action == RELEASE)
		{
			keyMap &= ~bitLocation;
		}
	}

}

void Camera::HandleMousePositionInput(float xChange, float yChange)
{
	yaw += xChange * turnSpeed;
	pitch += yChange * turnSpeed;

	pitch = glm::min(pitch, 89.0f);
	pitch = glm::max(pitch, -89.0f);

	std::cout << pitch <<"\n";

	CalculateDirectionVectors();
}

void Camera::HandleKeyInputUpdate(float deltaTime)
{
	float velocity = movementSpeed * deltaTime;
	if (keyMap & W_KEY_BIT_LOCATION)
	{
		position += front * velocity;
	}
	if (keyMap & S_KEY_BIT_LOCATION)
	{
		position -= front * velocity;
	}
	if (keyMap & A_KEY_BIT_LOCATION)
	{
		position -= right * velocity;
	}
	if (keyMap & D_KEY_BIT_LOCATION)
	{
		position += right * velocity;
	}

	for (size_t i = 0; i < shaderList.size(); i++)
	{
		shaderList[i]->Bind();
		shaderList[i]->SetMat4x4(SHADER_UNIFORM::VIEW, GetViewMatrix());
		shaderList[i]->SetVec3(SHADER_UNIFORM::CAMERA_POSITION, position);
		shaderList[i]->Unbind();
	}
}

void Camera::SetAspectRatio(float aspectRatio)
{
	projectionMatrix = glm::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);
	for (size_t i = 0; i < shaderList.size(); i++)
	{
		shaderList[i]->Bind();
		shaderList[i]->SetMat4x4(SHADER_UNIFORM::PROJECTION, projectionMatrix);
		shaderList[i]->Unbind();
	}
}

glm::vec3 Camera::GetPosition()
{
	return position;
}

void Camera::AddShader(Shader* shader)
{
	shader->Bind();
	shader->SetMat4x4(SHADER_UNIFORM::PROJECTION, projectionMatrix);
	shader->SetMat4x4(SHADER_UNIFORM::VIEW, GetViewMatrix());
	shader->SetVec3(SHADER_UNIFORM::CAMERA_POSITION, position);
	shader->Unbind();

	shaderList.push_back(shader);
}
