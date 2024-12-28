#pragma once

#include <QVector3D>
#include <QMatrix4x4>
#include <QKeyEvent>
#include <QMouseEvent>
#include <unordered_map>


// Ĭ���������
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 1.0f;
const float SENSITIVITY = 0.01f;
const float ZOOM = 45.0f;

// ����࣬�������벢������ͼ����
class Camera
{
public:
    // ���캯��
    Camera(QVector3D position = QVector3D(0.0f, 0.0f, 1.0f),
        QVector3D up = QVector3D(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH);

    // ��ȡ��ͼ���󣬽���������ϵת�����������ϵ
    QMatrix4x4 GetViewMatrix();

    void processInput(float deltaTime);

    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

    

private:
    
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = false);
    void ProcessMouseScroll(float yoffset);


    // ���������������
    void updateCameraVectors();
    void reset();

    // �������
    QVector3D Position, Front, Up, Right, WorldUp;
    float Yaw, Pitch;
    float MovementSpeed, MouseSensitivity, Zoom;

    // ������
    bool isMousePress = false;
    QPoint lastMousePos;

    std::unordered_map<int, bool> keys;
};