#include "Point.h"
#include <OpenGLWidget.h>

Point::Point(const QVector3D& position, QColor color, Shape shape)
    : position(position), color(color), shape(shape)
{
}

Point::~Point(){
    // ������Դ
    vbo.destroy();
    vao.destroy();
}

QVector3D Point::getPosition() const
{
    return position;
}

void Point::setPosition(const QVector3D& newPos)
{
    position = newPos;
}

QColor Point::getColor() const
{
    return color;
}

void Point::setColor(const QColor& newColor)
{
    color = newColor;
}

Point::Shape Point::getShape() const
{
    return shape;
}

void Point::setShape(Shape newShape)
{
    shape = newShape;
}

void Point::initialize()
{
    // ��ȡ OpenGL ����
    QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

    shaderProgram->bind();
    // ���� VAO �� VBO
    vao.create();  // ���� VAO
    vbo.create();  // ���� VBO

    // ���� VAO
    vao.bind();  // �� VAO

    // ���� VBO
    vbo.bind();  // �� VBO
    if (shape == Shape::Square) {
        GLfloat squareVertices[] = {
            -0.01f,  0.01f, 0.0f,  // ����
             0.01f,  0.01f, 0.0f,  // ����
            -0.01f, -0.01f, 0.0f,   // ����

             0.01f,  0.01f, 0.0f,  // ����
             0.01f, -0.01f, 0.0f,  // ����
            -0.01f, -0.01f, 0.0f,   // ����
        };
        vbo.allocate(squareVertices, sizeof(squareVertices));
    }
    else if (shape == Shape::Circle) {
        const int segments = 100;
        GLfloat circleVertices[segments * 3];
        GLfloat angleStep = 2.0f * 3.1415926f / segments;

        for (int i = 0; i < segments; ++i) {
            GLfloat angle = i * angleStep;
            circleVertices[i * 3] = 0.01f * cos(angle) + position.x();
            circleVertices[i * 3 + 1] = 0.01f * sin(angle) + position.y();
            circleVertices[i * 3 + 2] = position.z();
        }
        vbo.allocate(circleVertices, sizeof(circleVertices));
    }

    // ���ö�������ָ��
    functions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    functions->glEnableVertexAttribArray(0);

    // ��� VBO �� VAO
    vbo.release();
    vao.release();

    shaderProgram->release();
}


void Point::draw() 
{
    shaderProgram->bind();
    QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

    // ������ɫ
    QVector3D vec3Color(color.redF(), color.greenF(), color.blueF());
    shaderProgram->setUniformValue("color", vec3Color);

    // ����ģ�;���ƽ�Ƶ� position��
    QMatrix4x4 modelMatrix;
    modelMatrix.translate(position);  // ��ģ��ƽ�Ƶ�ָ��λ��
    // ����ģ�;�����ɫ��
    shaderProgram->setUniformValue("model", modelMatrix);

    // �� VAO ������
    vao.bind();
    vbo.bind();
    if (shape == Shape::Square) {
        functions->glDrawArrays(GL_TRIANGLE_FAN, 0, 6);  // ���Ʒ���
    }
    else if (shape == Shape::Circle) {
        functions->glDrawArrays(GL_TRIANGLE_FAN, 0, 100);  // ����Բ��
    }
    vao.release();
    vbo.release();
    shaderProgram->release();
}


