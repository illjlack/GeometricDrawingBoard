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

    // �¼�������
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void updateCameraMovement();

private:
    // ��ɫ������
    QOpenGLShaderProgram* shaderProgram;

    Camera camera;

    // ����ʱ��
    QElapsedTimer frameTimer;
    float deltaTime;                             // ֡��ʱ��
    float lastFrame;                             // ��һ֡ʱ��

    QOpenGLVertexArrayObject* vao;
    QOpenGLBuffer* vertexBuffer;

    std::vector<Object*> vec;
};
