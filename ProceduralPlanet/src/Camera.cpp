#include "Camera.h"
#include "InputCode.h"

#define W_KEY_BIT_LOCATION  (1 << 0)
#define S_KEY_BIT_LOCATION  (1 << 1)
#define A_KEY_BIT_LOCATION  (1 << 2)
#define D_KEY_BIT_LOCATION  (1 << 3)

Camera::Camera(float fieldOfView, float aspectRatio, float nearPlane, float farPlane):
	projectionMatrix(glm::perspective(fieldOfView, aspectRatio, nearPlane, farPlane)),
	position(glm::vec3(0.0f)),
	front(glm::vec3(0.0f, 0.0f, -1.0f)),
	up(glm::vec3(0.0f)),
	right(glm::vec3(0.0f)),
	worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
	yaw(0.0f),
	pitch(0.0f),
	movementSpeed(1),
	turnSpeed(0.5f),
	keyMap(0)
{
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

void Camera::handleMousePositionInput(float xChange, float yChange)
{
	yaw += xChange * turnSpeed;
	pitch += yChange * turnSpeed;

	pitch = glm::min(pitch, 89.0f);
	pitch = glm::max(pitch, -89.0f);

	calculateDirectionVectors();
}

void Camera::handleKeyInputUpdate(float deltaTime)
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
}
