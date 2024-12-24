#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

// 运动方向
enum class Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

enum class Rotate
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

class Camera
{
public:
	Camera(glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 startfront = glm::vec3(0.0f, 0.0f, -1.0f));

	// 获取相机的视图矩阵
	glm::mat4 getViewMatrix();

	// 处理相机运动
	void processMovement(Movement direction, float deltaTime = 1.0);
	// 处理相机旋转
	void processRotation(Rotate direction, float deltaTime = 1.0);

private:
	// 相机位置，和相对方向（因为要拖拽有角度）
	glm::vec3 position;
	glm::vec3 front = { 0.0f, 0.0f, -1.0f };
	glm::vec3 up = { 0.0f, 1.0f, 0.0f };
	glm::vec3 right = { 1.0f, 0.0f, 0.0f };

	float yaw = -90.0f; // 偏航角
	float pitch = 0.0f; // 俯仰角

	float cameraSpeed = 0.01f; // 相机移动速度
	float rotationSpeed = 0.5f; // 旋转速度

	// 通过 yaw 和 pitch 更新相机的方向向量
	void updateCameraVectors();
};

