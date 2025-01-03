#pragma once
#include <QPainter>
#include <QPointF>
#include <QColor>
#include <QVector>
#include <QKeyEvent>


class Geo {
public:
    enum class GeoType {
        Undefined = 0,         // 未定义

        Point,                 // 点
        
        Polyline,              // 折线
        Spline,                // 样条线
        ArcThreePoints,        // 三点圆弧
        ArcTwoPoints,          // 两点圆弧
        Streamline             // 流线
    };

    enum class  GeoDrawState
    {
        Complete,   // 空闲状态
        Drawing     // 正在绘制
    };

    virtual ~Geo() = default;
    virtual void draw(QPainter& painter) = 0; // 绘制方法

    GeoType getType();
    GeoDrawState getGeoDrawState();
    
    // 接受鼠标和键盘事件来进行绘制
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

protected:
    void setGeoType(Geo::GeoType newType);
    void setGeoDrawState(GeoDrawState newState);

private:
    GeoType geoType = GeoType::Undefined;
    GeoDrawState geoDrawState = GeoDrawState::Drawing;
};


class Point : public Geo {
public:
    enum class Shape {
        Square,
        Circle
    };

    Point();
    Point(const QPointF& position, QColor color = Qt::white, Shape shape = Shape::Square);
    ~Point();

    QPointF getPosition() const;
    void setPosition(const QPointF& newPos);

    QColor getColor() const;
    void setColor(const QColor& newColor);

    Shape getShape() const;
    void setShape(Shape newShape);

    void draw(QPainter& painter) override;

    void mousePressEvent(QMouseEvent* event) override;
private:
    QPointF position;  // 点的位置
    QColor color;      // 点的颜色
    Shape shape = Shape::Square;       // 点的形状
};


class BaseLine : public Geo {
public:
    enum class Style {
        Solid,     // 实线
        Dashed     // 虚线
    };
    BaseLine();
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
    QVector<QPointF> points;    // 点集
    QColor color = Qt::red;     // 颜色
    Style style = Style::Solid; // 样式
    float lineWidth = 1.0f;       // 线宽
    float dashPattern = 1.0f;     // 虚线段长
};


class Polyline : public BaseLine {
public:
    Polyline();
    Polyline(const QVector<QPointF>& points,
        QColor color = Qt::black,
        Style style = Style::Solid,
        float width = 1.0f);

    void mousePressEvent(QMouseEvent* event) override;

protected:
    void draw(QPainter& painter) override;
};

class Spline : public BaseLine {
public:
    Spline(const QVector<QPointF>& points,
        QColor color = Qt::black,
        Style style = Style::Solid,
        float width = 1.0f);
    Spline();
    void mousePressEvent(QMouseEvent* event) override;

protected:
    QVector<QPointF> curvePoints;
    void draw(QPainter& painter) override;
};
