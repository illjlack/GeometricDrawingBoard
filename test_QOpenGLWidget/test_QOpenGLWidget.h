#pragma once

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <test_cube.h>

class test_QOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit test_QOpenGLWidget(QWidget* parent = nullptr);
    ~test_QOpenGLWidget() override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QOpenGLBuffer vbo; // ���㻺�����
    QOpenGLVertexArrayObject vao; // �����������
    QOpenGLShaderProgram* shaderProgram; // ��ɫ������

    CubeRenderer cube;
};