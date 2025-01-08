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
    case DrawMode::DrawSimpleLine:
        geo = new SimpleLine();
        break;
    case DrawMode::DrawDoubleLine:
        geo = new DoubleLine();
        break;
    case DrawMode::DrawParallelLine:

        break;
    case DrawMode::DrawTwoPointCircle:

        break;
    case DrawMode::DrawSimpleArea:
        geo = new SimpleArea();
        break;
    case DrawMode::DrawComplexArea:
        geo = new ComplexArea();
        break;
    default:
        throw std::runtime_error("null DrawMode!!!!");
        break;
    }
    return geo;
}


// ================================================================================================ Geo
void Geo::drawControlPoints(QPainter& painter)
{
    if (isStateDrawing())controlPoints.push_back(tempControlPoints);
    for (auto& controlPoint : controlPoints)
    {
        // ���û��ʺͻ�ˢ
        painter.setPen(QPen(Qt::lightGray, 2));
        painter.setBrush(Qt::NoBrush);
        // ���ƿ��Ƶ�
        painter.drawRect(controlPoint.x() - 6, controlPoint.y() - 6, 12, 12);

        painter.setBrush(QBrush(Qt::red));
        painter.setPen(Qt::NoPen);
        painter.drawRect(controlPoint.x() - 5, controlPoint.y() - 5, 10, 10);

        painter.setBrush(Qt::NoBrush);
    }
    if (isStateDrawing())controlPoints.pop_back();
}

void Geo::hitTesting(QPointF point)
{
    // ���������ԣ��ж��Ƿ�����Ŀ������
    // ���磬�����ж�һ�����Ƿ��ڼ���ͼ���ڣ������Ƿ��ڿ��Ƶ㸽��
    for (auto& controlPoint : controlPoints)
    {
        if (QRectF(controlPoint.x() - 6, controlPoint.y() - 6, 12, 12).contains(point))
        {
            // ����ĳ�����Ƶ㣬������ӦһЩ����
            setStateSelected();
            break;
        }
    }
}


GeoType Geo::getGeoType()
{
    return geoType;
}
void Geo::setGeoType(GeoType newType)
{
    geoType = newType;
}

void Geo::completeDrawing()
{
    setStateComplete();
    setStateNotSelected(); // һ��ʼ��ѡ��״̬
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

bool Geo::isStateInitialized()
{
    return geoState & GeoStateInitialized;
}

void Geo::setStateInitialized()
{
    geoState |= GeoStateInitialized;
}

void Geo::setStateInvalid()
{

    geoState |= GeoStateInvalid;
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

// =======================================================================================================Point

Point::Point()
{
    setGeoType(GeoType::TypePoint);
    setStateSelected(); // ���ڻ����У��ǵ�ǰѡ��
}

void Point::initialize()
{
    color = getSetting<QRgb>(Key_PointColor);                // �����ɫ
    shape = getSetting<PointShape>(Key_PointShape);          // �����״
    setStateInitialized();
}

void Point::mousePressEvent(QMouseEvent* event)
{
    if (!isStateInitialized())initialize();
    if (isStateComplete())return;
    //--------------------------------------

    if (event->button() == Qt::RightButton)
    {
        completeDrawing();
    }

    if (event->button() == Qt::LeftButton)
    {
        controlPoints.push_back(event->pos());
        completeDrawing();
    }
}

void Point::mouseMoveEvent(QMouseEvent* event)
{
    if (!isStateComplete())
    {
        tempControlPoints = event->pos();
    }
}

void Point::completeDrawing()
{
    if (controlPoints.isEmpty())
    {
        setStateInvalid();
    }
    Geo::completeDrawing();
}

void Point::draw(QPainter& painter)
{
    if (!isStateInitialized() || isStateInvalid())
    {
        return;
    }
    QPointF point;
    if (isStateDrawing())
    {
        point = tempControlPoints;
    }
    else
    {
        point = controlPoints[0];
    }

    QPainterPath path;
    if (shape == PointShape::Square) {
        path.addRect(point.x() - 5, point.y() - 5, 10, 10);
    }
    else if (shape == PointShape::Circle) {
        path.addEllipse(point, 5.0, 5.0);
    }
    painter.setBrush(QBrush(color));
    painter.setPen(Qt::NoPen);
    painter.drawPath(path);
    if (isStateSelected())
    {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(Qt::lightGray, 2));

        if (shape == PointShape::Square) {
            painter.drawRect(point.x() - 6, point.y() - 6, 12, 12);
        }
        else if (shape == PointShape::Circle) {
            painter.drawEllipse(point, 6.0, 6.0);
        }
    }
    painter.setBrush(Qt::NoBrush);
    // �����ѡ��,�����Ƶ�
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }
}

