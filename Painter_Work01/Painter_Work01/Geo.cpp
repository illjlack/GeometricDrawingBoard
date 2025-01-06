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

void Geo::setGeoType(Geo::GeoType newType)
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

void BaseLine::pushPoint(QPointF& point)
{
    points.push_back(point);
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
            QPointF point = event->pos();
            pushPoint(point);
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
            QPointF point = event->pos();
            pushPoint(point);
            curvePoints = mathUtil::calculateBSpline(points, 3, 10*points.size());
        }
    }
}


void Spline::draw(QPainter& painter) {
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

    // 如果已经计算出样条曲线的点，则绘制样条曲线
    if (!curvePoints.isEmpty()) {
        // 创建 QPainterPath 用于绘制平滑曲线
        QPainterPath path;
        path.moveTo(curvePoints[0]);  // 将起点设置为曲线的第一个点

        // 使用 QPainterPath 逐步加入后续点
        for (int i = 1; i < curvePoints.size(); ++i) {
            path.lineTo(curvePoints[i]);  // 将后续点连接成一条折线
        }

        // 使用 QPainterPath 绘制平滑曲线
        painter.drawPath(path);
    }
    else
    {
        // 如果没有计算出样条曲线点，则绘制输入的折线
        painter.drawPolyline(points);
    }
}
// ===================================================================== Polygon
Polygon::Polygon(const QVector<QPointF>& points, const QColor& fillColor, const QColor& borderColor, BaseLine::Style borderStyle, float borderWidth)
{
}

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
    borderColor = color;
}

QColor Polygon::getBorderColor() const
{
    return  borderColor;
}

void Polygon::setBorderStyle(BaseLine::Style style)
{
    borderStyle = style;
}

BaseLine::Style Polygon::getBorderStyle() const
{
    return  borderStyle;
}

void Polygon::setBorderWidth(float width)
{
    borderWidth = width;
}

float Polygon::getBorderWidth() const
{
    return borderWidth;
}

void Polygon::draw(QPainter& painter)
{
    if (!edges) {
        return; // 如果没有边界数据，直接返回
    }
    QPen pen(Qt::black, 2);
    painter.setPen(pen);

    edges->draw(painter);

    if (getGeoDrawState() == GeoDrawState::Complete) {
        QBrush brush(Qt::blue); // 填充为蓝色
        painter.setBrush(brush);

        QPainterPath path;
        QVector<QPointF> points = edges->getPoints();
        if (!points.isEmpty()) {
            path.moveTo(points[0]);
            for (int i = 1; i < points.size(); ++i) {
                path.lineTo(points[i]);
            }
            path.closeSubpath(); // 闭合路径
            painter.drawPath(path);
        }
    }
}

void Polygon::mousePressEvent(QMouseEvent* event)
{
    if (getGeoDrawState() == Geo::GeoDrawState::Complete)
    {
        return;
    }

    if (event->button() == Qt::RightButton)
    {
        setGeoDrawState(Geo::GeoDrawState::Complete);

        if (!edges)
        {
            setIsInvalid(true); // 作废
            return;
        }
        QPointF* firstPoint = edges->points.begin();
        if (!firstPoint)
        {
            setIsInvalid(true); // 作废
            return;
        }
        edges->pushPoint(*firstPoint);
        edges->setGeoDrawState(Geo::GeoDrawState::Complete);
        setGeoDrawState(Geo::GeoDrawState::Complete);
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

