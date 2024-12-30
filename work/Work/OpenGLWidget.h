#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QElapsedTimer>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include "Camera.h"
#include "Object.h"

class OpenGLWidget :
    public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    OpenGLWidget(QWidget* parent = nullptr);
    ~OpenGLWidget() override;

    void pushObject(Object* obj);

protected:
    // 重写OpenGL生命周期函数
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void test();

    // 事件处理函数
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;


private:
    // 着色器程序
    QOpenGLShaderProgram* shaderProgram;

    Camera camera;

    // 控制时间
    QElapsedTimer frameTimer;
    float deltaTime;                             // 帧间时间
    float lastFrame;                             // 上一帧时间

    std::vector<Object*> vec;

    void updateCameraMovement();
    void processInput();
};
