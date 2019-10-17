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

	void calculateDirectionVectors();
public:
	Camera(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);
	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetViewMatrix();
	void handleKeyInput(int key, int action);
	void handleMousePositionInput(float xChange, float yChange);
	void handleKeyInputUpdate(float deltaTime);
};

