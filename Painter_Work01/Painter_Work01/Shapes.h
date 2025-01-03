#pragma once
#include <QPainter>
#include <QPointF>
#include <QColor>
#include <QVector>

class Shape {
public:
    enum class ShapeType {
        Undefined = 0,         // δ����

        Point,                 // ��
        
        Polyline,              // ����
        Spline,                // ������
        ArcThreePoints,        // ����Բ��
        ArcTwoPoints,          // ����Բ��
        Streamline             // ����
    };

    virtual ~Shape() = default;
    virtual void draw(QPainter& painter) = 0; // ���Ʒ���

    Shape::ShapeType getType();

private:
    ShapeType type = ShapeType::Undefined;
    
};


class Point : public Shape {
public:
    enum class Shape {
        Square,
        Circle
    };

    Point(const QPointF& position, QColor color = Qt::white, Shape shape = Shape::Square);
    ~Point();

    QPointF getPosition() const;
    void setPosition(const QPointF& newPos);

    QColor getColor() const;
    void setColor(const QColor& newColor);

    Shape getShape() const;
    void setShape(Shape newShape);

    void draw(QPainter& painter) override;

private:
    QPointF position;  // ���λ��
    QColor color;      // �����ɫ
    Shape shape;       // �����״
};


class BaseLine : public Shape {
public:
    enum class Style {
        Solid,     // ʵ��
        Dashed     // ����
    };

    BaseLine(const QVector<QPointF>& points,
        QColor color = Qt::black,
        Style style = Style::Solid,
        float width = 1.0f,
        float dashPattern = 1.0f);
    virtual ~BaseLine();

    // ���õ㼯
    void setPoints(const QVector<QPointF>& points);
    QVector<QPointF> getPoints() const;

    // ������ɫ
    void setColor(QColor color);
    QColor getColor() const;

    // ������ʽ
    void setStyle(Style style);
    Style getStyle() const;

    // �����߿�
    void setLineWidth(float width);
    float getLineWidth() const;

    // �������߶γ�
    void setDashPattern(float pattern);
    float getDashPattern() const;

protected:
    QVector<QPointF> points;  // �㼯
    QColor color;             // ��ɫ
    Style style;              // ��ʽ
    float lineWidth;          // �߿�
    float dashPattern;        // ���߶γ�
};


class Polyline : public BaseLine {
public:
    Polyline(const QVector<QPointF>& points,
        QColor color = Qt::black,
        Style style = Style::Solid,
        float width = 1.0f)
        : BaseLine(points, color, style, width) {}

protected:
    void draw(QPainter& painter) override;
};
