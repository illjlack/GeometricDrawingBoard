#include "Canvas.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include "comm.h"

Canvas::Canvas(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true); // 启用鼠标跟踪
}

Canvas::~Canvas() {}

bool Canvas::loadFromFile(const QString& fileName)
{
    return true;
}

bool Canvas::saveToFile(const QString& fileName)
{
    return true;
}

bool Canvas::exportToShp(const QString& fileName)
{
    QMessageBox::information(this, tr("导出到 SHP 文件"), tr("导出功能暂未实现"));
    return false;
}

void Canvas::pushShape(Geo* geo)
{
    vec.push_back(geo);
}

void Canvas::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 启用抗锯齿
    painter.fillRect(this->rect(), Qt::white); // 绘制白色背景
    
    for (auto shape : vec)
    {
        shape->draw(painter);
    }
}

void Canvas::keyPressEvent(QKeyEvent* event)
{
}

void Canvas::keyReleaseEvent(QKeyEvent* event)
{
}


// 强制完成绘制
void Canvas::CompleteDrawing()
{
    if (currentDrawGeo)currentDrawGeo->completeDrawing();
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    if (DrawMode::None != getSetting<DrawMode>(Key_DrawMode))
    {
        if (!currentDrawGeo || currentDrawGeo->isStateComplete())
        {
            currentDrawGeo = createGeo(getSetting<DrawMode>(Key_DrawMode));
            pushShape(currentDrawGeo);
        }
        currentDrawGeo->mousePressEvent(event);
    }
    update();
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    if (DrawMode::None != getSetting<DrawMode>(Key_DrawMode))
    {
        if (!currentDrawGeo || currentDrawGeo->isStateComplete())
        {
            currentDrawGeo = createGeo(getSetting<DrawMode>(Key_DrawMode));
            pushShape(currentDrawGeo);
        }
        currentDrawGeo->mouseMoveEvent(event);
    }
    update();
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
}

void Canvas::wheelEvent(QWheelEvent* event)
{
}
