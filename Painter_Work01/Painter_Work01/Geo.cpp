#include "Geo.h"
#include <QPainterPath>
#include <qDebug>

#define M_PI 3.14159265358979323846


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
    case DrawMode::DrawArcThreePoints:
        geo = new Arc3Points();
        break;
    case DrawMode::DrawArcTwoPoints:
        geo = new Arc2Points();
        break;
    case DrawMode::DrawComplexPolygon:
        geo = new ComplexPolygon();
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
Spline::Spline():geoSplineCurve(getSetting<int>(Key_SplineOrder), getSetting<int>(Key_SplineNodeCount))
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
            geoSplineCurve.addControlPoint(point);
        }
    }
}

QVector<QPointF> Spline::getPoints()
{
    return geoSplineCurve.getCurvePoints();
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

    const QVector<QPointF>& curvePoints = geoSplineCurve.getCurvePoints();

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

void Spline::completeDrawing()
{
    // 检查是否合法
    if (geoSplineCurve.getDegree() + 1 > geoSplineCurve.getNumControlPoints())
    {
        setStateInvalid();
    }
    BaseLine::completeDrawing();
}

// ===================================================================== Arc3Points

Arc3Points::Arc3Points()
{
    setGeoType(GeoType::TypeArcThreePoints);
    setStateSelected(); // 还在绘制中，是当前选中
}

Arc3Points::~Arc3Points()
{
}

void Arc3Points::mousePressEvent(QMouseEvent* event)
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
            if (controlPoints.size() == 3)
            {
                completeDrawing();
            }
        }
    }
}

void Arc3Points::completeDrawing()
{
    // 检查是否合法
    if (controlPoints.size() < 3 || controlPoints[0] == controlPoints[1] || 
        controlPoints[0] == controlPoints[2] || controlPoints[1] == controlPoints[2])
    {
        setStateInvalid();
    }
    BaseLine::completeDrawing();
}

QVector<QPointF> Arc3Points::getPoints()
{
    return QVector<QPointF>(controlPoints.begin(), controlPoints.end());
}

void Arc3Points::mouseMoveEvent(QMouseEvent* event)
{
    if (isStateDrawing())
    {
        tempControlPoint = event->pos();
    }
}

void Arc3Points::draw(QPainter& painter)
{
    if (isStateInvalid()) {
        return;
    }

    QPen pen;
    pen.setColor(color);
    pen.setWidthF(lineWidth);

    if (lineStyle == LineStyle::Dashed) {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(dashPattern);
    }
    else {
        pen.setStyle(Qt::SolidLine);
    }

    painter.setPen(pen);

    if (controlPoints.size() == 3) {
        drawArc(painter, controlPoints[0], controlPoints[1], controlPoints[2]);
    }

    if (isStateDrawing() && controlPoints.size() == 2) {
        drawArc(painter, controlPoints[0], controlPoints[1], tempControlPoint);
    }

    if (isStateSelected()) {
        for (auto& point : controlPoints) {
            point.draw(painter);
        }
        if (isStateDrawing()) {
            tempControlPoint.draw(painter);
        }
    }
}

void Arc3Points::drawArc(QPainter& painter, const QPointF& point1, const QPointF& point2, const QPointF& point3)
{
    QPointF center;
    double radius;

    if (calculateCircle(point1, point2, point3, center, radius)) {
        QPainterPath path;

        double startAngle = - std::atan2(point1.y() - center.y(), point1.x() - center.x()) * 180.0 / M_PI;
        double endAngle = - std::atan2(point3.y() - center.y(), point3.x() - center.x()) * 180.0 / M_PI;
        double middleAngle = -std::atan2(point2.y() - center.y(), point2.x() - center.x()) * 180.0 / M_PI;
 
        // 转换为[0,360]的坐标(都是逆时针方向)
        auto normalizeAngle = [](double angle) {
            while (angle < 0) {
                angle += 360;
            }
            while (angle >360) {
                angle -= 360;
            }
            return angle;
        };

        double angleDiffEnd = normalizeAngle(endAngle - startAngle);
        double angleDiffMid = normalizeAngle(middleAngle - startAngle);

        double angleDiff;
        if (angleDiffEnd > angleDiffMid) // 同方向且第二个点在中间
        {
            angleDiff = angleDiffEnd;
        }
        else // 从另一个方向经过第二个点
        {
            angleDiff = angleDiffEnd - 360;
        }
        
        QRectF rect(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius);

        path.moveTo(point1);

        path.arcTo(rect, startAngle, angleDiff);
        painter.drawPath(path);
    }
}

