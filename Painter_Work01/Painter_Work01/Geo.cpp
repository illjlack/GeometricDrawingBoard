
#include <QPainterPath>
#include <qDebug>
#include <stdexcept>
#include "Geo.h"
#include "comm.h"

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
        geo = new ParallelLine();
        break;
    case DrawMode::DrawTwoPointCircle:
        geo = new TwoPointCircle();
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

// ===================================================================== Geo
void Geo::initialize()
{
    geoParameters.initGeoParameters();
    setStateInitialized();
}

void Geo::drawControlPoints(QPainter& painter)
{
    if (isStateDrawing() && !tempControlPoints.isNull())controlPoints.push_back(tempControlPoints);
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
    if (isStateDrawing() && !tempControlPoints.isNull())controlPoints.pop_back();
}

void Geo::drawBuffer(QPainter& painter)
{

}

bool Geo::hitTesting(const QPointF& point)
{
    hitPoint = point;
    // �����жϿ��Ƶ��ĵ������
    // ���������ԣ��ж��Ƿ�����Ŀ������
    bool selected = false;
    for (auto& controlPoint : controlPoints)
    {
        if (QRectF(controlPoint.x() - 6, controlPoint.y() - 6, 12, 12).contains(point))
        {
            setStateSelected();
            selected = true;

            currentSelectedPoint = &controlPoint;
            break;
        }
    }
    if (!selected)currentSelectedPoint = nullptr;
    return selected;
}

void Geo::dragGeo(const QPointF& point)
{
    // ���ѡ����ǿ��Ƶ�,���޸Ŀ��Ƶ�
    if (currentSelectedPoint)
    {
        *currentSelectedPoint = point;
        markControlPointsChanged();
    }
    else // ��Ȼ�ƶ�ͼ
    {
        QPointF dis = point - hitPoint;
        for (auto& controlPoint : controlPoints)
        {
            controlPoint += dis;
        }
        markControlPointsChanged();
    }
    hitPoint = point;
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

bool Geo::isMouseLeftButtonPressed()
{
    return mouseLeftButtonPressed;
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

void Geo::markControlPointsChanged() {
    controlPointsChanged = true;
    markBufferChanged();
}

bool Geo::isControlPointsChanged() const {
    return controlPointsChanged;
}

void Geo::resetControlPointsChanged() 
{
    controlPointsChanged = false;
}

void Geo::markBufferChanged()
{
    bufferChanged = true;
}
bool Geo::isBufferChanged() const 
{
    return bufferChanged;
}
void Geo::resetBufferChanged()
{
    bufferChanged = false;
}

// ��ȡ��ǰ�� GeoParameters
GeoParameters Geo::getGeoParameters()
{
    return geoParameters; // ���ص�ǰ�ṹ��
}

// ���� GeoParameters
void  Geo::setGeoParameters(const GeoParameters& params)
{
    if (geoParameters.bufferDistance != params.bufferDistance || 
        geoParameters.bufferCalculationMode!= params.bufferCalculationMode)markBufferChanged();

    geoParameters = params;
}

void Geo::keyPressEvent(QKeyEvent* event)
{
}

void Geo::keyReleaseEvent(QKeyEvent* event)
{
}

// ���ѡ��
void Geo::mouseMoveEvent(QMouseEvent* event)
{
}

void Geo::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        mouseLeftButtonPressed = true;
    }
}

void Geo::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        mouseLeftButtonPressed = false;
    }
}

void Geo::wheelEvent(QWheelEvent* event)
{
}

void Geo::updateTempPoint(const QPoint& pos)
{
    tempControlPoints = pos;
}

void Geo::endSegmentDrawing()
{
    completeDrawing();
}

void Geo::pushControlPoint(const QPoint& pos)
{
    markControlPointsChanged();
    controlPoints.push_back(pos);
}

// ===================================================================== Point

Point::Point()
{
    setGeoType(GeoType::TypePoint);
    setStateSelected(); // ���ڻ����У��ǵ�ǰѡ��
}


void Point::mousePressEvent(QMouseEvent* event)
{
    if (!isStateInitialized()) initialize();
    if (isStateComplete()) return;

    if (event->button() == Qt::RightButton)
    {
        endSegmentDrawing();
    }
    else if (event->button() == Qt::LeftButton)
    {
        pushControlPoint(event->pos());
        endSegmentDrawing();
    }

    Geo::mousePressEvent(event);
}

