#pragma once
#include "Object.h"
#include <glm/glm.hpp>
#include <QColor>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

class Point :
    public Object
{
public:
    enum class Shape {
        Square,
        Circle
    };

    Point(glm::vec3 position, QColor color = Qt::black, Shape shape = Shape::Square)
        : position(position), color(color), shape(shape) 
    {
        initialize();
    }

    glm::vec3 getPosition() const { return position; }
    void setPosition(const glm::vec3& newPos) { position = newPos; }

    QColor getColor() const { return color; }
    void setColor(const QColor& newColor) { color = newColor; }

    Shape getShape() const { return shape; }
    void setShape(Shape newShape) { shape = newShape; }

    void setShader(QOpenGLShaderProgram* shader) override
    {
        this->shader = shader;
    }

private:
    QOpenGLBuffer vbo;
    QOpenGLVertexArrayObject vao;

    glm::vec3 position;  // 点的位置
    QColor color;        // 点的颜色
    Shape shape;         // 点的形状
    
    void initialize() override;

    void draw() override;
};

