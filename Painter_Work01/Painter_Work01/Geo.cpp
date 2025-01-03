#include "Geo.h"
#include <QPainterPath>
#include "mathUtil.h"

// =======================================================================================================Shape
Geo::GeoType Geo::getType()
{
	return geoType;
}

Geo::GeoDrawState Geo::getGeoDrawState()
{
    return geoDrawState;
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

void Geo::setGeoType(Geo::GeoType newType)
{
    geoType = newType;
}

void Geo::setGeoDrawState(GeoDrawState newState)
{
    geoDrawState = newState;
}

// =======================================================================================================Point

Point::Point():color(Qt::red)
{
}

Point::Point(const QPointF& position, QColor color, Shape shape)
    : position(position), color(color), shape(shape)
{
    setGeoType(Geo::GeoType::Point);
    setGeoDrawState(Geo::GeoDrawState::Complete);
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

Point::Shape Point::getShape() const
{
    return shape;
}

void Point::setShape(Shape newShape)
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

    if (shape == Shape::Square) {
        path.addRect(position.x() - 5, position.y() - 5, 10, 10);  // ����һ�� 10x10 �ľ���
    }
    else if (shape == Shape::Circle) {
        path.addEllipse(position, 5.0, 5.0);  // ���ư뾶Ϊ 5 ��Բ
    }

    painter.drawPath(path);
}

void Point::mousePressEvent(QMouseEvent* event)
{
    if (getGeoDrawState() == Geo::GeoDrawState::Drawing)
    {
        position = event->pos();
        setGeoDrawState(Geo::GeoDrawState::Complete);
    }
}

// =======================================================================================================BaseLine
// ���캯��
BaseLine::BaseLine()
{
}

BaseLine::BaseLine(const QVector<QPointF>& points,
    QColor color,
    Style style,
    float width,
    float dashPattern)
    : points(points), color(color), style(style), lineWidth(width), dashPattern(dashPattern) {}

// ��������
BaseLine::~BaseLine() {}

// ���õ㼯
void BaseLine::setPoints(const QVector<QPointF>& points) {
    this->points = points;
}

// ��ȡ�㼯
QVector<QPointF> BaseLine::getPoints() const {
    return points;
}

// ������ɫ
void BaseLine::setColor(QColor color) {
    this->color = color;
}

// ��ȡ��ɫ
QColor BaseLine::getColor() const {
    return color;
}

// ��������
void BaseLine::setStyle(Style style) {
    this->style = style;
}

// ��ȡ����
BaseLine::Style BaseLine::getStyle() const {
    return style;
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

// =======================================================================================================Polyline
// ���캯��
Polyline::Polyline(const QVector<QPointF>& points,
    QColor color,
    Style style,
    float width)
    : BaseLine(points, color, style, width) 
{
    setGeoDrawState(Geo::GeoDrawState::Complete);
}

Polyline::Polyline()
{
}


// ���Ʒ���
void Polyline::draw(QPainter& painter) {
    // ���û���
    QPen pen;
    pen.setColor(color);
    pen.setWidthF(lineWidth);

    // ����������ʽ
    if (style == Style::Dashed) {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(dashPattern);  // �������߶εĳ���
    }
    else {
        pen.setStyle(Qt::SolidLine);
    }

    painter.setPen(pen);

    painter.drawPolyline(points);

}

void Polyline::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        setGeoDrawState(Geo::GeoDrawState::Complete);
    }
    else
    {
        if (getGeoDrawState() == Geo::GeoDrawState::Drawing)
        {
            points.push_back(event->pos());
        }
    }
}

// ====================================================Spline
Spline::Spline(const QVector<QPointF>& points,
    QColor color,
    Style style,
    float width) : BaseLine(points, color, style, width)
{
    setGeoDrawState(Geo::GeoDrawState::Complete);
}

Spline::Spline()
{
}

void Spline::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        setGeoDrawState(Geo::GeoDrawState::Complete);
    }
    else
    {
        if (getGeoDrawState() == Geo::GeoDrawState::Drawing)
        {
            points.push_back(event->pos());
            curvePoints = mathUtil::calculateBSpline(points, 3, 10*points.size());
        }
    }
}


void Spline::draw(QPainter& painter) {
    // ���û���
    QPen pen;
    pen.setColor(color);
    pen.setWidthF(lineWidth);

    // ����������ʽ
    if (style == Style::Dashed) {
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
        painter.drawPolyline(points);
    }
}
