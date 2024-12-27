#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ����ƶ�����ö��
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Ĭ���������
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// ����࣬�������벢������ͼ����
class Camera
{
public:
    // �������
    glm::vec3 Position, Front, Up, Right, WorldUp;
    float Yaw, Pitch;
    float MovementSpeed, MouseSensitivity, Zoom;

    // ���캯��
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH);

    // ��ȡ��ͼ����
    glm::mat4 GetViewMatrix();

    // �����������
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // ��������ƶ�����
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // ��������������
    void ProcessMouseScroll(float yoffset);

private:
    // ���������������
    void updateCameraVectors();
};

#endif