// ===================================================================== Arc2Points
Arc2Points::Arc2Points()
{
    setGeoType(GeoType::TypeArcTwoPoints);
    setStateSelected(); // 还在绘制中，是当前选中
}

Arc2Points::~Arc2Points()
{
}

void Arc2Points::mousePressEvent(QMouseEvent* event)
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
            if (controlPoints.size() == 2)
            {
                completeDrawing();
            }
        }
    }
}

void Arc2Points::completeDrawing()
{
    // 检查是否合法
    if (controlPoints.size() < 2 || controlPoints[0] == controlPoints[1])
    {
        setStateInvalid();
    }
    BaseLine::completeDrawing();
}

QVector<QPointF> Arc2Points::getPoints()
{
    return QVector<QPointF>(controlPoints.begin(), controlPoints.end());
}

void Arc2Points::mouseMoveEvent(QMouseEvent* event)
{
    if (isStateDrawing())
    {
        tempControlPoint = event->pos();
    }
}

void Arc2Points::draw(QPainter& painter)
{
    if (isStateInvalid()) {
        return;
    }

    QPen pen;
    pen.setColor(color);
    pen.setWidthF(lineWidth);

    if (lineStyle == LineStyle::Dashed) {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(dashPattern);
    }
    else {
        pen.setStyle(Qt::SolidLine);
    }

    painter.setPen(pen);

    // 如果已有两个点，绘制圆
    if (controlPoints.size() == 2) {
        drawCircle(painter, controlPoints[0], controlPoints[1]);
    }

    if (isStateDrawing() && controlPoints.size() == 1) {
        drawCircle(painter, controlPoints[0], tempControlPoint);
    }

    // 绘制控制点
    if (isStateSelected()) {
        for (auto& point : controlPoints) {
            point.draw(painter);
        }
        if (isStateDrawing()) {
            tempControlPoint.draw(painter);
        }
    }
}

void Arc2Points::drawCircle(QPainter& painter, const QPointF& point1, const QPointF& point2)
{
    QPointF center;
    double radius;

    // 计算圆心和半径
    if (calculateCircle(point1, point2, center, radius)) {
        QPainterPath path;

        double startAngle = -std::atan2(point1.y() - center.y(), point1.x() - center.x()) * 180.0 / M_PI;;
        QRectF rect(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius);

        path.moveTo(point1);  // 设置圆弧的起始点

        path.arcTo(rect, startAngle, 360);  // 绘制整个圆
        painter.drawPath(path);
    }
}

// ===================================================================== Polygon
Polygon::Polygon()
{
    setGeoType(GeoType::TypePolygon);
    setStateSelected(); // 还在绘制中，是当前选中

    edges = static_cast<BaseLine*>(createGeo(getSetting<DrawMode>(Key_PgLineMode)));
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

    QPen pen;
    pen.setColor(lineColor);
    pen.setWidthF(lineWidth);

    if (lineStyle == LineStyle::Dashed) {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(lineDashPattern);
    }
    else {
        pen.setStyle(Qt::SolidLine);
    }
    painter.setPen(pen);

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

    // 绘制控制点
    if (isStateSelected()) {
        for (auto& point : edges->controlPoints) 
        {
            point.draw(painter);
        }
        if (isStateDrawing()) 
        {
            edges->tempControlPoint.draw(painter);
        }
    }
}


