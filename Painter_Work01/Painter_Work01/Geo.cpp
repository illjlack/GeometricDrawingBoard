#include "Geo.h"
#include <QPainterPath>

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
Spline::Spline():geoSplineCurve(getSetting<int>(Key_SplineOrder), getSetting<int>(Key_SplineNodeCount))
{
    setGeoType(GeoType::TypeSpline);
    setStateSelected(); // ���ڻ����У��ǵ�ǰѡ��
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

    // ����������ʽ
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

void Spline::completeDrawing()
{
    // ����Ƿ�Ϸ�
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
    if (controlPoints.size() < 3)
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
    if (isStateInvalid())
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

    // ������������㣬����Բ�ĺͰ뾶
    if (controlPoints.size() == 3) {
        QPointF center;
        float radius;

        if (calculateCircle(controlPoints[0], controlPoints[1], controlPoints[2], center, radius)) {
            // ����Բ�ĺͰ뾶�ɹ�������Բ��


            QPainterPath path;
            // ������ʼ�ǶȺͽ����Ƕ�
            double startAngle = std::atan2(controlPoints[0].y() - center.y(), controlPoints[0].x() - center.x()) * 180.0 / M_PI;
            double endAngle = std::atan2(controlPoints[2].y() - center.y(), controlPoints[2].x() - center.x()) * 180.0 / M_PI;

            // ȷ���Ƕȴ�С�Ŀ�ʼ����
            double angleDiff = endAngle - startAngle;
            if (angleDiff < 0) {
                angleDiff += 360;  // �����Ƕȵ������ȷ����˳ʱ��
            }

            path.moveTo(controlPoints[0]);  // ����Բ������ʼ��

            // ���ƴ� startAngle �� endAngle ��Բ��
            QRectF rect(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius);
            path.arcTo(rect, startAngle, angleDiff);  // �� startAngle �� endAngle


            painter.drawPath(path);
        }
    }

    if (isStateDrawing() && controlPoints.size() == 2)
    {
        QPointF center;
        float radius;

        if (calculateCircle(controlPoints[0], controlPoints[1], tempControlPoint, center, radius)) {
            // ����Բ�ĺͰ뾶�ɹ�������Բ��
            QPainterPath path;

            // ������ʼ�ǶȺͽ����Ƕ�
            double startAngle = std::atan2(controlPoints[0].y() - center.y(), controlPoints[0].x() - center.x()) * 180.0 / M_PI;
            double endAngle = std::atan2(tempControlPoint.y() - center.y(), tempControlPoint.x() - center.x()) * 180.0 / M_PI;

            // ȷ���Ƕȴ�С�Ŀ�ʼ����
            double angleDiff = endAngle - startAngle;
            if (angleDiff < 0) {
                angleDiff += 360;  // �����Ƕȵ������ȷ����˳ʱ��
            }

            path.moveTo(controlPoints[0]);  // ����Բ������ʼ��

            // ���ƴ� startAngle �� endAngle ��Բ��
            QRectF rect(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius);
            path.arcTo(rect, startAngle, angleDiff);  // �� startAngle �� endAngle

            painter.drawPath(path);
        }
    }

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


// ===================================================================== Polygon
Polygon::Polygon()
{
    setGeoType(GeoType::TypePolygon);
    setStateSelected(); // ���ڻ����У��ǵ�ǰѡ��

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
    // ����Ƿ�Ϸ�
    if (edges->isStateInvalid()||edges->controlPoints.size() < 3)
    {
        setStateInvalid();
    }

    // ����ʼ��Ҳ����ͼ�Σ�����������(��,�յ������һ��Ҳ�����)
    // todo

    edges->completeDrawing();
    Geo::completeDrawing();
}