void Point::mouseMoveEvent(QMouseEvent* event)
{
    if (!isStateComplete())
    {
        updateTempPoint(event->pos());
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

    // ���״̬����һ������ɼ������ƻ�����
    if (isStateComplete() && geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }

    // �����ѡ�У����ƿ��Ƶ�
    if (isStateSelected())
    {
        drawControlPoints(painter);
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
    if (geoParameters.pointShape == PointShape::Square) {
        path.addRect(point.x() - 5, point.y() - 5, 10, 10);
    }
    else if (geoParameters.pointShape == PointShape::Circle) {
        path.addEllipse(point, 5.0, 5.0);
    }
    painter.setBrush(QBrush(geoParameters.pointColor));
    painter.setPen(Qt::NoPen);
    painter.drawPath(path);
    if (isStateSelected())
    {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(Qt::lightGray, 2));

        if (geoParameters.pointShape == PointShape::Square) {
            painter.drawRect(point.x() - 6, point.y() - 6, 12, 12);
        }
        else if (geoParameters.pointShape == PointShape::Circle) {
            painter.drawEllipse(point, 6.0, 6.0);
        }
    }
    painter.setBrush(Qt::NoBrush);
}

void Point::drawBuffer(QPainter& painter)
{
    if (!geoParameters.bufferVisible)
    {
        return; // ������������ɼ���ֱ�ӷ���
    }

    // ���û������߿�
    if (!geoParameters.bufferHasBorder)
    {
        painter.setPen(Qt::NoPen);
    }
    else
    {
        QPen bufferPen;
        bufferPen.setColor(geoParameters.bufferLineColor);
        bufferPen.setWidthF(geoParameters.bufferLineWidth);
        if (geoParameters.bufferLineStyle == LineStyle::Dashed)
        {
            bufferPen.setStyle(Qt::DashLine);
            bufferPen.setDashOffset(geoParameters.bufferLineDashPattern);
        }
        else
        {
            bufferPen.setStyle(Qt::SolidLine);
        }
        painter.setPen(bufferPen);
    }

    QBrush bufferBrush(QColor(geoParameters.bufferFillColor));
    painter.setBrush(bufferBrush);

    // ����������仯��·��Ϊ�գ����¼��㻺����·��
    if (isBufferChanged())
    {
        resetBufferChanged(); // ���û�����״̬
        bufferPath = QPainterPath(); // ��ղ���������·��

        QVector<QVector<QPointF>>pointss = { controlPoints };
        if (computeBufferBoundary( geoParameters.bufferCalculationMode,pointss, geoParameters.bufferDistance, buffers))
        {
            for (auto& points : buffers)
            {
                if (points.size() < 2) continue; // ������Ч��ͼ
                bufferPath.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    bufferPath.lineTo(points[i]);
                }
            }
        }
    }

    // ���ƻ�����·��
    painter.drawPath(bufferPath);

    // �ָ������״̬
    painter.setBrush(Qt::NoBrush);
}
// ===================================================================== SimpleLine

SimpleLine::SimpleLine()
{
    setGeoType(GeoType::TypeSimpleLine);
    setStateSelected(); // ���ڻ����У��ǵ�ǰѡ��
}

void SimpleLine::mousePressEvent(QMouseEvent* event)
{
    if (!isStateInitialized()) initialize();
    if (isStateComplete()) return;

    if (event->button() == Qt::RightButton)
    {
        endSegmentDrawing();
    }
    else if (event->button() == Qt::LeftButton)
    {
        pushControlPoint(event->pos());
    }

    Geo::mousePressEvent(event);
}

void SimpleLine::mouseMoveEvent(QMouseEvent* event)
{
    if (isStateComplete()) return;

    updateTempPoint(event->pos());

    if (geoParameters.nodeLineStyle == NodeLineStyle::StyleStreamline && isMouseLeftButtonPressed())
    {
        pushControlPoint(event->pos());
    }
}

void SimpleLine::mouseReleaseEvent(QMouseEvent* event)
{
    // ��������߽����λ���
    if (!isStateComplete() && geoParameters.nodeLineStyle == NodeLineStyle::StyleStreamline && isMouseLeftButtonPressed())
    {
        endSegmentDrawing();
    }
    Geo::mouseReleaseEvent(event);
}

void SimpleLine::completeDrawing()
{
    if (!calculateLinePoints(geoParameters.nodeLineStyle, controlPoints, points))
    {
        setStateInvalid();
    }
    Geo::completeDrawing();
}

bool SimpleLine::hitTesting(const QPointF& point)
{
    bool selected = false;

    const double epsilon = 5.0;

    // �������ߵ������߶�
    for (int i = 0; i < points.size() - 1; ++i) 
    {
        QPointF& p1 = points[i];     // �߶����
        QPointF& p2 = points[i + 1]; // �߶��յ�

        double distance = pointToSegmentDistance(point, p1, p2);
        if (distance <= epsilon) 
        {
            selected = true;
            break;
        }
    }

    return Geo::hitTesting(point) || selected;
}


void SimpleLine::draw(QPainter& painter)
{
    if (!isStateInitialized() || isStateInvalid())
    {
        return;
    }

    // ���״̬����һ������ɼ������ƻ�����
    if (isStateComplete() && geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }

    // �����ѡ�У����ƿ��Ƶ�
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }

    // ���û���
    QPen pen;
    pen.setColor(geoParameters.lineColor);
    pen.setWidthF(geoParameters.lineWidth);
    if (geoParameters.lineStyle == LineStyle::Dashed)
    {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(geoParameters.lineDashPattern);
    }
    else
    {
        pen.setStyle(Qt::SolidLine);
    }
    painter.setPen(pen);

    // ��̬�����ʱ���Ƶ�
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        markControlPointsChanged(); // ��ǿ��Ƶ㷢���˱仯
    }

    // ������Ƶ㷢���仯��·��Ϊ�գ������¼���·��
    if (isControlPointsChanged())
    {
        resetControlPointsChanged(); // ���ÿ��Ƶ�״̬
        path = QPainterPath();       // ��ղ���������·��

        if (calculateLinePoints(geoParameters.nodeLineStyle, controlPoints, points))
        {
            if (!points.isEmpty())
            {
                path.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    path.lineTo(points[i]);
                }
            }
        }
    }

    // ����·��
    painter.drawPath(path);

    // �ָ���̬���Ƶ�
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.pop_back();
        markControlPointsChanged();
    }


}

