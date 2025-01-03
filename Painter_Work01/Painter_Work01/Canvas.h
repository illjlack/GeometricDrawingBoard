#pragma once

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QPoint>
#include <vector>
#include "Geo.h"

class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget* parent = nullptr);
    ~Canvas();

    // 绘制模式
    enum DrawMode {
        None,       
        DrawPoint,
        DrawPolyline,              // 折线
        DrawSpline,                // 样条线
        DrawArcThreePoints,        // 三点圆弧
        DrawArcTwoPoints,          // 两点圆弧
        DrawStreamline             // 流线
    };

    // 绘制状态
    enum DrawState {
        Idle,       // 空闲状态
        Drawing     // 正在绘制
    };

    void setDrawMode(DrawMode mode); // 设置绘制模式

    // 文件操作接口
    bool loadFromFile(const QString& fileName);
    bool saveToFile(const QString& fileName);
    bool exportToShp(const QString& fileName);

    void pushShape(Geo* shape);

    Geo* createShape();

protected:
    void paintEvent(QPaintEvent* event) override;        // 
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;   // 鼠标按下事件
    void mouseMoveEvent(QMouseEvent* event) override;    // 鼠标移动事件
    void mouseReleaseEvent(QMouseEvent* event) override; // 鼠标释放事件
    void wheelEvent(QWheelEvent* event) override;

private:
    DrawMode currentMode;           // 当前绘制模式
    DrawState currentState;         // 当前绘制状态

    Geo* currentDrawGeo = nullptr;
    std::vector<Geo*> vec;
};
