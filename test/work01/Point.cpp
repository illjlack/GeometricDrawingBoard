#include "Point.h"
#include <OpenGLWidget.h>

void Point::initializeOpenGL() 
{
    // 获取 OpenGL 函数
    QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

    // 创建 VAO 和 VBO
    vao.create();  // 创建 VAO
    vbo.create();  // 创建 VBO

    // 设置 VAO
    vao.bind();  // 绑定 VAO

    // 设置 VBO
    vbo.bind();  // 绑定 VBO
    if (shape == Shape::Square) {
        GLfloat squareVertices[] = {
            -0.05f,  0.05f, 0.0f,  // 左上
             0.05f,  0.05f, 0.0f,  // 右上
             0.05f, -0.05f, 0.0f,  // 右下
            -0.05f, -0.05f, 0.0f   // 左下
        };
        vbo.allocate(squareVertices, sizeof(squareVertices));
    }
    else if (shape == Shape::Circle) {
        const int segments = 100;
        GLfloat circleVertices[segments * 3];
        GLfloat angleStep = 2.0f * 3.1415926f / segments;

        for (int i = 0; i < segments; ++i) {
            GLfloat angle = i * angleStep;
            circleVertices[i * 3] = 0.05f * cos(angle) + position.x;
            circleVertices[i * 3 + 1] = 0.05f * sin(angle) + position.y;
            circleVertices[i * 3 + 2] = position.z;
        }
        vbo.allocate(circleVertices, sizeof(circleVertices));
    }

    // 设置顶点属性指针
    functions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    functions->glEnableVertexAttribArray(0);

    // 解绑 VBO 和 VAO
    vbo.release();
    vao.release();
}


void Point::draw() {
    QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

    // 设置颜色
    QColor color = getColor();

    // 绑定 VAO 并绘制
    vao.bind();
    if (shape == Shape::Square) {
        functions->glDrawArrays(GL_TRIANGLE_FAN, 0, 4);  // 绘制方形
    }
    else if (shape == Shape::Circle) {
        functions->glDrawArrays(GL_TRIANGLE_FAN, 0, 100);  // 绘制圆形
    }
    vao.release();
}