void SimpleLine::drawBuffer(QPainter& painter)
{
    if (!geoParameters.bufferVisible)
    {
        return; // ������������ɼ���ֱ�ӷ���
    }

    // ���û������߿�
    if (!geoParameters.bufferHasBorder)
    {
        painter.setPen(Qt::NoPen);
    }
    else
    {
        QPen bufferPen;
        bufferPen.setColor(geoParameters.bufferLineColor);
        bufferPen.setWidthF(geoParameters.bufferLineWidth);
        if (geoParameters.bufferLineStyle == LineStyle::Dashed)
        {
            bufferPen.setStyle(Qt::DashLine);
            bufferPen.setDashOffset(geoParameters.bufferLineDashPattern);
        }
        else
        {
            bufferPen.setStyle(Qt::SolidLine);
        }
        painter.setPen(bufferPen);
    }

    QBrush bufferBrush(QColor(geoParameters.bufferFillColor));
    painter.setBrush(bufferBrush);

    // ����������仯��·��Ϊ�գ����¼��㻺����·��
    if (isBufferChanged() || bufferPath.isEmpty())
    {
        resetBufferChanged(); // ���û�����״̬
        bufferPath = QPainterPath(); // ��ղ���������·��

        QVector<QVector<QPointF>>pointss = { points };
        if (computeBufferBoundary( geoParameters.bufferCalculationMode,pointss, geoParameters.bufferDistance, buffers))
        {
            for (auto& points : buffers)
            {
                if (points.size() < 2) continue; // ������Ч��ͼ
                bufferPath.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    bufferPath.lineTo(points[i]);
                }
            }
        }
    }

    // ���ƻ�����·��
    painter.drawPath(bufferPath);

    // �ָ������״̬
    painter.setBrush(Qt::NoBrush);
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
    Geo::initialize();
}

void DoubleLine::mousePressEvent(QMouseEvent* event)
{
    if (!isStateInitialized()) initialize();
    if (isStateComplete()) return;

    if (event->button() == Qt::RightButton)
    {
        endSegmentDrawing();
    }
    else if (event->button() == Qt::LeftButton)
    {
        pushControlPoint(event->pos());
    }

    Geo::mousePressEvent(event);
}

void DoubleLine::mouseMoveEvent(QMouseEvent* event)
{
    if (!isStateComplete())
    {
        updateTempPoint(event->pos());

        // ���������,�����ƶ���ʱ��ͻ���
        if (isDrawing && component.last().nodeLineStyle == NodeLineStyle::StyleStreamline && isMouseLeftButtonPressed())
        {
            pushControlPoint(event->pos());
        }
    }
}

void DoubleLine::mouseReleaseEvent(QMouseEvent* event)
{
    // ��������߽����λ���
    if (!isStateComplete() && component.last().nodeLineStyle == NodeLineStyle::StyleStreamline && isMouseLeftButtonPressed())
    {
        endSegmentDrawing();
    }
    Geo::mouseReleaseEvent(event);
}

void DoubleLine::pushControlPoint(const QPoint& pos)
{
    markControlPointsChanged();

    isDrawing = true;
    controlPoints.push_back(pos);
    component.last().len++;

    if (component.last().nodeLineStyle == NodeLineStyle::NoStyle)
    {
        component.last().nodeLineStyle = geoParameters.nodeLineStyle;
    }
}

void DoubleLine::endSegmentDrawing()
{
    if (isDrawing && component.size() < 2)
    {
        component.push_back({ 0, NodeLineStyle::NoStyle });
    }
    else
    {
        completeDrawing();
    }
    isDrawing = false;
}

bool DoubleLine::hitTesting(const QPointF& point)
{
    bool selected = false;

    const double epsilon = 5.0;

    for (auto& points : pointss)
    {
        // �������ߵ������߶�
        for (int i = 0; i < points.size() - 1; ++i) {
            QPointF& p1 = points[i];     // �߶����
            QPointF& p2 = points[i + 1]; // �߶��յ�

            double distance = pointToSegmentDistance(point, p1, p2);
            if (distance <= epsilon) {
                selected = true;
                break;
            }
        }
        if (selected)break;
    }
    return Geo::hitTesting(point) || selected;
}

