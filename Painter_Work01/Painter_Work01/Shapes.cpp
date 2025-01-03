#include "Shapes.h"
#include <QPainterPath>

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
    // 设置画笔颜色和无边框
    painter.setBrush(QBrush(color));
    painter.setPen(Qt::NoPen);

    // 创建一个 QPainterPath
    QPainterPath path;

    if (shape == Shape::Square) {
        path.addRect(position.x() - 5, position.y() - 5, 10, 10);  // 绘制一个 10x10 的矩形
    }
    else if (shape == Shape::Circle) {
        path.addEllipse(position, 5.0, 5.0);  // 绘制半径为 5 的圆
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
// 构造函数
BaseLine::BaseLine()
{
}

BaseLine::BaseLine(const QVector<QPointF>& points,
    QColor color,
    Style style,
    float width,
    float dashPattern)
    : points(points), color(color), style(style), lineWidth(width), dashPattern(dashPattern) {}

// 析构函数
BaseLine::~BaseLine() {}

// 设置点集
void BaseLine::setPoints(const QVector<QPointF>& points) {
    this->points = points;
}

// 获取点集
QVector<QPointF> BaseLine::getPoints() const {
    return points;
}

// 设置颜色
void BaseLine::setColor(QColor color) {
    this->color = color;
}

// 获取颜色
QColor BaseLine::getColor() const {
    return color;
}

// 设置线型
void BaseLine::setStyle(Style style) {
    this->style = style;
}

// 获取线型
BaseLine::Style BaseLine::getStyle() const {
    return style;
}

// 设置线宽
void BaseLine::setLineWidth(float width) {
    this->lineWidth = width;
}

// 获取线宽
float BaseLine::getLineWidth() const {
    return lineWidth;
}

// 设置虚线段长
void BaseLine::setDashPattern(float pattern) {
    this->dashPattern = pattern;
}

// 获取虚线段长
float BaseLine::getDashPattern() const {
    return dashPattern;
}

// =======================================================================================================Polyline
// 构造函数
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


// 绘制方法
void Polyline::draw(QPainter& painter) {
    // 设置画笔
    QPen pen;
    pen.setColor(color);
    pen.setWidthF(lineWidth);

    // 设置虚线样式
    if (style == Style::Dashed) {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(dashPattern);  // 设置虚线段的长度
    }
    else {
        pen.setStyle(Qt::SolidLine);
    }

    painter.setPen(pen);

    // 绘制折线
    if (points.size() > 1) {
        painter.drawPolyline(points);
    }
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


