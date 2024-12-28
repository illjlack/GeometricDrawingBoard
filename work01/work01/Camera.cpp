#include "Camera.h"
#include <QMatrix4x4>
#include <QtMath>

// ���캯������ʼ���������
Camera::Camera(QVector3D position, QVector3D up, float yaw, float pitch)
    : Front(QVector3D(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// ��ȡ��ͼ����
QMatrix4x4 Camera::GetViewMatrix()
{
    QMatrix4x4 view;
    // ���λ�ã��������ģ��Ϸ�
    view.lookAt(Position, Position + Front, Up);
    return view;
}

// �����������
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == Camera_Movement::FORWARD) Position += Front * velocity;
    if (direction == Camera_Movement::BACKWARD) Position -= Front * velocity;
    if (direction == Camera_Movement::LEFT) Position -= Right * velocity;
    if (direction == Camera_Movement::RIGHT) Position += Right * velocity;
}

// ��������ƶ�����
void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch) {
        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;
    }

    updateCameraVectors();
}

// ��������������
void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= static_cast<float>(yoffset);
    if (Zoom < 1.0f) Zoom = 1.0f;
    if (Zoom > 100.0f) Zoom = 45.0f;
}

// ���������������
void Camera::updateCameraVectors()
{
    QVector3D front;
    front.setX(qCos(qDegreesToRadians(Yaw)) * qCos(qDegreesToRadians(Pitch)));
    front.setY(qSin(qDegreesToRadians(Pitch)));
    front.setZ(qSin(qDegreesToRadians(Yaw)) * qCos(qDegreesToRadians(Pitch)));
    Front = front.normalized();

    Right = QVector3D::crossProduct(Front, WorldUp).normalized();
    Up = QVector3D::crossProduct(Right, Front).normalized();
}
