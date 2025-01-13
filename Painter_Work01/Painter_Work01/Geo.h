#pragma once

#include <QPainter>
#include <QPointF>
#include <QColor>
#include <QVector>
#include <QKeyEvent>
#include <QPainterPath>
#include "GeoMathUtil.h"
#include "comm.h"

class Geo;
Geo* createGeo(DrawMode mode);

// ===================================================================== Geo
class Geo 
{
public:
    virtual ~Geo() = default;

    // 获取几何类型
    GeoType getGeoType();

    // 接受鼠标和键盘事件进行绘制或修改（基类仅维护状态）
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event, const QPointF& pos);
    virtual void mousePressEvent(QMouseEvent* event, const QPointF& pos);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    // 点击测试
    virtual bool hitTesting(const QPointF& point);

    void dragGeo(const QPointF& point);

    // 完成构造
    virtual void completeDrawing();
    // 绘制方法
    virtual void draw(QPainter& painter) = 0;

    // 状态操作
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
    virtual void initialize();                          // 初始化，在第一次点击时设置属性
    virtual void drawControlPoints(QPainter& painter);  // 绘制控制点
    virtual void drawBuffer(QPainter& painter);         // 绘制缓冲区

    void setGeoType(GeoType newType);   // 构造函数中设置类型

    QVector<QPointF> controlPoints;     // 控制点
    QPointF tempControlPoints;          // 临时控制点
    GeoParameters geoParameters;        // 几何参数

    virtual void updateTempPoint(const QPointF& pos);   // 更新临时点
    virtual void endSegmentDrawing();   // 结束段绘制
    virtual void pushControlPoint(const QPointF& pos);  // 添加控制点

    bool isMouseLeftButtonPressed();

    void setStateInitialized();
    void setStateInvalid();
    void setStateComplete();

    void markControlPointsChanged();            // 标记控制点改变
    bool isControlPointsChanged() const;        // 检查控制点是否改变
    void resetControlPointsChanged();           // 重置标记

    void markBufferChanged();            // 标记缓冲区改变
    bool isBufferChanged() const;        // 检查缓冲区是否改变
    void resetBufferChanged();           // 重置标记

    QPointF* currentSelectedPoint = nullptr; // 当前选中控制点
    QPointF hitPoint;                       // 点击位置（用来记录移动距离）

private:
    
    int geoState = 0;                       // 几何状态
    bool mouseLeftButtonPressed = false;    // 鼠标左键按下标记
    GeoType geoType = GeoType::Undefined;   // 几何类型
    bool controlPointsChanged = false;      // 控制点变化标记
    bool bufferChanged = true;              // 缓冲区变化标记
};

// ===================================================================== Point
class Point : public Geo 
{
public:
    Point();

    void mousePressEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseMoveEvent(QMouseEvent* event, const QPointF& pos) override;
    void completeDrawing() override;

    void draw(QPainter& painter) override;
    void drawBuffer(QPainter& painter) override;

private:
    QPainterPath pathBuffer;                    // 路径缓冲区
    QVector<QVector<QPointF>> buffers;          // 缓冲区
    QPainterPath bufferPath;                    // 路径
};

// ===================================================================== SimpleLine
class SimpleLine : public Geo 
{
public:
    SimpleLine();

    void mousePressEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseMoveEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    bool hitTesting(const QPointF& point) override;

    void draw(QPainter& painter) override;
    void drawBuffer(QPainter& painter) override;

protected:
    QVector<QPointF> points;              // 点集
    QPainterPath path;                    // 路径
    QVector<QVector<QPointF>> buffers;    // 缓冲区
    QPainterPath bufferPath;              // 路径
};

// ===================================================================== DoubleLine
class DoubleLine : public Geo 
{
public:
    DoubleLine();
    void initialize() override;

    void mousePressEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseMoveEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void pushControlPoint(const QPointF& pos) override;
    void endSegmentDrawing() override;

    bool hitTesting(const QPointF& point) override;

    void draw(QPainter& painter) override;
    void drawBuffer(QPainter& painter) override;

protected:
    bool isDrawing;                       // 是否在绘制
    QVector<Component> component;         // 控制点属性
    QVector<QVector<QPointF>> pointss;    // 二维点集
    QPainterPath path;                    // 路径

    QVector<QVector<QPointF>> buffers;    // 缓冲区
    QPainterPath bufferPath;              // 路径
};

// ===================================================================== ParallelLine
class ParallelLine : public Geo 
{
public:
    ParallelLine();
    void initialize() override;

    void mousePressEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseMoveEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    bool hitTesting(const QPointF& point) override;

    void endSegmentDrawing() override;
    void pushControlPoint(const QPointF& pos) override;

    void draw(QPainter& painter) override;
    void drawBuffer(QPainter& painter) override;

protected:
    QVector<QVector<QPointF>> pointss;    // 二维点集
    QVector<Component> component;
    QPainterPath path;                    // 路径

    QVector<QVector<QPointF>> buffers;    // 缓冲区
    QPainterPath bufferPath;              // 路径
};

// ===================================================================== TwoPointCircle
class TwoPointCircle : public Geo 
{
public:
    TwoPointCircle();

    void mousePressEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseMoveEvent(QMouseEvent* event, const QPointF& pos) override;
    void completeDrawing() override;

    void pushControlPoint(const QPointF& pos) override;

    bool hitTesting(const QPointF& point) override;

    void draw(QPainter& painter) override;
    void drawBuffer(QPainter& painter) override;

protected:
    QVector<QPointF> points;             // 点集
    QPainterPath path;                   // 路径

    QVector<QVector<QPointF>> buffers;  // 缓冲区
    QPainterPath bufferPath;            // 路径
};

// ===================================================================== SimpleArea
class SimpleArea : public Geo 
{
public:
    SimpleArea();

    void mousePressEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseMoveEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    bool hitTesting(const QPointF& point) override;

    void draw(QPainter& painter) override;
    void drawBuffer(QPainter& painter) override;

protected:
    QVector<QPointF> points;             // 点集
    QPainterPath path;                   // 路径

    QVector<QVector<QPointF>> buffers;  // 缓冲区
    QPainterPath bufferPath;            // 路径
};

// ===================================================================== ComplexArea 
class ComplexArea : public Geo 
{
public:
    ComplexArea();
    void initialize() override;

    void mousePressEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseMoveEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void endSegmentDrawing() override;
    void pushControlPoint(const QPointF& pos) override;

    bool hitTesting(const QPointF& point) override;

    void draw(QPainter& painter) override;
    void drawBuffer(QPainter& painter) override;

protected:
    bool isDrawing;                       // 是否在绘制
    QVector<QVector<QPointF>> pointss;    // 二维点集
    QPainterPath path;                    // 路径
    QVector<Component> component;

    QVector<QVector<QPointF>> buffers;    // 缓冲区
    QPainterPath bufferPath;              // 路径
};