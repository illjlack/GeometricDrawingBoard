#pragma once

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QPoint>
#include <vector>

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

protected:
    void paintEvent(QPaintEvent* event) override;        // 重绘事件
    void mousePressEvent(QMouseEvent* event) override;   // 鼠标按下事件
    void mouseMoveEvent(QMouseEvent* event) override;    // 鼠标移动事件
    void mouseReleaseEvent(QMouseEvent* event) override; // 鼠标释放事件

private:
    DrawMode currentMode;           // 当前绘制模式
    DrawState currentState;         // 当前绘制状态
    std::vector<QPoint> points;     // 已绘制的点
    std::vector<std::vector<QPoint>> lines;    // 已绘制的线段
    std::vector<std::vector<QPoint>> polygons; // 已绘制的多边形

    QPoint tempStartPoint; // 当前线条或多边形的起点
};
