#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Input.h"

class Camera:public InputListener
{
private:
	Camera();
	glm::mat4 projectionMatrix;
	glm::vec3 position, front, up, right, worldUp;
	float yaw, pitch;
	float movementSpeed, turnSpeed;
	unsigned char keyMap = 0;

	void CalculateDirectionVectors();
public:
	Camera(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);
	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetViewMatrix();
	void HandleKeyInput(int key, int action);
	void HandleMousePositionInput(float xChange, float yChange);
	void HandleKeyInputUpdate(float deltaTime);
	glm::vec3 GetPosition() { return position; };
};