// =======================================================================================================SimpleLine

SimpleLine::SimpleLine()
{
    setGeoType(GeoType::TypeSimpleLine);
    setStateSelected(); // ���ڻ����У��ǵ�ǰѡ��
}

void SimpleLine::initialize()
{
    lineWidth = getSetting<float>(Key_LineWidth);                   // �߿�
    dashPattern = getSetting<float>(Key_LineDashPattern);           // ���߶γ�
    color = getSetting<QRgb>(Key_LineColor);                        // ��ɫ
    lineStyle = getSetting<LineStyle>(Key_LineStyle);               // ��ʽ
    nodeLineStyle = getSetting<NodeLineStyle>(Key_NodeLineStyle);   // �ڵ�����
    setStateInitialized();
}

void SimpleLine::mousePressEvent(QMouseEvent* event)
{
    if (!isStateInitialized())initialize();
    if (isStateComplete())return;
    //--------------------------------------
    if (event->button() == Qt::RightButton)
    {
        completeDrawing();
    }
    if (event->button() == Qt::LeftButton)
    {
        controlPoints.push_back(event->pos());
    }
}

void SimpleLine::mouseMoveEvent(QMouseEvent* event)
{
    if (!isStateComplete())
    {
        tempControlPoints = event->pos();
    }
}

void SimpleLine::completeDrawing()
{
    if (!calculateLinePoints(nodeLineStyle, controlPoints, points))
    {
        setStateInvalid();
    }
    Geo::completeDrawing();
}

void SimpleLine::draw(QPainter& painter)
{
    if (!isStateInitialized() || isStateInvalid())
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

    if (isStateDrawing())controlPoints.push_back(tempControlPoints);
    if (calculateLinePoints(nodeLineStyle, controlPoints, points))
    {
        QPainterPath path;
        path.moveTo(points.first());
        for (int i = 1; i < points.size(); ++i)
        {
            path.lineTo(points[i]);
        }
        painter.drawPath(path);
    }
    if (isStateDrawing())controlPoints.pop_back();

    // �����ѡ��,�����Ƶ�
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }
}

// ===================================================================== SimpleArea
SimpleArea::SimpleArea()
{
    setGeoType(GeoType::TypeSimpleArea);
    setStateSelected(); // ���ڻ����У��ǵ�ǰѡ��
}

void SimpleArea::initialize()
{
    lineWidth = getSetting<float>(Key_PgLineWidth);                     // �߿���
    fillColor = getSetting<QRgb>(Key_PgFillColor);                      // ���������ɫ
    lineColor = getSetting<QRgb>(Key_PgLineColor);                      // �߿���ɫ
    lineStyle = getSetting<LineStyle>(Key_PgLineStyle);                 // �߿�����
    lineDashPattern = getSetting<float>(Key_PgLineDashPattern);         // ���߶γ�
    nodeLineStyle = getSetting<NodeLineStyle>(Key_PgNodeLineStyle);     // �ڵ�����
    setStateInitialized();
}

void SimpleArea::mousePressEvent(QMouseEvent* event)
{
    if (!isStateInitialized())initialize();
    if (isStateComplete())return;
    //--------------------------------------
    if (event->button() == Qt::RightButton)
    {
        completeDrawing();
    }
    if (event->button() == Qt::LeftButton)
    {
        controlPoints.push_back(event->pos());
    }
}

