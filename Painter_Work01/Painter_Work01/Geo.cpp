#include "Geo.h"
#include <QPainterPath>
#include "mathUtil.h"

Geo* createGeo(DrawMode mode)
{
    Geo* geo = nullptr;
    switch (mode)
    {
    case DrawMode::DrawPoint:
        geo = new Point();
        break;
    case DrawMode::DrawPolyline:
        geo = new Polyline();
        break;
    case DrawMode::DrawSpline:
        geo = new Spline();
        break;
    case DrawMode::DrawPolygon:
        geo = new Polygon();
        break;
    default:
        throw std::runtime_error("null DrawMode!!!!");
        break;
    }
    return geo;
}


// =======================================================================================================Shape
GeoType Geo::getGeoType()
{
    return geoType;
}

void Geo::completeDrawing()
{
    setStateComplete();
    setStateNotSelected();
}

bool Geo::isStateDrawing()
{
    return !(geoState & GeoStateComplete);
}

bool Geo::isStateComplete()
{
    return geoState & GeoStateComplete;
}

bool Geo::isStateInvalid()
{
    return geoState & GeoStateInvalid;
}

bool Geo::isStateSelected()
{
    return geoState & GeoStateSelected;
}

void Geo::setStateInvalid()
{

    geoState |=  GeoStateInvalid;
}

void Geo::setStateComplete()
{
    geoState |= GeoStateComplete;
}

void Geo::setStateSelected()
{
    geoState |= GeoStateSelected;
}

void Geo::setStateNotSelected()
{
    geoState &= ~GeoStateSelected;
}




void Geo::keyPressEvent(QKeyEvent* event)
{
}

void Geo::keyReleaseEvent(QKeyEvent* event)
{
}

void Geo::mouseMoveEvent(QMouseEvent* event)
{
}

void Geo::mousePressEvent(QMouseEvent* event)
{
}

void Geo::mouseReleaseEvent(QMouseEvent* event)
{
}

void Geo::wheelEvent(QWheelEvent* event)
{
}

void Geo::setGeoType(GeoType newType)
{
    geoType = newType;
}

// =======================================================================================================ControlPoint
ControlPoint::ControlPoint(QPointF& point): QPointF(point) {}

void ControlPoint::draw(QPainter& painter)
{
    // 设置画笔和画刷
    painter.setPen(QPen(Qt::lightGray, 2));
    painter.setBrush(Qt::NoBrush);

    // 绘制控制点
    painter.drawRect(x() - 6, y() - 6, 12, 12);

    painter.setBrush(QBrush(Qt::red));
    painter.setPen(Qt::NoPen);
    painter.drawRect(x() - 5, y() - 5, 10, 10);

    painter.setBrush(Qt::NoBrush);
}

// =======================================================================================================Point

Point::Point()
{
    setGeoType(GeoType::TypePoint);
    setStateSelected(); // 还在绘制中，是当前选中
}

Point::~Point() 
{
}

QPointF Point::getPosition() const
{
    return position;
}

void Point::setPosition(const QPointF& newPos)
{
    position = newPos;
}

QColor Point::getColor() const
{
    return color;
}

void Point::setColor(const QColor& newColor)
{
    color = newColor;
}

PointShape Point::getShape() const
{
    return shape;
}

void Point::setShape(PointShape newShape)
{
    shape = newShape;
}

void Point::draw(QPainter& painter)
{
    if (isStateInvalid())
    {
        return;
    }
    QPainterPath path;


    if (isStateDrawing())
    {
        if (shape == PointShape::Square) {
            path.addRect(tempPoint.x() - 5, tempPoint.y() - 5, 10, 10);
        }
        else if (shape == PointShape::Circle) {
            path.addEllipse(tempPoint, 5.0, 5.0);
        }
        painter.setBrush(QBrush(color));
        painter.setPen(Qt::NoPen);
        painter.drawPath(path);
        if (isStateSelected())
        {
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(Qt::lightGray, 2));

            if (shape == PointShape::Square) {
                painter.drawRect(position.x() - 6, position.y() - 6, 12, 12);
            }
            else if (shape == PointShape::Circle) {
                painter.drawEllipse(position, 6.0, 6.0);
            }
        }
    }
    else
    {
        if (shape == PointShape::Square) {
            path.addRect(position.x() - 5, position.y() - 5, 10, 10);
        }
        else if (shape == PointShape::Circle) {
            path.addEllipse(position, 5.0, 5.0);
        }
        painter.setBrush(QBrush(color));
        painter.setPen(Qt::NoPen);
        painter.drawPath(path);

        if (isStateSelected())
        {
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(Qt::lightGray, 2));

            if (shape == PointShape::Square) {
                painter.drawRect(position.x() - 6, position.y() - 6, 12, 12);
            }
            else if (shape == PointShape::Circle) {
                painter.drawEllipse(position, 6.0, 6.0);
            }
        }
    }
    painter.setBrush(Qt::NoBrush);
}


