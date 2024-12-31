#pragma once

#include <QColor>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include "Object.h"

namespace My

{

    class Point : public Object
    {
    public:
        enum class Shape {
            Square,
            Circle
        };

        Point(const QVector3D& position, QColor color = Qt::white, Shape shape = Shape::Square);
        ~Point();

        QVector3D getPosition() const;
        void setPosition(const QVector3D& newPos);

        QColor getColor() const;
        void setColor(const QColor& newColor);

        Shape getShape() const;
        void setShape(Shape newShape);

        void initialize() override;
        void draw() override;
        void drawBufferZone();
    private:
        QOpenGLBuffer vbo;
        QOpenGLVertexArrayObject vao;

        QOpenGLBuffer vboBufferZone;
        QOpenGLVertexArrayObject vaoBufferZone;

        QVector3D position;  // ���λ��
        QColor color;        // �����ɫ
        Shape shape;         // �����״

        QVector<QVector3D> createBufferZone(float d, int segments = 100) const;
    };

}