void SimpleArea::mouseMoveEvent(QMouseEvent* event)
{
    if (!isStateComplete())
    {
        tempControlPoints = event->pos();
    }
}

void SimpleArea::completeDrawing()
{
    if (!calculateCloseLinePoints(nodeLineStyle, controlPoints, points))
    {
        setStateInvalid();
    }
    Geo::completeDrawing();
}

void SimpleArea::draw(QPainter& painter)
{
    if (!isStateInitialized() || isStateInvalid())
    {
        return;
    }
    QPen pen;
    pen.setColor(lineColor);
    pen.setWidthF(lineWidth);
    if (lineStyle == LineStyle::Dashed)
    {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(lineDashPattern);
    }
    else
    {
        pen.setStyle(Qt::SolidLine);
    }
    painter.setPen(pen);

    QBrush brush(fillColor);
    painter.setBrush(brush);
    if (isStateDrawing())controlPoints.push_back(tempControlPoints);
    if (calculateCloseLinePoints(nodeLineStyle, controlPoints, points))
    {
        QPainterPath path;
        path.moveTo(points.first());
        for (int i = 1; i < points.size(); ++i)
        {
            path.lineTo(points[i]);
        }
        painter.drawPath(path);
    }
    painter.setBrush(Qt::NoBrush); // �ָ������   
    if (isStateDrawing())controlPoints.pop_back();

    // �����ѡ��,�����Ƶ�
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }
}

// ===================================================================== ComplexArea
ComplexArea::ComplexArea()
{
    setGeoType(GeoType::TypeComplexArea);
    setStateSelected(); // ���ڻ����У��ǵ�ǰѡ��
}

void ComplexArea::initialize()
{
    isDrawing = true;   // ��һ����ͼ׼������
    component.push_back({ 0,NodeLineStyle::NoStyle });      // �½���ͼ

    lineWidth = getSetting<float>(Key_PgLineWidth);                     // �߿���
    fillColor = getSetting<QRgb>(Key_PgFillColor);                      // ���������ɫ
    lineColor = getSetting<QRgb>(Key_PgLineColor);                      // �߿���ɫ
    lineStyle = getSetting<LineStyle>(Key_PgLineStyle);                 // �߿�����
    lineDashPattern = getSetting<float>(Key_PgLineDashPattern);         // ���߶γ�
    nodeLineStyle = getSetting<NodeLineStyle>(Key_PgNodeLineStyle);     // �ڵ�����
    setStateInitialized();
}

void ComplexArea::mousePressEvent(QMouseEvent* event)
{
    if (!isStateInitialized())initialize();
    if (isStateComplete())return;
    //--------------------------------------
    if (event->button() == Qt::RightButton)
    {
        if (isDrawing) // ���Ʒ�ͼ
        {
            isDrawing = false;
            // �½���ͼ
            component.push_back({0,NodeLineStyle::NoStyle});
        }
        else
        {
            completeDrawing();
        }
    }
    if (event->button() == Qt::LeftButton)
    {
        controlPoints.push_back(event->pos());
        component.last().len++;
        // ����ڵ����λ�û����
        if (component.last().nodeLineStyle == NodeLineStyle::NoStyle)
        {
            component.last().nodeLineStyle = nodeLineStyle;
        }
    }
}

void ComplexArea::mouseMoveEvent(QMouseEvent* event)
{
    if (!isStateComplete())
    {
        tempControlPoints = event->pos();
    }
}

void ComplexArea::completeDrawing()
{
    if (!calculateCloseLinePoints(component, controlPoints, pointss))
    {
        setStateInvalid();
    }
    Geo::completeDrawing();
}

