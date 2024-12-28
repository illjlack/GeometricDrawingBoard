#pragma once

#include <QVector3D>
#include <QMatrix4x4>

// ����ƶ�����ö��
enum class Camera_Movement {
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
    // ���캯��
    Camera(QVector3D position = QVector3D(0.0f, 0.0f, 5.0f),
        QVector3D up = QVector3D(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH);

    // ��ȡ��ͼ���󣬽���������ϵת�����������ϵ
    QMatrix4x4 GetViewMatrix();

    // �����������
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // ��������ƶ�����
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // ��������������
    void ProcessMouseScroll(float yoffset);

private:
    // ���������������
    void updateCameraVectors();

    // �������
    QVector3D Position, Front, Up, Right, WorldUp;
    float Yaw, Pitch;
    float MovementSpeed, MouseSensitivity, Zoom;
};