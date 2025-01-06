#include "Geo.h"
#include <QPainterPath>
#include "mathUtil.h"

// =======================================================================================================Shape
GeoType Geo::getType()
{
	return geoType;
}

GeoDrawState Geo::getGeoDrawState()
{
    return geoDrawState;
}

bool Geo::getIsInvalid()
{
    return isInvalid;
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

void Geo::setGeoDrawState(GeoDrawState newState)
{
    geoDrawState = newState;
}

void Geo::setIsInvalid(bool flag)
{
    isInvalid = true;
}

// =======================================================================================================Point

Point::Point()
{
}

Point::Point(const QPointF& position, QColor color, PointShape shape)
    : position(position), color(color), shape(shape)
{
    setGeoType(GeoType::TypePoint);
    setGeoDrawState(GeoDrawState::Complete);
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
    // ���û�����ɫ���ޱ߿�
    painter.setBrush(QBrush(color));
    painter.setPen(Qt::NoPen);

    // ����һ�� QPainterPath
    QPainterPath path;

    if (shape == PointShape::Square) {
        path.addRect(position.x() - 5, position.y() - 5, 10, 10);  // ����һ�� 10x10 �ľ���
    }
    else if (shape == PointShape::Circle) {
        path.addEllipse(position, 5.0, 5.0);  // ���ư뾶Ϊ 5 ��Բ
    }

    painter.drawPath(path);

    painter.setBrush(Qt::NoBrush);
}

void Point::mousePressEvent(QMouseEvent* event)
{
    if (getGeoDrawState() == GeoDrawState::Drawing)
    {
        position = event->pos();
        setGeoDrawState(GeoDrawState::Complete);
    }
}

// =======================================================================================================BaseLine
// ���캯��
BaseLine::BaseLine()
{
}

// ��������
BaseLine::~BaseLine() {}

// ���õ㼯
void BaseLine::setPoints(const QVector<QPointF>& points) {
    this->controlPoint = points;
}

// ��ȡ�㼯
QVector<QPointF> BaseLine::getPoints() const {
    return controlPoint;
}

// ������ɫ
void BaseLine::setColor(QColor color) {
    this->color = color;
}

// ��ȡ��ɫ
QColor BaseLine::getColor() const {
    return color;
}


void BaseLine::setLineStyle(LineStyle lineStyle)
{
    this->lineStyle = lineStyle;
}

// ��ȡ����
LineStyle BaseLine::getLineStyle() const {
    return lineStyle;
}

// �����߿�
void BaseLine::setLineWidth(float width) {
    this->lineWidth = width;
}

// ��ȡ�߿�
float BaseLine::getLineWidth() const {
    return lineWidth;
}

// �������߶γ�
void BaseLine::setDashPattern(float pattern) {
    this->dashPattern = pattern;
}

// ��ȡ���߶γ�
float BaseLine::getDashPattern() const {
    return dashPattern;
}

void BaseLine::pushPoint(QPointF& point)
{
    controlPoint.push_back(point);
}

// =======================================================================================================Polyline
Polyline::Polyline()
{
}
Polyline::~Polyline()
{
}

// ���Ʒ���
void Polyline::draw(QPainter& painter) {
    // ���û���
    QPen pen;
    pen.setColor(color);
    pen.setWidthF(lineWidth);

    // ����������ʽ
    if (lineStyle == LineStyle::Dashed) {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(dashPattern);  // �������߶εĳ���
    }
    else {
        pen.setStyle(Qt::SolidLine);
    }

    painter.setPen(pen);

    painter.drawPolyline(controlPoint);

}

void Polyline::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        setGeoDrawState(GeoDrawState::Complete);
    }
    else
    {
        if (getGeoDrawState() == GeoDrawState::Drawing)
        {
            QPointF point = event->pos();
            pushPoint(point);
        }
    }
}

// ====================================================Spline
Spline::Spline()
{
}

Spline::~Spline()
{
}

void Spline::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        setGeoDrawState(GeoDrawState::Complete);
    }
    else
    {
        if (getGeoDrawState() == GeoDrawState::Drawing)
        {
            QPointF point = event->pos();
            pushPoint(point);
            curvePoints = mathUtil::calculateBSpline(controlPoint, 3, 10*controlPoint.size());
        }
    }
}


void Spline::draw(QPainter& painter) {
    // ���û���
    QPen pen;
    pen.setColor(color);
    pen.setWidthF(lineWidth);

    // ����������ʽ
    if (lineStyle == LineStyle::Dashed) {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(dashPattern);  // �������߶εĳ���
    }
    else {
        pen.setStyle(Qt::SolidLine);
    }

    painter.setPen(pen);

    // ����Ѿ�������������ߵĵ㣬�������������
    if (!curvePoints.isEmpty()) {
        // ���� QPainterPath ���ڻ���ƽ������
        QPainterPath path;
        path.moveTo(curvePoints[0]);  // ���������Ϊ���ߵĵ�һ����

        // ʹ�� QPainterPath �𲽼��������
        for (int i = 1; i < curvePoints.size(); ++i) {
            path.lineTo(curvePoints[i]);  // �����������ӳ�һ������
        }

        // ʹ�� QPainterPath ����ƽ������
        painter.drawPath(path);
    }
    else
    {
        // ���û�м�����������ߵ㣬��������������
        painter.drawPolyline(controlPoint);
    }
}
// ===================================================================== Polygon
Polygon::Polygon()
{
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
    if (!edges) {
        return; // ���û�б߽����ݣ�ֱ�ӷ���
    }
    QPen pen(Qt::black, 2);
    painter.setPen(pen);

    edges->draw(painter);

    if (getGeoDrawState() == GeoDrawState::Complete) {
        QBrush brush(Qt::blue); // ���Ϊ��ɫ
        
        painter.setBrush(brush);

        QPainterPath path;
        QVector<QPointF> points = edges->getPoints();
        if (!points.isEmpty()) {
            path.moveTo(points[0]);
            for (int i = 1; i < points.size(); ++i) {
                path.lineTo(points[i]);
            }
            path.closeSubpath(); // �պ�·��
            painter.drawPath(path);
        }

        painter.setBrush(Qt::NoBrush); //�ָ������
    }


}

void Polygon::mousePressEvent(QMouseEvent* event)
{
    if (getGeoDrawState() == GeoDrawState::Complete)
    {
        return;
    }

    if (event->button() == Qt::RightButton)
    {
        setGeoDrawState(GeoDrawState::Complete);

        if (!edges)
        {
            setIsInvalid(true); // ����
            return;
        }
        QPointF* firstPoint = edges->controlPoint.begin();
        if (!firstPoint)
        {
            setIsInvalid(true); // ����
            return;
        }
        // ֱ��path.closeSubpath();�պ�·��
        // edges->pushPoint(*firstPoint);
        edges->setGeoDrawState(GeoDrawState::Complete);
        setGeoDrawState(GeoDrawState::Complete);
    }
    else
    {
        if (!edges)
        {
            edges = new Polyline();
        }
        edges->mousePressEvent(event);
    }
}

