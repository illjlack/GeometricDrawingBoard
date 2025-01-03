#pragma once
#include <QPainter>
#include <QPointF>
#include <QColor>
#include <QVector>

class Shape {
public:
    enum class ShapeType {
        Undefined = 0,         // 未定义

        Point,                 // 点
        
        Polyline,              // 折线
        Spline,                // 样条线
        ArcThreePoints,        // 三点圆弧
        ArcTwoPoints,          // 两点圆弧
        Streamline             // 流线
    };

    virtual ~Shape() = default;
    virtual void draw(QPainter& painter) = 0; // 绘制方法

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
    QPointF position;  // 点的位置
    QColor color;      // 点的颜色
    Shape shape;       // 点的形状
};


class BaseLine : public Shape {
public:
    enum class Style {
        Solid,     // 实线
        Dashed     // 虚线
    };

    BaseLine(const QVector<QPointF>& points,
        QColor color = Qt::black,
        Style style = Style::Solid,
        float width = 1.0f,
        float dashPattern = 1.0f);
    virtual ~BaseLine();

    // 设置点集
    void setPoints(const QVector<QPointF>& points);
    QVector<QPointF> getPoints() const;

    // 设置颜色
    void setColor(QColor color);
    QColor getColor() const;

    // 设置样式
    void setStyle(Style style);
    Style getStyle() const;

    // 设置线宽
    void setLineWidth(float width);
    float getLineWidth() const;

    // 设置虚线段长
    void setDashPattern(float pattern);
    float getDashPattern() const;

protected:
    QVector<QPointF> points;  // 点集
    QColor color;             // 颜色
    Style style;              // 样式
    float lineWidth;          // 线宽
    float dashPattern;        // 虚线段长
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
