#include "Canvas.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>

Canvas::Canvas(QWidget* parent)
    : QWidget(parent), currentMode(None)
{
    setMouseTracking(true); // 启用鼠标跟踪
}

Canvas::~Canvas() {}

void Canvas::setDrawMode(DrawMode mode)
{
    currentMode = mode;
    update(); // 更新画布
}

bool Canvas::loadFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());

    // todo
    file.close();
    update();
    return true;
}

bool Canvas::saveToFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonDocument doc;
    QJsonArray pointArray;
    for (const QPoint& point : points) {
        QJsonArray pointData;
        pointData.append(point.x());
        pointData.append(point.y());
        pointArray.append(pointData);
    }
    // todo
    doc.setArray(pointArray);
    file.write(doc.toJson());
    file.close();
    return true;
}

bool Canvas::exportToShp(const QString& fileName)
{
    // todo
    QMessageBox::information(this, tr("导出到 SHP 文件"), tr("导出功能暂未实现"));
    return false;
}

void Canvas::paintEvent(QPaintEvent* event)
{
    //QPainter painter(this);
    //painter.setRenderHint(QPainter::Antialiasing); // 启用抗锯齿

    //// 绘制点
    //painter.setPen(Qt::black);
    //for (const QPoint& point : points) {
    //    painter.drawEllipse(point, 5, 5);
    //}

    //// 绘制线
    //painter.setPen(QPen(Qt::blue, 2));
    //for (const auto& line : lines) {
    //    for (size_t i = 1; i < line.size(); ++i) {
    //        painter.drawLine(line[i - 1], line[i]);
    //    }
    //}

    //// 绘制多边形
    //painter.setPen(QPen(Qt::green, 2));
    //for (const auto& polygon : polygons) {
    //    if (polygon.size() > 2) {
    //        painter.drawPolygon(polygon.data(), polygon.size());
    //    }
    //}
}


void Canvas::mousePressEvent(QMouseEvent* event)
{
    //if (currentMode == DrawPoint) {
    //    points.push_back(event->pos());
    //}
    //else if (currentMode == DrawLine || currentMode == DrawPolygon) {
    //    tempStartPoint = event->pos();
    //}
    //update();
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    // 实时绘制可以在这里处理
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    //if (currentMode == DrawLine) {
    //    lines.push_back({ tempStartPoint, event->pos() });
    //}
    //else if (currentMode == DrawPolygon) {
    //    if (polygons.empty() || polygons.back().empty()) {
    //        polygons.push_back({ tempStartPoint, event->pos() });
    //    }
    //    else {
    //        polygons.back().push_back(event->pos());
    //    }
    //}
    //update();
}