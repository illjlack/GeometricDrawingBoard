#include "Camera.h"
#include <QMatrix4x4>
#include <QtMath>


// 构造函数：初始化相机参数
Camera::Camera(QVector3D position, QVector3D up, float yaw, float pitch)
    : Front(QVector3D(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), isMousePress(false)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// 获取视图矩阵
QMatrix4x4 Camera::GetViewMatrix()
{
    QMatrix4x4 view;
    // 相机位置，看的中心，上方
    view.lookAt(Position, Position + Front, Up);
    return view;
}

void Camera::setProjectionMode(ProjectionMode mode)
{
    projectionMode = mode;
}

QMatrix4x4 Camera::GetProjectionMatrix(float width, float height)
{
    QMatrix4x4 projection;
    float aspectRatio = width / height;

    if (projectionMode == ProjectionMode::Perspective) {
        projection.perspective(Zoom, aspectRatio, 0.1f, 100.0f);  // 透视投影
    }
    else if (projectionMode == ProjectionMode::Orthographic) {
        float orthoSize = Zoom*0.02f;  // 正交投影的大小
        projection.ortho(-orthoSize * aspectRatio, orthoSize * aspectRatio, -orthoSize, orthoSize, 0.1f, 100.0f);  // 正交投影
    }

    return projection;
}

// reset 函数：重置相机参数为默认值
void Camera::reset()
{
    Position = QVector3D(0.0f, 0.0f, 3.0f); // 默认位置
    WorldUp = QVector3D(0.0f, 1.0f, 0.0f);  // 默认向上方向
    Yaw = YAW;                              // 默认 Yaw 角
    Pitch = PITCH;                          // 默认 Pitch 角
    MovementSpeed = SPEED;                  // 默认移动速度
    MouseSensitivity = SENSITIVITY;         // 默认鼠标灵敏度
    Zoom = ZOOM;                            // 默认缩放
    updateCameraVectors();                  // 更新方向向量
}


void Camera::processInput(float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;

    if (keys[Qt::Key_D]) Position += Right * velocity;
    if (keys[Qt::Key_A]) Position -= Right * velocity;
    if (keys[Qt::Key_W] && !keys[Qt::Key_Shift]) Position += Up * velocity;
    if (keys[Qt::Key_S] && !keys[Qt::Key_Shift]) Position -= Up * velocity;
    if (keys[Qt::Key_W] && keys[Qt::Key_Shift]) Position += Front * velocity;
    if (keys[Qt::Key_S] && keys[Qt::Key_Shift]) Position -= Front * velocity;


    velocity *= 10;
    if (keys[Qt::Key_Up]) Pitch += velocity;
    if (keys[Qt::Key_Down]) Pitch -= velocity;
    if (keys[Qt::Key_Left]) Yaw -= velocity;
    if (keys[Qt::Key_Right]) Yaw += velocity;
    
    
    if (keys[Qt::Key_R]) reset();
    updateCameraVectors();
}

// 处理鼠标移动输入
void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw -= xoffset;
    Pitch -= yoffset;

    if (constrainPitch) {
        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;
    }

    updateCameraVectors();
}

// 处理鼠标滚轮输入
void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= static_cast<float>(yoffset);
    if (Zoom < 1.0f) Zoom = 1.0f;
    if (Zoom > 89.0f) Zoom = 89.0f;
}

// 更新相机方向向量
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

// 处理键盘按下事件
void Camera::keyPressEvent(QKeyEvent* event)
{
    // 标记按键被按下
    if (event->key() == Qt::Key_Shift) {
        keys[Qt::Key_Shift] = true;
    }
    if (event->key() == Qt::Key_W) {
        keys[Qt::Key_W] = true;
    }
    if (event->key() == Qt::Key_S) {
        keys[Qt::Key_S] = true;
    }
    if (event->key() == Qt::Key_A) {
        keys[Qt::Key_A] = true;
    }
    if (event->key() == Qt::Key_D) {
        keys[Qt::Key_D] = true;
    }

    if (event->key() == Qt::Key_Up) {
        keys[Qt::Key_Up] = true;  // 向上看
    }
    if (event->key() == Qt::Key_Down) {
        keys[Qt::Key_Down] = true;  // 向下看
    }
    if (event->key() == Qt::Key_Left) {
        keys[Qt::Key_Left] = true;  // 向左看
    }
    if (event->key() == Qt::Key_Right) {
        keys[Qt::Key_Right] = true;  // 向右看
    }

    if (event->key() == Qt::Key_R) {
        keys[Qt::Key_R] = true;  // Reset
    }
}

// 处理键盘松开事件
void Camera::keyReleaseEvent(QKeyEvent* event)
{
    // 标记按键被松开
    if (event->key() == Qt::Key_Shift) {
        keys[Qt::Key_Shift] = false;
    }
    if (event->key() == Qt::Key_W) {
        keys[Qt::Key_W] = false;
    }
    if (event->key() == Qt::Key_S) {
        keys[Qt::Key_S] = false;
    }
    if (event->key() == Qt::Key_A) {
        keys[Qt::Key_A] = false;
    }
    if (event->key() == Qt::Key_D) {
        keys[Qt::Key_D] = false;
    }

    if (event->key() == Qt::Key_Up) {
        keys[Qt::Key_Up] = false;  // 向上看
    }
    if (event->key() == Qt::Key_Down) {
        keys[Qt::Key_Down] = false;  // 向下看
    }
    if (event->key() == Qt::Key_Left) {
        keys[Qt::Key_Left] = false;  // 向左看
    }
    if (event->key() == Qt::Key_Right) {
        keys[Qt::Key_Right] = false;  // 向右看
    }

    if (event->key() == Qt::Key_R) {
        keys[Qt::Key_R] = false;  // Reset
    }
}

// 处理鼠标事件,按住转动视角
void Camera::mouseMoveEvent(QMouseEvent* event)
{
    if (isMousePress)
    {
        float xoffset = event->x() - lastMousePos.x();
        float yoffset = lastMousePos.y() - event->y();  // 反向y轴
        ProcessMouseMovement(xoffset, yoffset);
        lastMousePos = event->pos(); // 更新上一次鼠标位置
    }
}

void Camera::mousePressEvent(QMouseEvent* event)
{
    isMousePress = true;
    lastMousePos = event->pos();
}

void Camera::mouseReleaseEvent(QMouseEvent* event)
{
    isMousePress = false;
}

void Camera::mouseScrollEvent(QWheelEvent* event)
{
    // 滚动的单位是1/8度，转换为常用的增量单位
    float yoffset = event->angleDelta().y() / 120.0f;
    ProcessMouseScroll(yoffset);
}

