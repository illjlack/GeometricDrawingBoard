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
    // ���û��ʺͻ�ˢ
    painter.setPen(QPen(Qt::lightGray, 2));
    painter.setBrush(Qt::NoBrush);

    // ���ƿ��Ƶ�
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
    setStateSelected(); // ���ڻ����У��ǵ�ǰѡ��
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
    // ����Ƿ�Ϸ�
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
    setStateSelected(); // ���ڻ����У��ǵ�ǰѡ��
}

Polyline::~Polyline()
{
}

// ���Ʒ���
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

    // �����ѡ��,�����Ƶ�
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
// ���캯��
Spline::Spline() {
    setGeoType(GeoType::TypeSpline);
    setStateSelected(); // ���õ�ǰΪѡ��״̬����ʾ���ڻ�����
}

// ��������
Spline::~Spline() {}

void Spline::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) // �Ҽ���ɻ���
    {
        completeDrawing();
    }
    else // �����ӿ��Ƶ�
    {
        if (isStateDrawing()) // �����ǰ���ڻ���״̬
        {
            QPointF point = event->pos();
            pushPoint(point); // �洢���Ƶ�
            controlPoints.push_back(point); // ������ӵ����Ƶ��б�

            // ���㲢���������ϵĵ�
            if (controlPoints.size() >= 2) // �����������Ƶ���ܼ�����
            {
                QVector<QPointF>points(controlPoints.begin(), controlPoints.end());
                calculateLinePoints(NodeLineStyle::StyleSpline, points,500, linePoints); // ʹ��������
            }
        }
    }
}

QVector<QPointF> Spline::getPoints()
{
    // ���ؼ���õ��������ϵĵ�
    return linePoints;
}

void Spline::draw(QPainter& painter)
{
    if (isStateInvalid()) // �����ǰ״̬��Ч��ֱ�ӷ���
    {
        return;
    }

    QPen pen;
    pen.setColor(color);
    pen.setWidthF(lineWidth);

    // ����������ʽ
    if (lineStyle == LineStyle::Dashed) {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(dashPattern);
    }
    else {
        pen.setStyle(Qt::SolidLine);
    }

    painter.setPen(pen);

    // �����������ߣ�linePoints�洢���Ǽ�����������ߵ㣩
    if (!linePoints.isEmpty()) {
        QPainterPath path;
        path.moveTo(linePoints[0]);

        // ���ƴӵ�һ���㵽���һ���������
        for (int i = 1; i < linePoints.size(); ++i) {
            path.lineTo(linePoints[i]);
        }

        painter.drawPath(path); // ����·��
    }

    // �����ǰ����ѡ�У����ƿ��Ƶ�
    if (isStateSelected())
    {
        for (auto& point : controlPoints)
        {
            point.draw(painter); // ����ÿ�����Ƶ�
        }

        // �����ǰ���ڻ��ƣ���갴�²�����˿��Ƶ㣩��������ʱ���Ƶ�
        if (isStateDrawing())
        {
            tempControlPoint.draw(painter);
        }
    }
}

void Spline::completeDrawing()
{
    // �����Ƶ������Ƿ��㹻��������Ҫ 2 �����Ƶ���ܼ�����������
    if (controlPoints.size() < 2)
    {
        setStateInvalid(); // ������Ƶ㲻�㣬����Ϊ��Ч״̬
    }
    BaseLine::completeDrawing(); // ��ɻ���
}
// ===================================================================== Arc3Points

