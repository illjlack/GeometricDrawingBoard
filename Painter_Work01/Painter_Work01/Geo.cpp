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
// 构造函数
Spline::Spline() {
    setGeoType(GeoType::TypeSpline);
    setStateSelected(); // 设置当前为选中状态，表示正在绘制中
}

// 析构函数
Spline::~Spline() {}

void Spline::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) // 右键完成绘制
    {
        completeDrawing();
    }
    else // 左键添加控制点
    {
        if (isStateDrawing()) // 如果当前处于绘制状态
        {
            QPointF point = event->pos();
            pushPoint(point); // 存储控制点
            controlPoints.push_back(point); // 将点添加到控制点列表

            // 计算并更新曲线上的点
            if (controlPoints.size() >= 2) // 至少两个控制点才能计算线
            {
                QVector<QPointF>points(controlPoints.begin(), controlPoints.end());
                calculateLinePoints(NodeLineStyle::StyleSpline, points,500, linePoints); // 使用样条线
            }
        }
    }
}

QVector<QPointF> Spline::getPoints()
{
    // 返回计算得到的曲线上的点
    return linePoints;
}

void Spline::draw(QPainter& painter)
{
    if (isStateInvalid()) // 如果当前状态无效，直接返回
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

    // 绘制样条曲线（linePoints存储的是计算出来的曲线点）
    if (!linePoints.isEmpty()) {
        QPainterPath path;
        path.moveTo(linePoints[0]);

        // 绘制从第一个点到最后一个点的连线
        for (int i = 1; i < linePoints.size(); ++i) {
            path.lineTo(linePoints[i]);
        }

        painter.drawPath(path); // 绘制路径
    }

    // 如果当前对象被选中，绘制控制点
    if (isStateSelected())
    {
        for (auto& point : controlPoints)
        {
            point.draw(painter); // 绘制每个控制点
        }

        // 如果当前正在绘制（鼠标按下并添加了控制点），绘制临时控制点
        if (isStateDrawing())
        {
            tempControlPoint.draw(painter);
        }
    }
}

void Spline::completeDrawing()
{
    // 检查控制点数量是否足够，至少需要 2 个控制点才能计算样条曲线
    if (controlPoints.size() < 2)
    {
        setStateInvalid(); // 如果控制点不足，设置为无效状态
    }
    BaseLine::completeDrawing(); // 完成绘制
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

    QVector<QPointF> arcPoints;

    // 判断控制点数量是否为 3，计算弧线上的点
    if (controlPoints.size() == 3) {
        if (calculateArcPointsFromControlPoints(controlPoints[0], controlPoints[1], controlPoints[2],  100, arcPoints)) {
            // 如果计算成功，使用计算出的点绘制弧线
            QPainterPath path;
            path.moveTo(arcPoints[0]);  // 起点

            // 遍历并绘制弧线上的每个点
            for (const QPointF& point : arcPoints) {
                path.lineTo(point);
            }

            painter.drawPath(path);
        }
    }

    // 如果处于绘制状态且控制点数量为 2，绘制临时弧线
    if (isStateDrawing() && controlPoints.size() == 2) {
        if (calculateArcPointsFromControlPoints(controlPoints[0], controlPoints[1], tempControlPoint, 100, arcPoints)) {
            // 如果计算成功，使用计算出的点绘制弧线
            QPainterPath path;
            path.moveTo(arcPoints[0]);  // 起点

            // 遍历并绘制弧线上的每个点
            for (const QPointF& point : arcPoints) {
                path.lineTo(point);
            }
            painter.drawPath(path);
        }
    }

    // 绘制选中的控制点
    if (isStateSelected()) {
        for (auto& point : controlPoints) {
            point.draw(painter);
        }
        if (isStateDrawing()) {
            tempControlPoint.draw(painter);
        }
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

    QVector<QPointF> arcPoints;
    // 判断控制点数量是否为 3，计算弧线上的点
    if (controlPoints.size() == 2) {
        if (calculateArcPointsFromControlPoints(controlPoints[0], controlPoints[1], 100, arcPoints)) {
            // 如果计算成功，使用计算出的点绘制弧线
            QPainterPath path;
            path.moveTo(arcPoints[0]);  // 起点

            // 遍历并绘制弧线上的每个点
            for (const QPointF& point : arcPoints) {
                path.lineTo(point);
            }

            painter.drawPath(path);
        }
    }

    if (isStateDrawing() && controlPoints.size() == 1) {
        if (calculateArcPointsFromControlPoints(controlPoints[0], tempControlPoint, 100, arcPoints)) {
            // 如果计算成功，使用计算出的点绘制弧线
            QPainterPath path;
            path.moveTo(arcPoints[0]);  // 起点

            // 遍历并绘制弧线上的每个点
            for (const QPointF& point : arcPoints) {
                path.lineTo(point);
            }

            painter.drawPath(path);
        }
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