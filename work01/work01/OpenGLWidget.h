#pragma once

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Object.h"
#include <QOpenGLShaderProgram>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget* parent = Q_NULLPTR);

    void pushObject(Object* obj);
protected:

    void paintEvent(QPaintEvent* event) override;

    // 重写OpenGL生命周期函数
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    // 事件处理函数
    void keyPressEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
private:
    Camera camera;

    float deltaTime;
    float lastFrame;

    std::vector<Object*> vec;

    QOpenGLShaderProgram *shader;
};
