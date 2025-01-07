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

    // �ļ������ӿ�
    bool loadFromFile(const QString& fileName);
    bool saveToFile(const QString& fileName);
    bool exportToShp(const QString& fileName);

    void pushShape(Geo* shape);
    void CompleteDrawing();

protected:
    void paintEvent(QPaintEvent* event) override;        // 
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;   // ��갴���¼�
    void mouseMoveEvent(QMouseEvent* event) override;    // ����ƶ��¼�
    void mouseReleaseEvent(QMouseEvent* event) override; // ����ͷ��¼�
    void wheelEvent(QWheelEvent* event) override;

private:

    Geo* currentDrawGeo = nullptr;
    std::vector<Geo*> vec;
};
