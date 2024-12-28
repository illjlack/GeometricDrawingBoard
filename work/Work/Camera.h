#pragma once

#include <QVector3D>
#include <QMatrix4x4>
#include <QKeyEvent>
#include <QMouseEvent>
#include <unordered_map>


// 默认相机参数
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 1.0f;
const float SENSITIVITY = 0.01f;
const float ZOOM = 45.0f;

// 相机类，处理输入并计算视图矩阵
class Camera
{
public:
    // 构造函数
    Camera(QVector3D position = QVector3D(0.0f, 0.0f, 1.0f),
        QVector3D up = QVector3D(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH);

    // 获取视图矩阵，将世界坐标系转换到相机坐标系
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


    // 更新相机方向向量
    void updateCameraVectors();
    void reset();

    // 相机属性
    QVector3D Position, Front, Up, Right, WorldUp;
    float Yaw, Pitch;
    float MovementSpeed, MouseSensitivity, Zoom;

    // 鼠标控制
    bool isMousePress = false;
    QPoint lastMousePos;

    std::unordered_map<int, bool> keys;
};