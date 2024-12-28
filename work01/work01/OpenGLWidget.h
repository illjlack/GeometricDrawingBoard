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
    // ��дOpenGL�������ں���
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
 
    // �¼�������
    void keyPressEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    QOpenGLShaderProgram* shader;               // ��ɫ������Ʒ����һ����ɫ����

    std::vector<Object*> vec;                    // ��Ⱦ�������
    
    // ��ͼ
    glm::mat4 projection;                        // ͶӰ����
    Camera camera;                               // �������

    // ����ʱ��
    QElapsedTimer frameTimer;   
    float deltaTime;                             // ֡��ʱ��
    float lastFrame;                             // ��һ֡ʱ��
};
