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
#include <QElapsedTimer>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget* parent = nullptr);
    ~OpenGLWidget() override;

    void pushObject(Object* obj);
protected:
    // 重写OpenGL生命周期函数
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
 
    // 事件处理函数
    void keyPressEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    QOpenGLShaderProgram* shader;               // 着色器（物品共用一个着色器）

    std::vector<Object*> vec;                    // 渲染对象队列
    
    // 视图
    glm::mat4 projection;                        // 投影矩阵
    Camera camera;                               // 控制相机

    // 控制时间
    QElapsedTimer frameTimer;   
    float deltaTime;                             // 帧间时间
    float lastFrame;                             // 上一帧时间
};
