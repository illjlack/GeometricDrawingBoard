#include "Shapes.h"
#include <QPainterPath>

// =======================================================================================================Shape
Shape::ShapeType Shape::getType()
{
	return type;
}

// =======================================================================================================Point
Point::Point(const QPointF& position, QColor color, Shape shape)
    : position(position), color(color), shape(shape)
{
}

Point::~Point() {
}

QPointF Point::getPosition() const
{
    return position;
}

void Point::setPosition(const QPointF& newPos)
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

void Point::draw(QPainter& painter)
{
    // ���û�����ɫ���ޱ߿�
    painter.setBrush(QBrush(color));
    painter.setPen(Qt::NoPen);

    // ����һ�� QPainterPath
    QPainterPath path;

    if (shape == Shape::Square) {
        path.addRect(position.x() - 5, position.y() - 5, 10, 10);  // ����һ�� 10x10 �ľ���
    }
    else if (shape == Shape::Circle) {
        path.addEllipse(position, 5.0, 5.0);  // ���ư뾶Ϊ 5 ��Բ
    }

    painter.drawPath(path);
}
