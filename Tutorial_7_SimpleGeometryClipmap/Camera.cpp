#include "Camera.h"

void Camera::Init(glm::vec3 pos, glm::vec3 center, glm::vec3 up) {
	viewMat = glm::lookAt(pos, center, up);
	this->pos = pos;
	moveSpeed = 5.f;
	rotateSpeed = 1.0f;

	projectionMat = glm::perspective(45.f, 4.f / 3.f, 10.f, 800000.f);
	projectViewMat = projectionMat * viewMat;
}

glm::vec3 Camera::GetPos() const {
	return pos;
}

glm::vec3 Camera::GetDir() const {
	return dir;
}

glm::vec3 Camera::GetUp() const {
	return up;
}

void Camera::Move(GLFWwindow* window, float deltaTime) {
	static glm::mat4 invOldRot(1.f);

	glm::mat4 rot(1.f);
	rot = glm::rotate(rot, eulerAngle.y, glm::vec3(0.f, 1.f, 0.f));
	rot = glm::rotate(rot, eulerAngle.x, glm::vec3(1.f, 0.f, 0.f));

	vec3 d = normalize(vec3(rot * vec4(0, 0, -1, 1)));
	vec4 u = rot * vec4(0, 1, 0, 1);

	pos += d * moveSpeed * deltaTime * (float)(glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_S)) * 500.f;
	dir = d;
	up = normalize(vec3(u));

	viewMat = lookAt(pos, pos + dir, up);
	projectViewMat = projectionMat * viewMat;
}

void Camera::Rotate(float xpos, float ypos) {
	vec2 delta = vec2(xpos, ypos) * rotateSpeed;
	eulerAngle += vec3(-delta.y, -delta.x, 0.f);
	eulerAngle.x = glm::clamp(eulerAngle.x, -89.f, 89.f);
}





