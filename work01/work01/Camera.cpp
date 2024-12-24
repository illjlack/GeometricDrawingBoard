#include "Camera.h"

Camera::Camera(glm::vec3 startPosition, glm::vec3 startfront) : position(startPosition)
{

}

// 获取相机的视图矩阵
glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(position, position + front, up);
}

// 处理相机运动
void Camera::processMovement(Movement direction, float deltaTime)
{
	float velocity = cameraSpeed * deltaTime;
	if (direction == Movement::FORWARD)
		position += front * velocity;
	if (direction == Movement::BACKWARD)
		position -= front * velocity;
	if (direction == Movement::LEFT)
		position -= right * velocity;
	if (direction == Movement::RIGHT)
		position += right * velocity;
}

// 处理相机旋转
void Camera::processRotation(Rotate direction, float deltaTime)
{
	float velocity = rotationSpeed * deltaTime;
	if (direction == Rotate::UP)
		pitch += velocity;
	if (direction == Rotate::DOWN)
		pitch -= velocity;
	if (direction == Rotate::LEFT)
		yaw -= velocity;
	if (direction == Rotate::RIGHT)
		yaw += velocity;

	// 更新相机的方向向量
	updateCameraVectors();
}

// 更新相机的方向向量
void Camera::updateCameraVectors() {
	glm::vec3 newFront;
	newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newFront.y = sin(glm::radians(pitch));
	newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(newFront);//向量归一化

	// 计算右向量
	right = glm::normalize(glm::cross(front, up));
	// 计算上向量
	up = glm::normalize(glm::cross(right, front));
}