void DoubleLine::completeDrawing()
{
    if (calculateLinePoints(component, controlPoints, pointss) != 2)
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


    // ���״̬����һ������ɼ������ƻ�����
    if (isStateComplete() && geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }

    // �����ѡ�У����ƿ��Ƶ�
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }


    // ���û���
    QPen pen;
    pen.setColor(geoParameters.lineColor);
    pen.setWidthF(geoParameters.lineWidth);
    if (geoParameters.lineStyle == LineStyle::Dashed)
    {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(geoParameters.lineDashPattern);
    }
    else
    {
        pen.setStyle(Qt::SolidLine);
    }
    painter.setPen(pen);

    // ��̬�����ʱ���Ƶ�
    if (isDrawing && isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        component.last().len++;
        markControlPointsChanged(); // ��ǿ��Ƶ㷢���˱仯
    }

    // ������Ƶ㷢���仯�����¼���·��
    if (isControlPointsChanged())
    {
        resetControlPointsChanged(); // ���ÿ��Ƶ�״̬
        path = QPainterPath();       // ��ղ���������·��

        if (calculateLinePoints(component, controlPoints, pointss))
        {
            for (const auto& points : pointss)
            {
                if (points.size() < 2) continue; // ������Ч��ͼ

                path.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    path.lineTo(points[i]);
                }
            }
        }
    }

    // ����·��
    painter.drawPath(path);

    // �ָ���̬���Ƶ�
    if (isDrawing && isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.pop_back();
        component.last().len--;
        markControlPointsChanged();
    }
}


void DoubleLine::drawBuffer(QPainter& painter)
{
    if (!geoParameters.bufferVisible)
    {
        return; // ������������ɼ���ֱ�ӷ���
    }

    // ���û������߿�
    if (!geoParameters.bufferHasBorder)
    {
        painter.setPen(Qt::NoPen);
    }
    else
    {
        QPen bufferPen;
        bufferPen.setColor(geoParameters.bufferLineColor);
        bufferPen.setWidthF(geoParameters.bufferLineWidth);
        if (geoParameters.bufferLineStyle == LineStyle::Dashed)
        {
            bufferPen.setStyle(Qt::DashLine);
            bufferPen.setDashOffset(geoParameters.bufferLineDashPattern);
        }
        else
        {
            bufferPen.setStyle(Qt::SolidLine);
        }
        painter.setPen(bufferPen);
    }

    // ���û��������
    QBrush bufferBrush(QColor(geoParameters.bufferFillColor));
    painter.setBrush(bufferBrush);

    // ����������仯��·��Ϊ�գ����¼��㻺����·��
    if (isBufferChanged())
    {
        resetBufferChanged(); // ���û�����״̬
        bufferPath = QPainterPath(); // ��ղ���������·��

        if (computeBufferBoundary( geoParameters.bufferCalculationMode,pointss, geoParameters.bufferDistance, buffers))
        {
            for (auto& points : buffers)
            {
                if (points.size() < 2) continue; // ������Ч��ͼ
                bufferPath.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    bufferPath.lineTo(points[i]);
                }
            }
        }
    }

    // ���ƻ�����·��
    painter.drawPath(bufferPath);

    // �ָ������״̬
    painter.setBrush(Qt::NoBrush);
}




// ===================================================================== ParallelLine
ParallelLine::ParallelLine()
{
    setGeoType(GeoType::TypeComplexArea);
    setStateSelected(); // ���ڻ����У��ǵ�ǰѡ��
}

void ParallelLine::initialize()
{
    component.push_back({ 0,NodeLineStyle::NoStyle });      // �½���ͼ

    Geo::initialize();
}

void ParallelLine::mousePressEvent(QMouseEvent* event)
{
    if (!isStateInitialized()) initialize();
    if (isStateComplete()) return;

    if (event->button() == Qt::RightButton)
    {
        endSegmentDrawing();
    }

    if (event->button() == Qt::LeftButton)
    {
        pushControlPoint(event->pos());
    }

    Geo::mousePressEvent(event);
}

void ParallelLine::mouseMoveEvent(QMouseEvent* event)
{
    if (!isStateComplete())
    {
        updateTempPoint(event->pos());
    }

    // ���������, �����ƶ���ʱ��ͻ���
    if (!isStateComplete() && component.last().nodeLineStyle == NodeLineStyle::StyleStreamline && isMouseLeftButtonPressed())
    {
        pushControlPoint(event->pos());
    }
}

void ParallelLine::mouseReleaseEvent(QMouseEvent* event)
{
    // ��������߽����λ���
    if (!isStateComplete() && component.last().nodeLineStyle == NodeLineStyle::StyleStreamline && isMouseLeftButtonPressed())
    {
        endSegmentDrawing();
    }
    Geo::mouseReleaseEvent(event);
}

void ParallelLine::endSegmentDrawing()
{
    if (component.size() < 2) // ���Ʒ�ͼ,��������W
    {
        component.push_back({ 0,NodeLineStyle::NoStyle });
    }
    else
    {
        completeDrawing();
    }
}

void ParallelLine::pushControlPoint(const QPoint& pos)
{
    markControlPointsChanged();

    if (component.size() == 1)
    {
        controlPoints.push_back(pos);
        component.last().len++;
        // ����ڵ����λ�û����
        if (component.last().nodeLineStyle == NodeLineStyle::NoStyle)
        {
            component.last().nodeLineStyle = geoParameters.nodeLineStyle;
        }
    }
    else if (component.size() == 2)
    {
        controlPoints.push_back(pos);
        component.last().len++;
        endSegmentDrawing(); // �ڶ���������һ�����Ƶ�
    }
}