void Polygon::mousePressEvent(QMouseEvent* event)
{
    if (isStateComplete())
    {
        return;
    }
    if (event->button() == Qt::RightButton)
    {
        edges->mousePressEvent(event);
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
    Geo::completeDrawing();
}

// ===================================================================== ComplexPolygon
ComplexPolygon::ComplexPolygon() {
    setGeoType(GeoType::TypeComplexPolygon);
    setStateSelected();  // 还在绘制中，是当前选中

    // 这里只为一个边界设置，后续可以支持多个边界的初始化
    BaseLine* edge = static_cast<BaseLine*>(createGeo(getSetting<DrawMode>(Key_PgLineMode)));
    edges.append(edge);
}

ComplexPolygon::~ComplexPolygon() {
    // 清理所有边界对象
    qDeleteAll(edges);
    edges.clear();
}

void ComplexPolygon::setFillColor(const QColor& color) {
    fillColor = color;
}

QColor ComplexPolygon::getFillColor() const {
    return fillColor;
}

void ComplexPolygon::setBorderColor(const QColor& color) {
    lineColor = color;
}

QColor ComplexPolygon::getBorderColor() const {
    return lineColor;
}

void ComplexPolygon::setBorderStyle(LineStyle style) {
    lineStyle = style;
}

LineStyle ComplexPolygon::getBorderStyle() const {
    return lineStyle;
}

void ComplexPolygon::setBorderWidth(float width) {
    lineWidth = width;
}

float ComplexPolygon::getBorderWidth() const {
    return lineWidth;
}

void ComplexPolygon::draw(QPainter& painter) {
    if (edges.isEmpty() || isStateInvalid()) {
        return;
    }

    QPen pen;
    pen.setColor(lineColor);
    pen.setWidthF(lineWidth);

    if (lineStyle == LineStyle::Dashed) {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(lineDashPattern);
    }
    else {
        pen.setStyle(Qt::SolidLine);
    }
    painter.setPen(pen);

    QBrush brush(fillColor);
    painter.setBrush(brush);
    QPainterPath path;

    // 绘制每一个边界
    for (int i = 0; i < edges.size(); ++i) {
        const QVector<QPointF>& points = edges[i]->getPoints();
        if (i == 0) {
            if (!points.isEmpty()) {
                path.moveTo(points[0]);
                for (int j = 1; j < points.size(); ++j) {
                    path.lineTo(points[j]);
                }
            }
            path.closeSubpath();
        }
        else {
            // 对于后续边界，使用负的绘制方式，表示“孔”
            if (!points.isEmpty()) {
                path.moveTo(points[0]);
                for (int j = 1; j < points.size(); ++j) {
                    path.lineTo(points[j]);
                }
                path.closeSubpath(); // 闭合路径
            }
        }
    }
    painter.drawPath(path);

    // 绘制控制点
    if (isStateSelected()) 
    {
        for (auto edge : edges)
        {
            for (auto& point : edge->controlPoints)
            {
                point.draw(painter);
            }
            if (edge->isStateDrawing())
            {
                edge->tempControlPoint.draw(painter);
            }
        }
    }
    painter.setBrush(Qt::NoBrush);  // 恢复无填充
}

void ComplexPolygon::mousePressEvent(QMouseEvent* event) {
    if (isStateComplete()) 
    {
        return;
    }
    if (event->button() == Qt::RightButton) 
    {
        if (edges.last()->isStateComplete())
        {
            completeDrawing();
        }
        else
        {
            edges.last()->mousePressEvent(event);
            if (edges.last()->isStateInvalid())
            {
                delete edges.last();
                edges.pop_back();
                if (edges.isEmpty())completeDrawing();
            }
        }
    }
    else {
        if (edges.last()->isStateComplete())
        {
            BaseLine* edge = static_cast<BaseLine*>(createGeo(getSetting<DrawMode>(Key_PgLineMode)));
            edges.append(edge);
        }
        edges.last()->mousePressEvent(event);
    }
}

void ComplexPolygon::mouseMoveEvent(QMouseEvent* event) {
    // 继续绘制时，更新当前正在绘制的边界
    edges.last()->mouseMoveEvent(event);
}

void ComplexPolygon::completeDrawing() {
    if (edges.isEmpty()) 
    {
        setStateInvalid();
    }
    Geo::completeDrawing();
}