void Point::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        completeDrawing();
    }
    else
    {
        if (isStateDrawing())
        {
            position = event->pos();
            completeDrawing();
        }
    }
}

void Point::mouseMoveEvent(QMouseEvent* event)
{
    if (!isStateComplete())
    {
        tempPoint = event->pos();
    }
}

void Point::completeDrawing()
{
    if (position.isNull())
    {
        setStateInvalid();
    }
    Geo::completeDrawing();
}

// =======================================================================================================BaseLine
BaseLine::BaseLine()
{
}

BaseLine::~BaseLine() 
{
}

void BaseLine::setColor(QColor color) {
    this->color = color;
}

QColor BaseLine::getColor() const {
    return color;
}

void BaseLine::setLineStyle(LineStyle lineStyle)
{
    this->lineStyle = lineStyle;
}

LineStyle BaseLine::getLineStyle() const {
    return lineStyle;
}

void BaseLine::setLineWidth(float width) {
    this->lineWidth = width;
}

float BaseLine::getLineWidth() const {
    return lineWidth;
}

void BaseLine::setDashPattern(float pattern) {
    this->dashPattern = pattern;
}

float BaseLine::getDashPattern() const {
    return dashPattern;
}

void BaseLine::pushPoint(QPointF& point)
{
    controlPoints.push_back(ControlPoint(point));
}

void BaseLine::mouseMoveEvent(QMouseEvent* event)
{
    if (isStateDrawing())
    {
        tempControlPoint = event->pos();
    }
}

void BaseLine::completeDrawing()
{
    // 检查是否合法
    if (controlPoints.size() < 2)
    {
        setStateInvalid();
    }
    Geo::completeDrawing();
}

QVector<QPointF> BaseLine::getPoints()
{
    return QVector<QPointF>(controlPoints.begin(), controlPoints.end());
}

// =======================================================================================================Polyline
Polyline::Polyline()
{
    setGeoType(GeoType::TypePolyline);
    setStateSelected(); // 还在绘制中，是当前选中
}

Polyline::~Polyline()
{
}

// 绘制方法
void Polyline::draw(QPainter& painter) 
{
    if (isStateInvalid())
    {
        return;
    }
    QPen pen;
    pen.setColor(color); 
    pen.setWidthF(lineWidth);

    if (lineStyle == LineStyle::Dashed) 
    {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(dashPattern);
    }
    else 
    {
        pen.setStyle(Qt::SolidLine);
    }
    painter.setPen(pen);

    QPainterPath path;

    if (!controlPoints.isEmpty()) 
    {
        path.moveTo(controlPoints.first());
        for (int i = 1; i < controlPoints.size(); ++i) 
        {
            path.lineTo(controlPoints[i]);
        }
    }

    if (isStateDrawing())
    {
        if (controlPoints.isEmpty())
        {
            path.moveTo(tempControlPoint);
        }
        else
        {
            path.lineTo(tempControlPoint);
        }
    }

    painter.drawPath(path);

    // 如果被选中,画控制点
    if (isStateSelected())
    {
        for (auto& point : controlPoints)
        {
            point.draw(painter);
        }
        if (isStateDrawing())
        {
            tempControlPoint.draw(painter);
        }
    }
}


void Polyline::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        completeDrawing();
    }
    else
    {
        if (isStateDrawing())
        {
            QPointF point = event->pos();
            pushPoint(point);
        }
    }
}

// ====================================================Spline
Spline::Spline()
{
    setGeoType(GeoType::TypeSpline);
    setStateSelected(); // 还在绘制中，是当前选中
}

