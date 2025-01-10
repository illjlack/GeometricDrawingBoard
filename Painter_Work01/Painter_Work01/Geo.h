#pragma once
#include <QPainter>
#include <QPointF>
#include <QColor>
#include <QVector>
#include <QKeyEvent>
#include "GeoMathUtil.h"
#include "comm.h"

class Geo;
Geo* createGeo(DrawMode mode);

// ===================================================================== Geo
class Geo {
public:
    virtual ~Geo() = default;

    // 反射类型
    GeoType getGeoType(); 

    // 接受鼠标和键盘事件来进行绘制或修改(基类Geo里的输入事件不做具体功能，只维护状态)
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    // 点击测试
    virtual bool hitTesting(QPointF);
    // 完成构造
    virtual void completeDrawing();  
    // 绘制方法
    virtual void draw(QPainter& painter) = 0; 

    // 状态的一些修改
    bool isStateDrawing();
    bool isStateComplete();
    bool isStateInvalid();
    bool isStateSelected();
    bool isStateInitialized();

    void setStateSelected();
    void setStateNotSelected();
              
    GeoParameters getGeoParameters();                        // 获取当前的 GeoParameters
    void setGeoParameters(const GeoParameters& params);      // 设置 GeoParameters

protected:
    virtual void initialize();                          // 初始化，在第一次点击（确定第一个控制点）的时候自动调用，来设置属性
    
    virtual void drawControlPoints(QPainter& painter);  // 绘制控制点
    virtual void drawBuffer(QPainter& painter);         // 绘制缓冲区
    
    void setGeoType(GeoType newType);   // 构造函数中,确定类型
    QVector<QPointF> controlPoints;     // 控制点 (每个类可以自己加划分信息)
    QPointF tempControlPoints;          // 临时控制点, 在绘制中使用
    GeoParameters geoParameters;        // 几何的参数

    // 具体操作（默认实现）
    virtual void updateTempPoint(const QPoint& pos);   // 更新临时点
    virtual void endSegmentDrawing();   // 结束段绘制
    virtual void pushControlPoint(const QPoint& pos); // 添加控制点

    bool isMouseLeftButtonPressed();

    void setStateInitialized();
    void setStateInvalid();
    void setStateComplete();

    void markControlPointsChanged();            // 标记控制点已改变
    bool isControlPointsChanged() const;        // 检查控制点是否已改变
    void resetControlPointsChanged();           // 重置标记

    void markBufferChanged();            // 标记控制点已改变
    bool isBufferChanged() const;        // 检查控制点是否已改变
    void resetBufferChanged();           // 重置标记

    QPointF* currentSelectedPoint = nullptr; // 当前选中控制点

private:

    int geoState = 0;                       // 状态
    bool mouseLeftButtonPressed = false;    // 鼠标是否按下拖动
    GeoType geoType = GeoType::Undefined;   // 反射几何的类型
    bool controlPointsChanged = false;      // 控制点变化，要重算
    bool bufferChanged = true;             // 缓冲区变化，要重算
};

// ===================================================================== Point
class Point : public Geo {
public:
    Point();

    // 图形构造事件
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void draw(QPainter& painter) override;

private:
};

// ===================================================================== SimpleLine
class SimpleLine : public Geo {
public:
    SimpleLine();

    // 图形构造事件
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void draw(QPainter& painter) override;
protected:
    QVector<QPointF> points;        // 点集
};

// ===================================================================== DoubleLine
class DoubleLine : public Geo {
public:
    DoubleLine();
    void initialize() override;

    // 图形构造事件
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void pushControlPoint(const QPoint& pos) override;
    void endSegmentDrawing() override;

    void draw(QPainter& painter) override;
    void drawBuffer(QPainter& painter) override;
protected:

    bool isDrawing;                       // 在绘制
    QVector<QVector<QPointF>> pointss;    // 二维点集

    QVector<QVector<QPointF>> buffers;    // 缓冲区

    QVector<Component> component;

};

// ===================================================================== ParallelLine
class ParallelLine : public Geo {
public:
    ParallelLine();
    void initialize() override;

    // 图形构造事件
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void endSegmentDrawing() override;
    void pushControlPoint(const QPoint& pos) override;

    void draw(QPainter& painter) override;
protected:

    QVector<QVector<QPointF>> pointss;    // 二维点集
    QVector<Component> component;
};

// ===================================================================== TwoPointCircle
class TwoPointCircle : public Geo {
public:
    TwoPointCircle();

    // 图形构造事件
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void pushControlPoint(const QPoint& pos) override;

    void draw(QPainter& painter) override;
protected:

    QVector<QPointF> points;       // 点集
};

// ===================================================================== SimpleArea
class SimpleArea : public Geo {
public:
    SimpleArea();

    // 图形构造事件
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void draw(QPainter& painter) override;
protected:

    QVector<QPointF> points;    // 点集
};

// ===================================================================== ComplexArea 
class ComplexArea : public Geo {
public:
    ComplexArea();
    void initialize() override;

    // 图形构造事件
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    virtual void endSegmentDrawing();   // 结束段绘制
    virtual void pushControlPoint(const QPoint& pos); // 添加控制点

    void draw(QPainter& painter) override;
protected:

    bool isDrawing;                       // 在绘制
    QVector<QVector<QPointF>> pointss;    // 二维点集
    QVector<Component> component;
};