void ComplexArea::draw(QPainter& painter)
{
    if (!isStateInitialized()||isStateInvalid())
    {
        return;
    }
    QPen pen;
    pen.setColor(lineColor);
    pen.setWidthF(lineWidth);
    if (lineStyle == LineStyle::Dashed)
    {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(lineDashPattern);
    }
    else
    {
        pen.setStyle(Qt::SolidLine);
    }
    painter.setPen(pen);

    QBrush brush(fillColor);
    painter.setBrush(brush);
    if (isDrawing)
    {
        controlPoints.push_back(tempControlPoints);
        component.last().len++;
    }
    if (calculateCloseLinePoints(component, controlPoints, pointss))
    {
        QPainterPath path;
        for (auto& points : pointss)
        {
            if (points.size() < 3)continue; // ��һ����ͼʧЧ
            path.moveTo(points.first());
            for (int i = 1; i < points.size(); ++i)
            {
                path.lineTo(points[i]);
            }
        }
        painter.drawPath(path);
    }
    painter.setBrush(Qt::NoBrush); // �ָ������   
    if (isDrawing)
    {
        controlPoints.pop_back();
        component.last().len--;
    }
    // �����ѡ��,�����Ƶ�
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }
}

// ===================================================================== DoubleLine
DoubleLine::DoubleLine()
{
    setGeoType(GeoType::TypeComplexArea);
    setStateSelected(); // ���ڻ����У��ǵ�ǰѡ��
}

void DoubleLine::initialize()
{
    isDrawing = true;   // ��һ����ͼ׼������
    component.push_back({ 0,NodeLineStyle::NoStyle });      // �½���ͼ

    lineWidth = getSetting<float>(Key_PgLineWidth);                     // ���
    lineColor = getSetting<QRgb>(Key_PgLineColor);                      // ��ɫ
    lineStyle = getSetting<LineStyle>(Key_PgLineStyle);                 // ����
    lineDashPattern = getSetting<float>(Key_PgLineDashPattern);         // �γ�
    nodeLineStyle = getSetting<NodeLineStyle>(Key_PgNodeLineStyle);     // �ڵ�����
    setStateInitialized();
}

void DoubleLine::mousePressEvent(QMouseEvent* event)
{
    if (!isStateInitialized())initialize();
    if (isStateComplete())return;
    //--------------------------------------
    if (event->button() == Qt::RightButton)
    {
        if (isDrawing && component.size() < 2) // ���Ʒ�ͼ,��������W
        {
            isDrawing = false;
            // �½���ͼ
            component.push_back({ 0,NodeLineStyle::NoStyle });
        }
        else
        {
            completeDrawing();
        }
    }
    if (event->button() == Qt::LeftButton)
    {
        controlPoints.push_back(event->pos());
        component.last().len++;
        // ����ڵ����λ�û����
        if (component.last().nodeLineStyle == NodeLineStyle::NoStyle)
        {
            component.last().nodeLineStyle = nodeLineStyle;
        }
    }
}

void DoubleLine::mouseMoveEvent(QMouseEvent* event)
{
    if (!isStateComplete())
    {
        tempControlPoints = event->pos();
    }
}

void DoubleLine::completeDrawing()
{
    if (calculateLinePoints(component, controlPoints, pointss)!= 2)
    {
        setStateInvalid();
    }
    Geo::completeDrawing();
}

void DoubleLine::draw(QPainter& painter)
{
    if (!isStateInitialized() || isStateInvalid())
    {
        return;
    }
    QPen pen;
    pen.setColor(lineColor);
    pen.setWidthF(lineWidth);
    if (lineStyle == LineStyle::Dashed)
    {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(lineDashPattern);
    }
    else
    {
        pen.setStyle(Qt::SolidLine);
    }
    painter.setPen(pen);
    if (isDrawing)
    {
        controlPoints.push_back(tempControlPoints);
        component.last().len++;
    }
    if (calculateLinePoints(component, controlPoints, pointss))
    {
        QPainterPath path;
        for (auto& points : pointss)
        {
            if (points.size() < 2)continue; // ��һ����ͼʧЧ
            path.moveTo(points.first());
            for (int i = 1; i < points.size(); ++i)
            {
                path.lineTo(points[i]);
            }
        }
        painter.drawPath(path);
    }
    if (isDrawing)
    {
        controlPoints.pop_back();
        component.last().len--;
    }
    // �����ѡ��,�����Ƶ�
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }
}

