#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Input.h"
#include "Shader.h"

class Camera:public InputListener
{
private:
	Camera();
	float fieldOfView, nearPlane, farPlane;
	glm::mat4 projectionMatrix;
	glm::vec3 position, front, up, right, worldUp;
	float yaw, pitch;
	float movementSpeed, turnSpeed;
	unsigned char keyMap = 0;
	std::vector<Shader*> shaderList;

	void CalculateDirectionVectors();
public:
	Camera(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);
	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetViewMatrix();
	void HandleKeyInput(int key, int action);
	void HandleMousePositionInput(float xChange, float yChange);
	void HandleKeyInputUpdate(float deltaTime);
	void SetAspectRatio(float aspectRatio);
	glm::vec3 GetPosition();
	void AddShader(Shader* shader);
};