void ParallelLine::completeDrawing()
{
    if (calculateParallelLinePoints(component, controlPoints, pointss) != 2)
    {
        setStateInvalid();
    }
    Geo::completeDrawing();
}

bool ParallelLine::hitTesting(const QPointF& point)
{
    bool selected = false;

    const double epsilon = 5.0;

    for (auto& points : pointss)
    {
        // �������ߵ������߶�
        for (int i = 0; i < points.size() - 1; ++i) {
            QPointF& p1 = points[i];     // �߶����
            QPointF& p2 = points[i + 1]; // �߶��յ�

            double distance = pointToSegmentDistance(point, p1, p2);
            if (distance <= epsilon) {
                selected = true;
                break;
            }
        }
        if (selected)break;
    }
    return Geo::hitTesting(point) || selected;
}

void ParallelLine::draw(QPainter& painter)
{
    if (!isStateInitialized() || isStateInvalid())
    {
        return;
    }

    // ���״̬����һ������ɼ������ƻ�����
    if (isStateComplete() && geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }

    // �����ѡ�У����ƿ��Ƶ�
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }

    // ���û���
    QPen pen;
    pen.setColor(geoParameters.lineColor);
    pen.setWidthF(geoParameters.lineWidth);
    if (geoParameters.lineStyle == LineStyle::Dashed)
    {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(geoParameters.lineDashPattern);
    }
    else
    {
        pen.setStyle(Qt::SolidLine);
    }
    painter.setPen(pen);

    // ��̬�����ʱ���Ƶ�
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        component.last().len++;
        markControlPointsChanged(); // ��ǿ��Ƶ㷢���˱仯
    }

    // ������Ƶ㷢���仯�����¼���·��
    if (isControlPointsChanged())
    {
        resetControlPointsChanged(); // ���ÿ��Ƶ�״̬
        path = QPainterPath();       // ��ղ���������·��

        if (calculateParallelLinePoints(component, controlPoints, pointss))
        {
            for (const auto& points : pointss)
            {
                if (points.size() < 2) continue; // ������Ч��ͼ

                path.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    path.lineTo(points[i]);
                }
            }
        }
    }

    // ����·��
    painter.drawPath(path);

    // �ָ���̬���Ƶ�
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.pop_back();
        component.last().len--;
        markControlPointsChanged();
    }

}

void ParallelLine::drawBuffer(QPainter& painter)
{
    if (!geoParameters.bufferVisible)
    {
        return; // ������������ɼ���ֱ�ӷ���
    }

    // ���û������߿�
    if (!geoParameters.bufferHasBorder)
    {
        painter.setPen(Qt::NoPen);
    }
    else
    {
        QPen bufferPen;
        bufferPen.setColor(geoParameters.bufferLineColor);
        bufferPen.setWidthF(geoParameters.bufferLineWidth);
        if (geoParameters.bufferLineStyle == LineStyle::Dashed)
        {
            bufferPen.setStyle(Qt::DashLine);
            bufferPen.setDashOffset(geoParameters.bufferLineDashPattern);
        }
        else
        {
            bufferPen.setStyle(Qt::SolidLine);
        }
        painter.setPen(bufferPen);
    }

    // ���û��������
    QBrush bufferBrush(QColor(geoParameters.bufferFillColor));
    painter.setBrush(bufferBrush);

    // ����������仯��·��Ϊ�գ����¼��㻺����·��
    if (isBufferChanged())
    {
        resetBufferChanged(); // ���û�����״̬
        bufferPath = QPainterPath(); // ��ղ���������·��

        if (computeBufferBoundary( geoParameters.bufferCalculationMode,pointss, geoParameters.bufferDistance, buffers))
        {
            for (auto& points : buffers)
            {
                if (points.size() < 2) continue; // ������Ч��ͼ
                bufferPath.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    bufferPath.lineTo(points[i]);
                }
            }
        }
    }
    // ���ƻ�����·��
    painter.drawPath(bufferPath);
    // �ָ������״̬
    painter.setBrush(Qt::NoBrush);
}


// ===================================================================== TwoPointCircle
TwoPointCircle::TwoPointCircle()
{
    setGeoType(GeoType::TypeSimpleArea);
    setStateSelected(); // ���ڻ����У��ǵ�ǰѡ��
}


void TwoPointCircle::mousePressEvent(QMouseEvent* event)
{
    if (!isStateInitialized())initialize();
    if (isStateComplete())return;
    //--------------------------------------
    if (event->button() == Qt::RightButton)
    {
        endSegmentDrawing();
    }
    if (event->button() == Qt::LeftButton)
    {
        pushControlPoint(event->pos());
    }

    Geo::mousePressEvent(event);
}

void TwoPointCircle::mouseMoveEvent(QMouseEvent* event)
{
    if (!isStateComplete())
    {
        tempControlPoints = event->pos();
    }
}

