#include "Camera.h"
#include <QMatrix4x4>
#include <QtMath>


// ���캯������ʼ���������
Camera::Camera(QVector3D position, QVector3D up, float yaw, float pitch)
    : Front(QVector3D(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), isMousePress(false)
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

void Camera::setProjectionMode(ProjectionMode mode)
{
    projectionMode = mode;
}

QMatrix4x4 Camera::GetProjectionMatrix(float width, float height)
{
    QMatrix4x4 projection;
    float aspectRatio = width / height;

    if (projectionMode == ProjectionMode::Perspective) {
        projection.perspective(Zoom, aspectRatio, 0.1f, 100.0f);  // ͸��ͶӰ
    }
    else if (projectionMode == ProjectionMode::Orthographic) {
        float orthoSize = Zoom*0.02f;  // ����ͶӰ�Ĵ�С
        projection.ortho(-orthoSize * aspectRatio, orthoSize * aspectRatio, -orthoSize, orthoSize, 0.1f, 100.0f);  // ����ͶӰ
    }

    return projection;
}

// reset �����������������ΪĬ��ֵ
void Camera::reset()
{
    Position = QVector3D(0.0f, 0.0f, 3.0f); // Ĭ��λ��
    WorldUp = QVector3D(0.0f, 1.0f, 0.0f);  // Ĭ�����Ϸ���
    Yaw = YAW;                              // Ĭ�� Yaw ��
    Pitch = PITCH;                          // Ĭ�� Pitch ��
    MovementSpeed = SPEED;                  // Ĭ���ƶ��ٶ�
    MouseSensitivity = SENSITIVITY;         // Ĭ�����������
    Zoom = ZOOM;                            // Ĭ������
    updateCameraVectors();                  // ���·�������
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

// ��������ƶ�����
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

// ��������������
void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= static_cast<float>(yoffset);
    if (Zoom < 1.0f) Zoom = 1.0f;
    if (Zoom > 89.0f) Zoom = 89.0f;
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

// ������̰����¼�
void Camera::keyPressEvent(QKeyEvent* event)
{
    // ��ǰ���������
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
        keys[Qt::Key_Up] = true;  // ���Ͽ�
    }
    if (event->key() == Qt::Key_Down) {
        keys[Qt::Key_Down] = true;  // ���¿�
    }
    if (event->key() == Qt::Key_Left) {
        keys[Qt::Key_Left] = true;  // ����
    }
    if (event->key() == Qt::Key_Right) {
        keys[Qt::Key_Right] = true;  // ���ҿ�
    }

    if (event->key() == Qt::Key_R) {
        keys[Qt::Key_R] = true;  // Reset
    }
}

// ��������ɿ��¼�
void Camera::keyReleaseEvent(QKeyEvent* event)
{
    // ��ǰ������ɿ�
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
        keys[Qt::Key_Up] = false;  // ���Ͽ�
    }
    if (event->key() == Qt::Key_Down) {
        keys[Qt::Key_Down] = false;  // ���¿�
    }
    if (event->key() == Qt::Key_Left) {
        keys[Qt::Key_Left] = false;  // ����
    }
    if (event->key() == Qt::Key_Right) {
        keys[Qt::Key_Right] = false;  // ���ҿ�
    }

    if (event->key() == Qt::Key_R) {
        keys[Qt::Key_R] = false;  // Reset
    }
}

// ��������¼�,��סת���ӽ�
void Camera::mouseMoveEvent(QMouseEvent* event)
{
    if (isMousePress)
    {
        float xoffset = event->x() - lastMousePos.x();
        float yoffset = lastMousePos.y() - event->y();  // ����y��
        ProcessMouseMovement(xoffset, yoffset);
        lastMousePos = event->pos(); // ������һ�����λ��
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
    // �����ĵ�λ��1/8�ȣ�ת��Ϊ���õ�������λ
    float yoffset = event->angleDelta().y() / 120.0f;
    ProcessMouseScroll(yoffset);
}

