#include "Canvas.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>

Canvas::Canvas(QWidget* parent)
    : QWidget(parent), currentMode(None)
{
    setMouseTracking(true); // ����������
}

Canvas::~Canvas() {}

void Canvas::setDrawMode(DrawMode mode)
{
    currentMode = mode;
    update(); // ���»���
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
    //QFile file(fileName);
    //if (!file.open(QIODevice::WriteOnly)) {
    //    return false;
    //}

    //QJsonDocument doc;
    //QJsonArray pointArray;
    //for (const QPoint& point : points) {
    //    QJsonArray pointData;
    //    pointData.append(point.x());
    //    pointData.append(point.y());
    //    pointArray.append(pointData);
    //}
    //// todo
    //doc.setArray(pointArray);
    //file.write(doc.toJson());
    //file.close();
    return true;
}

bool Canvas::exportToShp(const QString& fileName)
{
    // todo
    QMessageBox::information(this, tr("������ SHP �ļ�"), tr("����������δʵ��"));
    return false;
}

void Canvas::pushShape(Geo* geo)
{
    vec.push_back(geo);
}

Geo* Canvas::createShape()
{
    using Mode = Canvas::DrawMode;
    Geo* geo = nullptr;

    if (Mode::DrawPoint == currentMode)
    {
        geo = new Point();
    }
    else if (Mode::DrawPolyline == currentMode)
    {
        geo = new Polyline();
    }
    else if (Mode::DrawSpline == currentMode)
    {
        geo = new Spline();
    }
    else if (Mode::DrawPolygon == currentMode)
    {
        geo = new Polygon();
    }

    return geo;
}

void Canvas::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // ���ÿ����
    painter.fillRect(this->rect(), Qt::white); // ���ư�ɫ����
    
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


void Canvas::mousePressEvent(QMouseEvent* event)
{
    //if (currentMode == DrawPoint) {
    //    points.push_back(event->pos());
    //}
    //else if (currentMode == DrawLine || currentMode == DrawPolygon) {
    //    tempStartPoint = event->pos();
    //}
    //update();
    
        // ��ȡ���λ��
    //QPointF clickPos = event->pos();

    //// �жϵ�ǰ����ģʽ
    //if (currentMode == DrawPoint) {
    //    // ����һ������󣬴�������λ�ã���ɫ����״���Ը�����Ҫ����
    //    Point newPoint(clickPos, Qt::black, Point::Shape::Circle);

    //    // ���´����ĵ���ӵ��㼯����
    //    points.push_back(newPoint);

    //    // ���»��������ػ�
    //    update();
    //}

    if (Canvas::DrawMode::None != currentMode)
    {
        if (!currentDrawGeo || currentDrawGeo->getGeoDrawState() == Geo::GeoDrawState::Complete)
        {
            currentDrawGeo = createShape();
            pushShape(currentDrawGeo);
        }
        currentDrawGeo->mousePressEvent(event);
    }
    update();
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    // ʵʱ���ƿ��������ﴦ��
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

void Canvas::wheelEvent(QWheelEvent* event)
{
}