void TwoPointCircle::pushControlPoint(const QPoint& pos)
{
    markControlPointsChanged();

    controlPoints.push_back(pos);
    if (controlPoints.size() == 2)
    {
        endSegmentDrawing();
    }
}

bool TwoPointCircle::hitTesting(const QPointF& point)
{   
    return Geo::hitTesting(point) || path.contains(point);
}

void TwoPointCircle::completeDrawing()
{
    if (!calculateCloseLinePoints(NodeLineStyle::StyleTwoPointCircle, controlPoints, points))
    {
        setStateInvalid();
    }
    Geo::completeDrawing();
}

void TwoPointCircle::draw(QPainter& painter)
{
    if (!isStateInitialized() || isStateInvalid())
    {
        return;
    }

    // ������ڻ��Ʋ�������ʱ���Ƶ㣬�������Ƶ�
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        markControlPointsChanged(); // ��ǿ��Ƶ��Ѹ���
    }
    // ������Ƶ㷢���ı䣬���¼���·��
    if (isControlPointsChanged())
    {
        resetControlPointsChanged(); // ���ÿ��Ƶ���ı�־

        // ���¼���·��
        path = QPainterPath();
        if (!controlPoints.isEmpty())
        {
            if (calculateCloseLinePoints(NodeLineStyle::StyleTwoPointCircle, controlPoints, points))
            {
                if (!points.isEmpty())
                {
                    path.moveTo(points.first());
                    for (int i = 1; i < points.size(); ++i)
                    {
                        path.lineTo(points[i]);
                    }
                }
            }
        }
    }
    // ������ڻ��Ʋ�������ʱ���Ƶ㣬���Ƴ���ʱ��
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.pop_back();
        markControlPointsChanged(); // ��ǿ��Ƶ��Ѹ���
    }

    // ���״̬����һ������ɼ������ƻ�����
    if (isStateComplete() && geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }

    // �����ѡ�У����ƿ��Ƶ�
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }

    // ���û�����ʽ
    QPen pen;
    pen.setColor(geoParameters.lineColor);
    pen.setWidthF(geoParameters.lineWidth);
    if (geoParameters.lineStyle == LineStyle::Dashed)
    {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(geoParameters.lineDashPattern);
    }
    else
    {
        pen.setStyle(Qt::SolidLine);
    }
    painter.setPen(pen);

    // ���û�ˢ��ʽ
    QBrush brush(geoParameters.fillColor);
    painter.setBrush(brush);

    // ����·��
    painter.drawPath(path);

    // �ָ��������ʽ
    painter.setBrush(Qt::NoBrush);


}

void TwoPointCircle::drawBuffer(QPainter& painter)
{
    if (!geoParameters.bufferVisible)
    {
        return; // ������������ɼ���ֱ�ӷ���
    }

    // ���û������߿�
    if (!geoParameters.bufferHasBorder)
    {
        painter.setPen(Qt::NoPen);
    }
    else
    {
        QPen bufferPen;
        bufferPen.setColor(geoParameters.bufferLineColor);
        bufferPen.setWidthF(geoParameters.bufferLineWidth);
        if (geoParameters.bufferLineStyle == LineStyle::Dashed)
        {
            bufferPen.setStyle(Qt::DashLine);
            bufferPen.setDashOffset(geoParameters.bufferLineDashPattern);
        }
        else
        {
            bufferPen.setStyle(Qt::SolidLine);
        }
        painter.setPen(bufferPen);
    }

    // ���û��������
    QBrush bufferBrush(QColor(geoParameters.bufferFillColor));
    painter.setBrush(bufferBrush);

    // ����������仯��·��Ϊ�գ����¼��㻺����·��
    if (isBufferChanged())
    {
        resetBufferChanged(); // ���û�����״̬
        bufferPath = QPainterPath(); // ��ղ���������·��

        QVector<QVector<QPointF>> pointss = { points };
        if (computeBufferBoundary( geoParameters.bufferCalculationMode,pointss, geoParameters.bufferDistance, buffers))
        {
            for (auto& points : buffers)
            {
                if (points.size() < 2) continue; // ������Ч��ͼ
                bufferPath.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    bufferPath.lineTo(points[i]);
                }
            }
        }
        bufferPath = bufferPath.subtracted(path); // ������
    }

    // ���ƻ�����·��
    painter.drawPath(bufferPath);

    // �ָ������״̬
    painter.setBrush(Qt::NoBrush);
}


// ===================================================================== SimpleArea
SimpleArea::SimpleArea()
{
    setGeoType(GeoType::TypeSimpleArea);
    setStateSelected(); // ���ڻ����У��ǵ�ǰѡ��
}

void SimpleArea::mousePressEvent(QMouseEvent* event)
{
    if (!isStateInitialized())initialize();
    if (isStateComplete())return;
    //--------------------------------------
    if (event->button() == Qt::RightButton)
    {
        endSegmentDrawing();
    }
    if (event->button() == Qt::LeftButton)
    {
        pushControlPoint(event->pos());
    }

    Geo::mousePressEvent(event);
}