Arc3Points::Arc3Points()
{
    setGeoType(GeoType::TypeArcThreePoints);
    setStateSelected(); // ���ڻ����У��ǵ�ǰѡ��
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
    // ����Ƿ�Ϸ�
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

    // �жϿ��Ƶ������Ƿ�Ϊ 3�����㻡���ϵĵ�
    if (controlPoints.size() == 3) {
        if (calculateArcPointsFromControlPoints(controlPoints[0], controlPoints[1], controlPoints[2],  100, arcPoints)) {
            // �������ɹ���ʹ�ü�����ĵ���ƻ���
            QPainterPath path;
            path.moveTo(arcPoints[0]);  // ���

            // ���������ƻ����ϵ�ÿ����
            for (const QPointF& point : arcPoints) {
                path.lineTo(point);
            }

            painter.drawPath(path);
        }
    }

    // ������ڻ���״̬�ҿ��Ƶ�����Ϊ 2��������ʱ����
    if (isStateDrawing() && controlPoints.size() == 2) {
        if (calculateArcPointsFromControlPoints(controlPoints[0], controlPoints[1], tempControlPoint, 100, arcPoints)) {
            // �������ɹ���ʹ�ü�����ĵ���ƻ���
            QPainterPath path;
            path.moveTo(arcPoints[0]);  // ���

            // ���������ƻ����ϵ�ÿ����
            for (const QPointF& point : arcPoints) {
                path.lineTo(point);
            }
            painter.drawPath(path);
        }
    }

    // ����ѡ�еĿ��Ƶ�
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
    setStateSelected(); // ���ڻ����У��ǵ�ǰѡ��
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
    // ����Ƿ�Ϸ�
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
    // �жϿ��Ƶ������Ƿ�Ϊ 3�����㻡���ϵĵ�
    if (controlPoints.size() == 2) {
        if (calculateArcPointsFromControlPoints(controlPoints[0], controlPoints[1], 100, arcPoints)) {
            // �������ɹ���ʹ�ü�����ĵ���ƻ���
            QPainterPath path;
            path.moveTo(arcPoints[0]);  // ���

            // ���������ƻ����ϵ�ÿ����
            for (const QPointF& point : arcPoints) {
                path.lineTo(point);
            }

            painter.drawPath(path);
        }
    }

    if (isStateDrawing() && controlPoints.size() == 1) {
        if (calculateArcPointsFromControlPoints(controlPoints[0], tempControlPoint, 100, arcPoints)) {
            // �������ɹ���ʹ�ü�����ĵ���ƻ���
            QPainterPath path;
            path.moveTo(arcPoints[0]);  // ���

            // ���������ƻ����ϵ�ÿ����
            for (const QPointF& point : arcPoints) {
                path.lineTo(point);
            }

            painter.drawPath(path);
        }
    }
    // ���ƿ��Ƶ�
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
    setStateSelected(); // ���ڻ����У��ǵ�ǰѡ��

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
        path.closeSubpath(); // �պ�·��
    }

    if (isStateDrawing() && points.size())
    {
        QPointF& tempPoint = edges->tempControlPoint;
        path.moveTo(points[0]);
        for (int i = 1; i < points.size(); ++i)
        {
            path.lineTo(points[i]);
        }
        path.lineTo(tempPoint); // ��ʱ��Ҳ����·��
        path.closeSubpath(); // �պ�·��
    }

    painter.drawPath(path);
    painter.setBrush(Qt::NoBrush); // �ָ������     

    // ���ƿ��Ƶ�
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
    // ����Ƿ�Ϸ�
    if (edges->isStateInvalid()||edges->controlPoints.size() < 3)
    {
        setStateInvalid();
    }
    Geo::completeDrawing();
}

// ===================================================================== ComplexPolygon
ComplexPolygon::ComplexPolygon() {
    setGeoType(GeoType::TypeComplexPolygon);
    setStateSelected();  // ���ڻ����У��ǵ�ǰѡ��

    // ����ֻΪһ���߽����ã���������֧�ֶ���߽�ĳ�ʼ��
    BaseLine* edge = static_cast<BaseLine*>(createGeo(getSetting<DrawMode>(Key_PgLineMode)));
    edges.append(edge);
}

ComplexPolygon::~ComplexPolygon() {
    // �������б߽����
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

    // ����ÿһ���߽�
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
            // ���ں����߽磬ʹ�ø��Ļ��Ʒ�ʽ����ʾ���ס�
            if (!points.isEmpty()) {
                path.moveTo(points[0]);
                for (int j = 1; j < points.size(); ++j) {
                    path.lineTo(points[j]);
                }
                path.closeSubpath(); // �պ�·��
            }
        }
    }
    painter.drawPath(path);

    // ���ƿ��Ƶ�
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
    painter.setBrush(Qt::NoBrush);  // �ָ������
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
    // ��������ʱ�����µ�ǰ���ڻ��Ƶı߽�
    edges.last()->mouseMoveEvent(event);
}

void ComplexPolygon::completeDrawing() {
    if (edges.isEmpty()) 
    {
        setStateInvalid();
    }
    Geo::completeDrawing();
}