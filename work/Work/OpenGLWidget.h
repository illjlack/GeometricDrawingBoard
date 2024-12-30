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
    // ��дOpenGL�������ں���
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void test();

    // �¼�������
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;


private:
    // ��ɫ������
    QOpenGLShaderProgram* shaderProgram;

    Camera camera;

    // ����ʱ��
    QElapsedTimer frameTimer;
    float deltaTime;                             // ֡��ʱ��
    float lastFrame;                             // ��һ֡ʱ��

    std::vector<Object*> vec;

    void updateCameraMovement();
    void processInput();
};
