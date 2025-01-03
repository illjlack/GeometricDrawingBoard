#pragma once

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QPoint>
#include <vector>
#include "Shapes.h"

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
        DrawLine,   
        DrawPolygon 
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

    void pushShape(Shape* shape);

protected:
    void paintEvent(QPaintEvent* event) override;        // 重绘事件
    void mousePressEvent(QMouseEvent* event) override;   // 鼠标按下事件
    void mouseMoveEvent(QMouseEvent* event) override;    // 鼠标移动事件
    void mouseReleaseEvent(QMouseEvent* event) override; // 鼠标释放事件

private:
    DrawMode currentMode;           // 当前绘制模式
    DrawState currentState;         // 当前绘制状态

    std::vector<Shape*> vec;
};