void SimpleArea::mouseMoveEvent(QMouseEvent* event)
{
    if (!isStateComplete())
    {
        tempControlPoints = event->pos();
    }

    // ���������, �����ƶ���ʱ��ͻ���
    if (!isStateComplete() && geoParameters.nodeLineStyle == NodeLineStyle::StyleStreamline && isMouseLeftButtonPressed())
    {
        pushControlPoint(event->pos());
    }
}

void SimpleArea::mouseReleaseEvent(QMouseEvent* event)
{
    // ��������߽����λ���
    if (!isStateComplete() && geoParameters.nodeLineStyle == NodeLineStyle::StyleStreamline && isMouseLeftButtonPressed())
    {
        endSegmentDrawing();
    }
    Geo::mouseReleaseEvent(event);
}

void SimpleArea::completeDrawing()
{
    if (!calculateCloseLinePoints(geoParameters.nodeLineStyle, controlPoints, points))
    {
        setStateInvalid();
    }
    Geo::completeDrawing();
}

bool SimpleArea::hitTesting(const QPointF& point)
{
    return Geo::hitTesting(point) || path.contains(point);
}

void SimpleArea::draw(QPainter& painter)
{
    if (!isStateInitialized() || isStateInvalid())
    {
        return;
    }

    // ��̬���¿��Ƶ�
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        markControlPointsChanged();
    }

    // ������Ƶ㷢���ı䣬���¼���·��
    if (isControlPointsChanged())
    {
        resetControlPointsChanged(); // ���ÿ��Ƶ���ı�־

        // ���¼���·��
        path = QPainterPath();
        if (!controlPoints.isEmpty())
        {
            if (calculateCloseLinePoints(geoParameters.nodeLineStyle, controlPoints, points))
            {
                if (!points.isEmpty())
                {
                    path.moveTo(points.first());
                    for (int i = 1; i < points.size(); ++i)
                    {
                        path.lineTo(points[i]);
                    }
                }
            }
        }
    }
    // �ָ���̬���Ƶ�
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.pop_back();
        markControlPointsChanged();
    }



    // ���״̬����һ������ɼ������ƻ�����
    if (isStateComplete() && geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }

    // �����ѡ�У����ƿ��Ƶ�
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }

    // ���״̬����һ������ɼ������ƻ�����
    if (isStateComplete() && geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }
    // �����ѡ�У����ƿ��Ƶ�
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }

    // ���û���
    QPen pen;
    pen.setColor(geoParameters.lineColor);
    pen.setWidthF(geoParameters.lineWidth);
    if (geoParameters.lineStyle == LineStyle::Dashed)
    {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(geoParameters.lineDashPattern);
    }
    else
    {
        pen.setStyle(Qt::SolidLine);
    }
    painter.setPen(pen);
    // ���û�ˢ
    QBrush brush(geoParameters.fillColor);
    painter.setBrush(brush);
    // ����·��
    painter.drawPath(path);
    // �ָ�����仭ˢ
    painter.setBrush(Qt::NoBrush);
}

void SimpleArea::drawBuffer(QPainter& painter)
{
    if (!geoParameters.bufferVisible)
    {
        return; // ������������ɼ���ֱ�ӷ���
    }

    // ���û������߿�
    if (!geoParameters.bufferHasBorder)
    {
        painter.setPen(Qt::NoPen);
    }
    else
    {
        QPen bufferPen;
        bufferPen.setColor(geoParameters.bufferLineColor);
        bufferPen.setWidthF(geoParameters.bufferLineWidth);
        if (geoParameters.bufferLineStyle == LineStyle::Dashed)
        {
            bufferPen.setStyle(Qt::DashLine);
            bufferPen.setDashOffset(geoParameters.bufferLineDashPattern);
        }
        else
        {
            bufferPen.setStyle(Qt::SolidLine);
        }
        painter.setPen(bufferPen);
    }

    // ���û��������
    QBrush bufferBrush(QColor(geoParameters.bufferFillColor));
    painter.setBrush(bufferBrush);

    // ����������仯��·��Ϊ�գ����¼��㻺����·��
    if (isBufferChanged())
    {
        resetBufferChanged(); // ���û�����״̬
        bufferPath = QPainterPath(); // ��ղ���������·��

        QVector<QVector<QPointF>> pointss = { points };
        if (computeBufferBoundary( geoParameters.bufferCalculationMode,pointss, geoParameters.bufferDistance, buffers))
        {
            for (auto& points : buffers)
            {
                if (points.size() < 2) continue; // ������Ч��ͼ
                bufferPath.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    bufferPath.lineTo(points[i]);
                }
            }
        }
        bufferPath = bufferPath.subtracted(path);
    }

    // ���ƻ�����·��
    painter.drawPath(bufferPath);

    // �ָ������״̬
    painter.setBrush(Qt::NoBrush);
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

    Geo::initialize();
}

void ComplexArea::mousePressEvent(QMouseEvent* event)
{
    if (!isStateInitialized())initialize();
    if (isStateComplete())return;
    //--------------------------------------
    if (event->button() == Qt::RightButton)
    {
        endSegmentDrawing();
    }
    if (event->button() == Qt::LeftButton)
    {
        pushControlPoint(event->pos());
    }

    Geo::mousePressEvent(event);
}

