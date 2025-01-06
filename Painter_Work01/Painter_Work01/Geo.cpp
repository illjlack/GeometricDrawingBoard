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
    // 设置画笔颜色和无边框
    painter.setBrush(QBrush(color));
    painter.setPen(Qt::NoPen);

    // 创建一个 QPainterPath
    QPainterPath path;

    if (shape == PointShape::Square) {
        path.addRect(position.x() - 5, position.y() - 5, 10, 10);  // 绘制一个 10x10 的矩形
    }
    else if (shape == PointShape::Circle) {
        path.addEllipse(position, 5.0, 5.0);  // 绘制半径为 5 的圆
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
// 构造函数
BaseLine::BaseLine()
{
}

// 析构函数
BaseLine::~BaseLine() {}

// 设置点集
void BaseLine::setPoints(const QVector<QPointF>& points) {
    this->controlPoint = points;
}

// 获取点集
QVector<QPointF> BaseLine::getPoints() const {
    return controlPoint;
}

// 设置颜色
void BaseLine::setColor(QColor color) {
    this->color = color;
}

// 获取颜色
QColor BaseLine::getColor() const {
    return color;
}


void BaseLine::setLineStyle(LineStyle lineStyle)
{
    this->lineStyle = lineStyle;
}

// 获取线型
LineStyle BaseLine::getLineStyle() const {
    return lineStyle;
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
    controlPoint.push_back(point);
}

// =======================================================================================================Polyline
Polyline::Polyline()
{
}
Polyline::~Polyline()
{
}

// 绘制方法
void Polyline::draw(QPainter& painter) {
    // 设置画笔
    QPen pen;
    pen.setColor(color);
    pen.setWidthF(lineWidth);

    // 设置虚线样式
    if (lineStyle == LineStyle::Dashed) {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(dashPattern);  // 设置虚线段的长度
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
    // 设置画笔
    QPen pen;
    pen.setColor(color);
    pen.setWidthF(lineWidth);

    // 设置虚线样式
    if (lineStyle == LineStyle::Dashed) {
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

        painter.setBrush(Qt::NoBrush); //恢复无填充
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
            setIsInvalid(true); // 作废
            return;
        }
        QPointF* firstPoint = edges->controlPoint.begin();
        if (!firstPoint)
        {
            setIsInvalid(true); // 作废
            return;
        }
        // 直接path.closeSubpath();闭合路径
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

