#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

// �˶�����
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

	// ��ȡ�������ͼ����
	glm::mat4 getViewMatrix();

	// ��������˶�
	void processMovement(Movement direction, float deltaTime = 1.0);
	// ���������ת
	void processRotation(Rotate direction, float deltaTime = 1.0);

private:
	// ���λ�ã�����Է�����ΪҪ��ק�нǶȣ�
	glm::vec3 position;
	glm::vec3 front = { 0.0f, 0.0f, -1.0f };
	glm::vec3 up = { 0.0f, 1.0f, 0.0f };
	glm::vec3 right = { 1.0f, 0.0f, 0.0f };

	float yaw = -90.0f; // ƫ����
	float pitch = 0.0f; // ������

	float cameraSpeed = 0.01f; // ����ƶ��ٶ�
	float rotationSpeed = 0.5f; // ��ת�ٶ�

	// ͨ�� yaw �� pitch ��������ķ�������
	void updateCameraVectors();
};

