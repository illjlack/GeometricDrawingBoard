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

    // ����ģʽ
    enum DrawMode {
        None,       
        DrawPoint,  
        DrawLine,   
        DrawPolygon 
    };

    // ����״̬
    enum DrawState {
        Idle,       // ����״̬
        Drawing     // ���ڻ���
    };

    void setDrawMode(DrawMode mode); // ���û���ģʽ

    // �ļ������ӿ�
    bool loadFromFile(const QString& fileName);
    bool saveToFile(const QString& fileName);
    bool exportToShp(const QString& fileName);

    void pushShape(Shape* shape);

protected:
    void paintEvent(QPaintEvent* event) override;        // �ػ��¼�
    void mousePressEvent(QMouseEvent* event) override;   // ��갴���¼�
    void mouseMoveEvent(QMouseEvent* event) override;    // ����ƶ��¼�
    void mouseReleaseEvent(QMouseEvent* event) override; // ����ͷ��¼�

private:
    DrawMode currentMode;           // ��ǰ����ģʽ
    DrawState currentState;         // ��ǰ����״̬

    std::vector<Shape*> vec;
};