Spline::~Spline()
{
}

void Spline::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        completeDrawing();
    }
    else
    {
        if (isStateDrawing())
        {
            QPointF point = event->pos();
            pushPoint(point);
            curvePoints = mathUtil::calculateBSpline(controlPoints, 3, 20*controlPoints.size());
        }
    }
}

QVector<QPointF> Spline::getPoints()
{
    return curvePoints;
}

void Spline::draw(QPainter& painter) 
{
    if (isStateInvalid())
    {
        return;
    }

    QPen pen;
    pen.setColor(color);
    pen.setWidthF(lineWidth);

    // 设置虚线样式
    if (lineStyle == LineStyle::Dashed) {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(dashPattern);
    }
    else {
        pen.setStyle(Qt::SolidLine);
    }

    painter.setPen(pen);

    if (!curvePoints.isEmpty()) {
        QPainterPath path;
        path.moveTo(curvePoints[0]);

        for (int i = 1; i < curvePoints.size(); ++i) {
            path.lineTo(curvePoints[i]);
        }

        painter.drawPath(path);
    }

    // 如果被选中,画控制点
    if (isStateSelected())
    {
        for (auto& point : controlPoints)
        {
            point.draw(painter);
        }
        if (isStateDrawing())
        {
            tempControlPoint.draw(painter);
        }
    }
}
// ===================================================================== Polygon
Polygon::Polygon()
{
    setGeoType(GeoType::TypePolygon);
    setStateSelected(); // 还在绘制中，是当前选中

    edges = static_cast<BaseLine*>(createGeo(getSetting<DrawMode>(Key_PgLineMode)));
    edges->setColor(lineColor);
    edges->setLineWidth(lineWidth);
    edges->setLineStyle(lineStyle);
}

Polygon::~Polygon()
{
}

void Polygon::setFillColor(const QColor& color)
{
    fillColor = color;
}

QColor Polygon::getFillColor() const
{
    return fillColor;
}

void Polygon::setBorderColor(const QColor& color)
{
    lineColor = color;
}

QColor Polygon::getBorderColor() const
{
    return  lineColor;
}

void Polygon::setBorderStyle(LineStyle style)
{
    lineStyle = style;
}

LineStyle Polygon::getBorderStyle() const
{
    return  lineStyle;
}

void Polygon::setBorderWidth(float width)
{
    lineWidth = width;
}

float Polygon::getBorderWidth() const
{
    return lineWidth;
}

void Polygon::draw(QPainter& painter)
{
    if (!edges || isStateInvalid())
    {
        return;
    }
    edges->draw(painter);

    QBrush brush(fillColor);
    painter.setBrush(brush);
    QPainterPath path;

    const QVector<QPointF>& points = edges->getPoints();

    if (isStateComplete() && points.size())
    {
        path.moveTo(points[0]);
        for (int i = 1; i < points.size(); ++i)
        {
            path.lineTo(points[i]);
        }
        path.closeSubpath(); // 闭合路径
    }

    if (isStateDrawing() && points.size())
    {
        QPointF& tempPoint = edges->tempControlPoint;
        path.moveTo(points[0]);
        for (int i = 1; i < points.size(); ++i)
        {
            path.lineTo(points[i]);
        }
        path.lineTo(tempPoint); // 临时点也加入路径
        path.closeSubpath(); // 闭合路径
    }

    painter.drawPath(path);
    painter.setBrush(Qt::NoBrush); // 恢复无填充     
}


void Polygon::mousePressEvent(QMouseEvent* event)
{
    if (isStateComplete())
    {
        return;
    }
    if (event->button() == Qt::RightButton)
    {
        completeDrawing();
    }
    else
    {
        edges->mousePressEvent(event);
    }
}

void Polygon::mouseMoveEvent(QMouseEvent* event)
{
    edges->mouseMoveEvent(event);
}

void Polygon::completeDrawing()
{
    // 检查是否合法
    if (edges->isStateInvalid()||edges->controlPoints.size() < 3)
    {
        setStateInvalid();
    }

    // 把起始点也放入图形，来计算曲线(但,终点和起点放一起也很奇怪)
    // todo

    edges->completeDrawing();
    Geo::completeDrawing();
}

 