void ComplexArea::mouseMoveEvent(QMouseEvent* event)
{
    if (!isStateComplete())
    {
        tempControlPoints = event->pos();
    }
    if (isDrawing && component.last().nodeLineStyle == NodeLineStyle::StyleStreamline && isMouseLeftButtonPressed())
    {
        pushControlPoint(event->pos());
    }
}

void ComplexArea::mouseReleaseEvent(QMouseEvent* event)
{
    // ��������߽����λ���
    if (isDrawing && component.last().nodeLineStyle == NodeLineStyle::StyleStreamline && isMouseLeftButtonPressed())
    {
        endSegmentDrawing();
    }
    Geo::mouseReleaseEvent(event);
}

void ComplexArea::pushControlPoint(const QPoint& pos)
{
    markControlPointsChanged();

    isDrawing = true;
    controlPoints.push_back(pos);
    component.last().len++;
    // ����ڵ����λ�û����
    if (component.last().nodeLineStyle == NodeLineStyle::NoStyle)
    {
        component.last().nodeLineStyle = geoParameters.nodeLineStyle;
    }
}

bool ComplexArea::hitTesting(const QPointF& point)
{
    return Geo::hitTesting(point) || path.contains(point);
}

void ComplexArea::endSegmentDrawing()
{
    if (isDrawing) // ���Ʒ�ͼ
    {
        isDrawing = false;
        // �½���ͼ
        component.push_back({ 0,NodeLineStyle::NoStyle });
    }
    else
    {
        isDrawing = false;
        completeDrawing();
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
    if (!isStateInitialized() || isStateInvalid())
    {
        return;
    }

    // ��̬�����ʱ���Ƶ�
    if (isDrawing && isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        component.last().len++;
        markControlPointsChanged();
    }

    // ������Ƶ�ı��·��δ���㣬����������·��
    if (isControlPointsChanged())
    {
        resetControlPointsChanged(); // ���ÿ��Ƶ���ı�־

        // ���¼���·��
        path = QPainterPath();
        if (calculateCloseLinePoints(component, controlPoints, pointss))
        {
            for (auto& points : pointss)
            {
                if (points.size() < 3)continue; // ��һ����ͼʧЧ
                path.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    path.lineTo(points[i]);
                }
            }
        }
    }

    // �ָ���̬���Ƶ�
    if (isDrawing && isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.pop_back();
        component.last().len--;
        markControlPointsChanged();
    }


    // ���״̬����һ������ɼ������ƻ�����
    if (isStateComplete() && geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }

    // �����ѡ�У����ƿ��Ƶ�
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }

    // ���û���
    QPen pen;
    pen.setColor(geoParameters.lineColor);
    pen.setWidthF(geoParameters.lineWidth);
    if (geoParameters.lineStyle == LineStyle::Dashed)
    {
        pen.setStyle(Qt::DashLine);
        pen.setDashOffset(geoParameters.lineDashPattern);
    }
    else
    {
        pen.setStyle(Qt::SolidLine);
    }
    painter.setPen(pen);

    // ���û�ˢ
    QBrush brush(geoParameters.fillColor);
    painter.setBrush(brush);

    // ����·��
    painter.drawPath(path);

    // �ָ�����仭ˢ
    painter.setBrush(Qt::NoBrush);
}

void ComplexArea::drawBuffer(QPainter& painter)
{
    if (!geoParameters.bufferVisible)
    {
        return; // ������������ɼ���ֱ�ӷ���
    }

    // ���û������߿�
    if (!geoParameters.bufferHasBorder)
    {
        painter.setPen(Qt::NoPen);
    }
    else
    {
        QPen bufferPen;
        bufferPen.setColor(geoParameters.bufferLineColor);
        bufferPen.setWidthF(geoParameters.bufferLineWidth);
        if (geoParameters.bufferLineStyle == LineStyle::Dashed)
        {
            bufferPen.setStyle(Qt::DashLine);
            bufferPen.setDashOffset(geoParameters.bufferLineDashPattern);
        }
        else
        {
            bufferPen.setStyle(Qt::SolidLine);
        }
        painter.setPen(bufferPen);
    }

    // ���û��������
    QBrush bufferBrush(QColor(geoParameters.bufferFillColor));
    painter.setBrush(bufferBrush);

    // ����������仯��·��Ϊ�գ����¼��㻺����·��
    if (isBufferChanged())
    {
        resetBufferChanged(); // ���û�����״̬
        bufferPath = QPainterPath(); // ��ղ���������·��

        if (computeBufferBoundary( geoParameters.bufferCalculationMode ,pointss, geoParameters.bufferDistance, buffers))
        {
            for (auto& points : buffers)
            {
                if (points.size() < 2) continue; // ������Ч��ͼ
                bufferPath.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    bufferPath.lineTo(points[i]);
                }
            }
        }
        bufferPath = bufferPath.subtracted(path);
    }

    // ���ƻ�����·��
    painter.drawPath(bufferPath);

    // �ָ������״̬
    painter.setBrush(Qt::NoBrush);
}
