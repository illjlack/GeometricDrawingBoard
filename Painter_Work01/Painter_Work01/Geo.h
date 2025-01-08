#pragma once
#include <QPainter>
#include <QPointF>
#include <QColor>
#include <QVector>
#include <QKeyEvent>

#include "DrawSettings.h"
#include "GeoMathUtil.h"

class Geo;
Geo* createGeo(DrawMode mode);

// ================================================================================================ Geo
class Geo {
public:
    virtual ~Geo() = default;
    virtual void initialize() = 0;                          // 初始化，在第一次点击（确定第一个控制点）的时候自动调用，来设置属性
    virtual void draw(QPainter& painter) = 0;           // 绘制方法
    virtual void drawControlPoints(QPainter& painter);  // 绘制控制点
    //virtual void drawBuffer(QPainter& painter) = 0;     // 绘制缓冲区
    virtual void hitTesting(QPointF);                   // 点击测试
    virtual void completeDrawing(); // 完成构造（完成前接受事件来进行绘制）

    GeoType getGeoType(); // 反射类型


    // 接受鼠标和键盘事件来进行绘制或修改
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    // 状态的一些修改
    bool isStateDrawing();
    bool isStateComplete();
    bool isStateInvalid();
    bool isStateSelected();
    bool isStateInitialized();

    void setStateInitialized();
    void setStateInvalid();
    void setStateComplete();
    void setStateSelected();
    void setStateNotSelected();
protected:
    void setGeoType(GeoType newType);   // 构造函数中,确定类型
    QVector<QPointF> controlPoints;    // 控制点 (每个类可以自己加划分信息)
    QPointF tempControlPoints;          // 临时控制点, 在绘制中使用
private:
    int geoState = 0;
    GeoType geoType = GeoType::Undefined;
};

// ================================================================================================ Point
class Point : public Geo {
public:
    Point();
    void initialize() override;

    // 图形构造事件
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void draw(QPainter& painter) override;

private:
    QColor color;                    // 点的颜色
    PointShape shape;                // 点的形状
};

// ================================================================================================ SimpleLine
class SimpleLine : public Geo {
public:
    SimpleLine();
    void initialize() override;

    // 图形构造事件
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void draw(QPainter& painter) override;
protected:

    QVector<QPointF> points;        // 点集
    NodeLineStyle nodeLineStyle;    // 节点线型
    
    float lineWidth;                    // 线宽
    float dashPattern;                  // 虚线段长
    QColor color;                       // 颜色
    LineStyle lineStyle;                // 样式（实线、虚线）
};

// ================================================================================================ SimpleArea
class SimpleArea : public Geo {
public:
    SimpleArea();
    void initialize() override;

    // 图形构造事件
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void draw(QPainter& painter) override;
protected:

    QVector<QPointF> points;    // 点集
    NodeLineStyle nodeLineStyle;    // 节点线型

    float lineWidth;               // 边框宽度
    QColor fillColor;              // 面内填充颜色
    QColor lineColor;              // 边框颜色
    LineStyle lineStyle;           // 边框线形
    float lineDashPattern;         // 虚线段长

};
// ================================================================================================ SimpleArea
class ComplexArea : public Geo {
public:
    ComplexArea();
    void initialize() override;

    // 图形构造事件
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void draw(QPainter& painter) override;
protected:

    bool isDrawing;                       // 在绘制
    QVector<QVector<QPointF>> pointss;    // 二维点集
    QVector<Component> component;

    NodeLineStyle nodeLineStyle;   // 节点线型
    float lineWidth;               // 边框宽度
    QColor fillColor;              // 面内填充颜色
    QColor lineColor;              // 边框颜色
    LineStyle lineStyle;           // 边框线形
    float lineDashPattern;         // 虚线段长
};

// ================================================================================================ DoubleLine
class DoubleLine : public Geo {
public:
    DoubleLine();
    void initialize() override;

    // 图形构造事件
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void draw(QPainter& painter) override;
protected:

    bool isDrawing;                       // 在绘制
    QVector<QVector<QPointF>> pointss;    // 二维点集
    QVector<Component> component;

    NodeLineStyle nodeLineStyle;   // 节点线型
    float lineWidth;               // 边框宽度
    QColor fillColor;              // 面内填充颜色
    QColor lineColor;              // 边框颜色
    LineStyle lineStyle;           // 边框线形
    float lineDashPattern;         // 虚线段长
};
