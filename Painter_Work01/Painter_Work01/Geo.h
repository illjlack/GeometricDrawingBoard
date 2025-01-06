#pragma once
#include <QPainter>
#include <QPointF>
#include <QColor>
#include <QVector>
#include <QKeyEvent>

#include "DrawSettings.h"

// ================================================ Geo
class Geo {
public:
    virtual ~Geo() = default;
    virtual void draw(QPainter& painter) = 0; // 绘制方法

    // 接受鼠标和键盘事件来进行绘制
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    GeoType getType();
    GeoDrawState getGeoDrawState();
    bool getIsInvalid();

protected:
    void setGeoType(GeoType newType);
    void setGeoDrawState(GeoDrawState newState);
    void setIsInvalid(bool flag);

private:
    GeoType geoType = GeoType::Undefined;
    GeoDrawState geoDrawState = GeoDrawState::Drawing;

    bool isInvalid = false;
    bool isSelected = false;
};

// ================================================ Point
class Point : public Geo {
public:
    Point();
    Point(const QPointF& position, QColor color, PointShape shape);
    ~Point();

    QPointF getPosition() const;
    void setPosition(const QPointF& newPos);

    QColor getColor() const;
    void setColor(const QColor& newColor);

    PointShape getShape() const;
    void setShape(PointShape newShape);

    void draw(QPainter& painter) override;

    void mousePressEvent(QMouseEvent* event) override;
private:
    QPointF position;                                                   // 点的位置
    QColor color = getSetting<QRgb>(Key_PointColor);                    // 点的颜色
    PointShape shape = getSetting<PointShape>(Key_PointShape);          // 点的形状
};

// ================================================ BaseLine
class BaseLine : public Geo {
public:
    BaseLine();
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
    // 设置 Polygon 类为友元类(图的边是线)
    friend class Polygon;
    QVector<QPointF> controlPoint;                                      // 控制点

    float lineWidth = getSetting<float>(Key_LineWidth);                 // 线宽
    float dashPattern = getSetting<float>(Key_LineDashPattern);         // 虚线段长
    QColor color = getSetting<QRgb>(Key_LineColor);                     // 颜色
    LineStyle lineStyle = getSetting<LineStyle>(Key_LineStyle);         // 样式
};

// ================================================ Polyline
class Polyline : public BaseLine {
public:
    Polyline();
    ~Polyline();
    void mousePressEvent(QMouseEvent* event) override;

protected:
    void draw(QPainter& painter) override;
};

// ================================================ Spline
class Spline : public BaseLine {
public:
    Spline();
    ~Spline();
    void mousePressEvent(QMouseEvent* event) override;

protected:
    QVector<QPointF> curvePoints;
    void draw(QPainter& painter) override;
};

// ================================================ Polygon
class Polygon : public Geo {
public:
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
    QColor fillColor = getSetting<QRgb>(Key_PgFillColor);                  // 面内填充颜色
    QColor lineColor = getSetting<QRgb>(Key_PgLineColor);                  // 边框颜色
    float lineWidth = getSetting<float>(Key_PgLineWidth);                  // 边框宽度
    LineStyle lineStyle = getSetting<LineStyle>(Key_PgLineStyle);          // 边框线形
};