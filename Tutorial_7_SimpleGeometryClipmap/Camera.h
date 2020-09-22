#pragma once
#include "Predefine.h"

class Camera {
private:
	glm::vec3 pos, dir, up;
	float rotateSpeed;

public:
	glm::vec3 eulerAngle{ 0, 0, 0 };
	glm::mat4 viewMat;
	glm::mat4 projectionMat;
	glm::mat4 projectViewMat;
	float moveSpeed;

	void Init(glm::vec3 pos, glm::vec3 center, glm::vec3 up);
	glm::vec3 GetPos() const;
	glm::vec3 GetDir() const;
	glm::vec3 GetUp() const;
	void Move(GLFWwindow* window, float deltaTime);
	void Rotate(float xOff, float yOff);
};