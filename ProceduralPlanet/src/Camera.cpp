#include "Camera.h"
#include "InputCode.h"
#include <iostream>

Camera::Camera(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
	projectionMatrix = glm::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);
	//projectionMatrix = glm::ortho(0.0f,960.0f,0.0f,540.0f,-1.0f,1.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	position = glm::vec3(0.0f, 0.0f,0.0f);
	yaw = 0.0f;
	pitch = 0.0f;

	front = glm::vec3(0.0f, 0.0f, -1.0f);
	movementSpeed = 1;
	turnSpeed = 0.5f;
	calculateDirectionVectors();
}
void Camera::calculateDirectionVectors()
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

void Camera::handleKeyInput(int key,int action)//TODO make smooth
{
	if (key == KEY_W)
	{
		position += front * movementSpeed;
	}
	if (key == KEY_S)
	{
		position -= front * movementSpeed;
	}
	if (key == KEY_A)
	{
		position -= right * movementSpeed;
	}
	if (key == KEY_D)
	{
		position += right * movementSpeed;
	}
	
}

void Camera::handleMousePositionInput(float xChange, float yChange)
{
	yaw += xChange * turnSpeed;
	pitch += yChange * turnSpeed;

	pitch = glm::min(pitch, 89.0f);
	pitch = glm::max(pitch, -89.0f);

	calculateDirectionVectors();
}
