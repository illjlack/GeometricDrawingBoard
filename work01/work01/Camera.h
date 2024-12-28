#pragma once

#include <QVector3D>
#include <QMatrix4x4>

// 相机移动方向枚举
enum class Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// 默认相机参数
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// 相机类，处理输入并计算视图矩阵
class Camera
{
public:
    // 构造函数
    Camera(QVector3D position = QVector3D(0.0f, 0.0f, 5.0f),
        QVector3D up = QVector3D(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH);

    // 获取视图矩阵，将世界坐标系转换到相机坐标系
    QMatrix4x4 GetViewMatrix();

    // 处理键盘输入
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // 处理鼠标移动输入
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // 处理鼠标滚轮输入
    void ProcessMouseScroll(float yoffset);

private:
    // 更新相机方向向量
    void updateCameraVectors();

    // 相机属性
    QVector3D Position, Front, Up, Right, WorldUp;
    float Yaw, Pitch;
    float MovementSpeed, MouseSensitivity, Zoom;
};