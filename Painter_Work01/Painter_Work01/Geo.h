#pragma once
#include <QPainter>
#include <QPointF>
#include <QColor>
#include <QVector>
#include <QKeyEvent>

#include "DrawSettings.h"

class Geo {
public:
    virtual ~Geo() = default;
    virtual void draw(QPainter& painter) = 0; // 绘制方法

    GeoType getType();
    GeoDrawState getGeoDrawState();
    bool getIsInvalid();
    
    // 接受鼠标和键盘事件来进行绘制
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

protected:
    void setGeoType(GeoType newType);
    void setGeoDrawState(GeoDrawState newState);
    void setIsInvalid(bool flag);

private:
    GeoType geoType = GeoType::Undefined;
    GeoDrawState geoDrawState = GeoDrawState::Drawing;

    bool isInvalid = false; // 无效
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
    BaseLine();
    BaseLine(const QVector<QPointF>& points,
        QColor color = Qt::black,
        LineStyle LineStyle = LineStyle::Solid,
        float width = 1.0f,
        float dashPattern = 1.0f);
    virtual ~BaseLine();

    // 设置点集
    void setPoints(const QVector<QPointF>& points);
    QVector<QPointF> getPoints() const;

    // 设置颜色
    void setColor(QColor color);
    QColor getColor() const;

    // 设置线形
    void setLineStyle(LineStyle lineStyle);
    LineStyle getLineStyle() const;

    // 设置线型
    void setLineWidth(float width);
    float getLineWidth() const;

    // 设置虚线段长
    void setDashPattern(float pattern);
    float getDashPattern() const;

    void pushPoint(QPointF& point);


protected:
    // 设置 Polygon 类为友元类
    friend class Polygon;
    QVector<QPointF> points;    // 点集
    QColor color = Qt::red;     // 颜色
    LineStyle lineStyle = LineStyle::Solid; // 样式
    float lineWidth = 1.0f;       // 线宽
    float dashPattern = 1.0f;     // 虚线段长
};


class Polyline : public BaseLine {
public:
    Polyline();
    Polyline(const QVector<QPointF>& points,
        QColor color = Qt::black,
        LineStyle LineStyle = LineStyle::Solid,
        float width = 1.0f);

    void mousePressEvent(QMouseEvent* event) override;

protected:
    void draw(QPainter& painter) override;
};

class Spline : public BaseLine {
public:
    Spline(const QVector<QPointF>& points,
        QColor color = Qt::black,
        LineStyle LineStyle = LineStyle::Solid,
        float width = 1.0f);
    Spline();
    void mousePressEvent(QMouseEvent* event) override;

protected:
    QVector<QPointF> curvePoints;
    void draw(QPainter& painter) override;
};



// ================================================ Polygon
class Polygon : public Geo {
public:
    Polygon(const QVector<QPointF>& points,
        const QColor& fillColor = Qt::white,
        const QColor& borderColor = Qt::black,
        LineStyle borderStyle = LineStyle::Solid,
        float borderWidth = 1.0f);
    Polygon();
    ~Polygon();

    void setFillColor(const QColor& color);
    QColor getFillColor() const;

    void setBorderColor(const QColor& color);
    QColor getBorderColor() const;

    void setBorderStyle(LineStyle LineStyle);
    LineStyle getBorderStyle() const;

    void setBorderWidth(float width);
    float getBorderWidth() const;

    void draw(QPainter& painter) override;

    void mousePressEvent(QMouseEvent* event) override;

private:
    BaseLine* edges = nullptr;
    QColor fillColor;               // 面内填充颜色
    QColor borderColor;             // 边框颜色
    float borderWidth;              // 边框宽度
    LineStyle borderStyle;    // 边框线形
};