
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
        // 设置画笔和画刷
        painter.setPen(QPen(Qt::lightGray, 2));
        painter.setBrush(Qt::NoBrush);
        // 绘制控制点
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
    // 基类判断控制点点的点击测试
    // 处理点击测试，判断是否点击到目标区域
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
    // 如果选择的是控制点,就修改控制点
    if (currentSelectedPoint)
    {
        *currentSelectedPoint = point;
        markControlPointsChanged();
    }
    else // 不然移动图
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
    setStateNotSelected(); // 一开始是选中状态
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

// 获取当前的 GeoParameters
GeoParameters Geo::getGeoParameters()
{
    return geoParameters; // 返回当前结构体
}

// 设置 GeoParameters
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

// 如果选中
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
    setStateSelected(); // 还在绘制中，是当前选中
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

    // 如果状态完成且缓冲区可见，绘制缓冲区
    if (isStateComplete() && geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }

    // 如果被选中，绘制控制点
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
        return; // 如果缓冲区不可见，直接返回
    }

    // 设置缓冲区边框
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

    // 如果缓冲区变化或路径为空，重新计算缓冲区路径
    if (isBufferChanged())
    {
        resetBufferChanged(); // 重置缓冲区状态
        bufferPath = QPainterPath(); // 清空并重新生成路径

        QVector<QVector<QPointF>>pointss = { controlPoints };
        if (computeBufferBoundary( geoParameters.bufferCalculationMode,pointss, geoParameters.bufferDistance, buffers))
        {
            for (auto& points : buffers)
            {
                if (points.size() < 2) continue; // 跳过无效分图
                bufferPath.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    bufferPath.lineTo(points[i]);
                }
            }
        }
    }

    // 绘制缓冲区路径
    painter.drawPath(bufferPath);

    // 恢复无填充状态
    painter.setBrush(Qt::NoBrush);
}
// ===================================================================== SimpleLine

SimpleLine::SimpleLine()
{
    setGeoType(GeoType::TypeSimpleLine);
    setStateSelected(); // 还在绘制中，是当前选中
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
    // 如果是流线结束段绘制
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

    // 遍历折线的所有线段
    for (int i = 0; i < points.size() - 1; ++i) 
    {
        QPointF& p1 = points[i];     // 线段起点
        QPointF& p2 = points[i + 1]; // 线段终点

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

    // 如果状态完成且缓冲区可见，绘制缓冲区
    if (isStateComplete() && geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }

    // 如果被选中，绘制控制点
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }

    // 设置画笔
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

    // 动态添加临时控制点
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        markControlPointsChanged(); // 标记控制点发生了变化
    }

    // 如果控制点发生变化或路径为空，则重新计算路径
    if (isControlPointsChanged())
    {
        resetControlPointsChanged(); // 重置控制点状态
        path = QPainterPath();       // 清空并重新生成路径

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

    // 绘制路径
    painter.drawPath(path);

    // 恢复动态控制点
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
        return; // 如果缓冲区不可见，直接返回
    }

    // 设置缓冲区边框
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

    // 如果缓冲区变化或路径为空，重新计算缓冲区路径
    if (isBufferChanged() || bufferPath.isEmpty())
    {
        resetBufferChanged(); // 重置缓冲区状态
        bufferPath = QPainterPath(); // 清空并重新生成路径

        QVector<QVector<QPointF>>pointss = { points };
        if (computeBufferBoundary( geoParameters.bufferCalculationMode,pointss, geoParameters.bufferDistance, buffers))
        {
            for (auto& points : buffers)
            {
                if (points.size() < 2) continue; // 跳过无效分图
                bufferPath.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    bufferPath.lineTo(points[i]);
                }
            }
        }
    }

    // 绘制缓冲区路径
    painter.drawPath(bufferPath);

    // 恢复无填充状态
    painter.setBrush(Qt::NoBrush);
}


// ===================================================================== DoubleLine
DoubleLine::DoubleLine()
{
    setGeoType(GeoType::TypeComplexArea);
    setStateSelected(); // 还在绘制中，是当前选中
}

void DoubleLine::initialize()
{
    isDrawing = true;   // 第一个分图准备绘制
    component.push_back({ 0,NodeLineStyle::NoStyle });      // 新建分图
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

        // 如果是流线,按下移动的时候就绘制
        if (isDrawing && component.last().nodeLineStyle == NodeLineStyle::StyleStreamline && isMouseLeftButtonPressed())
        {
            pushControlPoint(event->pos());
        }
    }
}

void DoubleLine::mouseReleaseEvent(QMouseEvent* event)
{
    // 如果是流线结束段绘制
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
        // 遍历折线的所有线段
        for (int i = 0; i < points.size() - 1; ++i) {
            QPointF& p1 = points[i];     // 线段起点
            QPointF& p2 = points[i + 1]; // 线段终点

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


    // 如果状态完成且缓冲区可见，绘制缓冲区
    if (isStateComplete() && geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }

    // 如果被选中，绘制控制点
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }


    // 设置画笔
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

    // 动态添加临时控制点
    if (isDrawing && isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        component.last().len++;
        markControlPointsChanged(); // 标记控制点发生了变化
    }

    // 如果控制点发生变化，重新计算路径
    if (isControlPointsChanged())
    {
        resetControlPointsChanged(); // 重置控制点状态
        path = QPainterPath();       // 清空并重新生成路径

        if (calculateLinePoints(component, controlPoints, pointss))
        {
            for (const auto& points : pointss)
            {
                if (points.size() < 2) continue; // 忽略无效分图

                path.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    path.lineTo(points[i]);
                }
            }
        }
    }

    // 绘制路径
    painter.drawPath(path);

    // 恢复动态控制点
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
        return; // 如果缓冲区不可见，直接返回
    }

    // 设置缓冲区边框
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

    // 设置缓冲区填充
    QBrush bufferBrush(QColor(geoParameters.bufferFillColor));
    painter.setBrush(bufferBrush);

    // 如果缓冲区变化或路径为空，重新计算缓冲区路径
    if (isBufferChanged())
    {
        resetBufferChanged(); // 重置缓冲区状态
        bufferPath = QPainterPath(); // 清空并重新生成路径

        if (computeBufferBoundary( geoParameters.bufferCalculationMode,pointss, geoParameters.bufferDistance, buffers))
        {
            for (auto& points : buffers)
            {
                if (points.size() < 2) continue; // 跳过无效分图
                bufferPath.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    bufferPath.lineTo(points[i]);
                }
            }
        }
    }

    // 绘制缓冲区路径
    painter.drawPath(bufferPath);

    // 恢复无填充状态
    painter.setBrush(Qt::NoBrush);
}




// ===================================================================== ParallelLine
ParallelLine::ParallelLine()
{
    setGeoType(GeoType::TypeComplexArea);
    setStateSelected(); // 还在绘制中，是当前选中
}

void ParallelLine::initialize()
{
    component.push_back({ 0,NodeLineStyle::NoStyle });      // 新建分图

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

    // 如果是流线, 按下移动的时候就绘制
    if (!isStateComplete() && component.last().nodeLineStyle == NodeLineStyle::StyleStreamline && isMouseLeftButtonPressed())
    {
        pushControlPoint(event->pos());
    }
}

void ParallelLine::mouseReleaseEvent(QMouseEvent* event)
{
    // 如果是流线结束段绘制
    if (!isStateComplete() && component.last().nodeLineStyle == NodeLineStyle::StyleStreamline && isMouseLeftButtonPressed())
    {
        endSegmentDrawing();
    }
    Geo::mouseReleaseEvent(event);
}

void ParallelLine::endSegmentDrawing()
{
    if (component.size() < 2) // 绘制分图,限制两条W
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
        // 如果节点线形还没定义
        if (component.last().nodeLineStyle == NodeLineStyle::NoStyle)
        {
            component.last().nodeLineStyle = geoParameters.nodeLineStyle;
        }
    }
    else if (component.size() == 2)
    {
        controlPoints.push_back(pos);
        component.last().len++;
        endSegmentDrawing(); // 第二个分量就一个控制点
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
        // 遍历折线的所有线段
        for (int i = 0; i < points.size() - 1; ++i) {
            QPointF& p1 = points[i];     // 线段起点
            QPointF& p2 = points[i + 1]; // 线段终点

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

    // 如果状态完成且缓冲区可见，绘制缓冲区
    if (isStateComplete() && geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }

    // 如果被选中，绘制控制点
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }

    // 设置画笔
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

    // 动态添加临时控制点
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        component.last().len++;
        markControlPointsChanged(); // 标记控制点发生了变化
    }

    // 如果控制点发生变化，重新计算路径
    if (isControlPointsChanged())
    {
        resetControlPointsChanged(); // 重置控制点状态
        path = QPainterPath();       // 清空并重新生成路径

        if (calculateParallelLinePoints(component, controlPoints, pointss))
        {
            for (const auto& points : pointss)
            {
                if (points.size() < 2) continue; // 忽略无效分图

                path.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    path.lineTo(points[i]);
                }
            }
        }
    }

    // 绘制路径
    painter.drawPath(path);

    // 恢复动态控制点
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
        return; // 如果缓冲区不可见，直接返回
    }

    // 设置缓冲区边框
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

    // 设置缓冲区填充
    QBrush bufferBrush(QColor(geoParameters.bufferFillColor));
    painter.setBrush(bufferBrush);

    // 如果缓冲区变化或路径为空，重新计算缓冲区路径
    if (isBufferChanged())
    {
        resetBufferChanged(); // 重置缓冲区状态
        bufferPath = QPainterPath(); // 清空并重新生成路径

        if (computeBufferBoundary( geoParameters.bufferCalculationMode,pointss, geoParameters.bufferDistance, buffers))
        {
            for (auto& points : buffers)
            {
                if (points.size() < 2) continue; // 跳过无效分图
                bufferPath.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    bufferPath.lineTo(points[i]);
                }
            }
        }
    }
    // 绘制缓冲区路径
    painter.drawPath(bufferPath);
    // 恢复无填充状态
    painter.setBrush(Qt::NoBrush);
}


// ===================================================================== TwoPointCircle
TwoPointCircle::TwoPointCircle()
{
    setGeoType(GeoType::TypeSimpleArea);
    setStateSelected(); // 还在绘制中，是当前选中
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

    // 如果正在绘制并且有临时控制点，则加入控制点
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        markControlPointsChanged(); // 标记控制点已更改
    }
    // 如果控制点发生改变，重新计算路径
    if (isControlPointsChanged())
    {
        resetControlPointsChanged(); // 重置控制点更改标志

        // 重新计算路径
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
    // 如果正在绘制并且有临时控制点，则移除临时点
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.pop_back();
        markControlPointsChanged(); // 标记控制点已更改
    }

    // 如果状态完成且缓冲区可见，绘制缓冲区
    if (isStateComplete() && geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }

    // 如果被选中，绘制控制点
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }

    // 设置画笔样式
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

    // 设置画刷样式
    QBrush brush(geoParameters.fillColor);
    painter.setBrush(brush);

    // 绘制路径
    painter.drawPath(path);

    // 恢复无填充样式
    painter.setBrush(Qt::NoBrush);


}

void TwoPointCircle::drawBuffer(QPainter& painter)
{
    if (!geoParameters.bufferVisible)
    {
        return; // 如果缓冲区不可见，直接返回
    }

    // 设置缓冲区边框
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

    // 设置缓冲区填充
    QBrush bufferBrush(QColor(geoParameters.bufferFillColor));
    painter.setBrush(bufferBrush);

    // 如果缓冲区变化或路径为空，重新计算缓冲区路径
    if (isBufferChanged())
    {
        resetBufferChanged(); // 重置缓冲区状态
        bufferPath = QPainterPath(); // 清空并重新生成路径

        QVector<QVector<QPointF>> pointss = { points };
        if (computeBufferBoundary( geoParameters.bufferCalculationMode,pointss, geoParameters.bufferDistance, buffers))
        {
            for (auto& points : buffers)
            {
                if (points.size() < 2) continue; // 跳过无效分图
                bufferPath.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    bufferPath.lineTo(points[i]);
                }
            }
        }
        bufferPath = bufferPath.subtracted(path); // 差运算
    }

    // 绘制缓冲区路径
    painter.drawPath(bufferPath);

    // 恢复无填充状态
    painter.setBrush(Qt::NoBrush);
}


// ===================================================================== SimpleArea
SimpleArea::SimpleArea()
{
    setGeoType(GeoType::TypeSimpleArea);
    setStateSelected(); // 还在绘制中，是当前选中
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

    // 如果是流线, 按下移动的时候就绘制
    if (!isStateComplete() && geoParameters.nodeLineStyle == NodeLineStyle::StyleStreamline && isMouseLeftButtonPressed())
    {
        pushControlPoint(event->pos());
    }
}

void SimpleArea::mouseReleaseEvent(QMouseEvent* event)
{
    // 如果是流线结束段绘制
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

    // 动态更新控制点
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        markControlPointsChanged();
    }

    // 如果控制点发生改变，重新计算路径
    if (isControlPointsChanged())
    {
        resetControlPointsChanged(); // 重置控制点更改标志

        // 重新计算路径
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
    // 恢复动态控制点
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.pop_back();
        markControlPointsChanged();
    }



    // 如果状态完成且缓冲区可见，绘制缓冲区
    if (isStateComplete() && geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }

    // 如果被选中，绘制控制点
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }

    // 如果状态完成且缓冲区可见，绘制缓冲区
    if (isStateComplete() && geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }
    // 如果被选中，绘制控制点
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }

    // 设置画笔
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
    // 设置画刷
    QBrush brush(geoParameters.fillColor);
    painter.setBrush(brush);
    // 绘制路径
    painter.drawPath(path);
    // 恢复无填充画刷
    painter.setBrush(Qt::NoBrush);
}

void SimpleArea::drawBuffer(QPainter& painter)
{
    if (!geoParameters.bufferVisible)
    {
        return; // 如果缓冲区不可见，直接返回
    }

    // 设置缓冲区边框
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

    // 设置缓冲区填充
    QBrush bufferBrush(QColor(geoParameters.bufferFillColor));
    painter.setBrush(bufferBrush);

    // 如果缓冲区变化或路径为空，重新计算缓冲区路径
    if (isBufferChanged())
    {
        resetBufferChanged(); // 重置缓冲区状态
        bufferPath = QPainterPath(); // 清空并重新生成路径

        QVector<QVector<QPointF>> pointss = { points };
        if (computeBufferBoundary( geoParameters.bufferCalculationMode,pointss, geoParameters.bufferDistance, buffers))
        {
            for (auto& points : buffers)
            {
                if (points.size() < 2) continue; // 跳过无效分图
                bufferPath.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    bufferPath.lineTo(points[i]);
                }
            }
        }
        bufferPath = bufferPath.subtracted(path);
    }

    // 绘制缓冲区路径
    painter.drawPath(bufferPath);

    // 恢复无填充状态
    painter.setBrush(Qt::NoBrush);
}


// ===================================================================== ComplexArea
ComplexArea::ComplexArea()
{
    setGeoType(GeoType::TypeComplexArea);
    setStateSelected(); // 还在绘制中，是当前选中
}

void ComplexArea::initialize()
{
    isDrawing = true;   // 第一个分图准备绘制
    component.push_back({ 0,NodeLineStyle::NoStyle });      // 新建分图

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
    // 如果是流线结束段绘制
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
    // 如果节点线形还没定义
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
    if (isDrawing) // 绘制分图
    {
        isDrawing = false;
        // 新建分图
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

    // 动态添加临时控制点
    if (isDrawing && isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        component.last().len++;
        markControlPointsChanged();
    }

    // 如果控制点改变或路径未计算，则重新生成路径
    if (isControlPointsChanged())
    {
        resetControlPointsChanged(); // 重置控制点更改标志

        // 重新计算路径
        path = QPainterPath();
        if (calculateCloseLinePoints(component, controlPoints, pointss))
        {
            for (auto& points : pointss)
            {
                if (points.size() < 3)continue; // 这一个分图失效
                path.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    path.lineTo(points[i]);
                }
            }
        }
    }

    // 恢复动态控制点
    if (isDrawing && isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.pop_back();
        component.last().len--;
        markControlPointsChanged();
    }


    // 如果状态完成且缓冲区可见，绘制缓冲区
    if (isStateComplete() && geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }

    // 如果被选中，绘制控制点
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }

    // 设置画笔
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

    // 设置画刷
    QBrush brush(geoParameters.fillColor);
    painter.setBrush(brush);

    // 绘制路径
    painter.drawPath(path);

    // 恢复无填充画刷
    painter.setBrush(Qt::NoBrush);
}

void ComplexArea::drawBuffer(QPainter& painter)
{
    if (!geoParameters.bufferVisible)
    {
        return; // 如果缓冲区不可见，直接返回
    }

    // 设置缓冲区边框
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

    // 设置缓冲区填充
    QBrush bufferBrush(QColor(geoParameters.bufferFillColor));
    painter.setBrush(bufferBrush);

    // 如果缓冲区变化或路径为空，重新计算缓冲区路径
    if (isBufferChanged())
    {
        resetBufferChanged(); // 重置缓冲区状态
        bufferPath = QPainterPath(); // 清空并重新生成路径

        if (computeBufferBoundary( geoParameters.bufferCalculationMode ,pointss, geoParameters.bufferDistance, buffers))
        {
            for (auto& points : buffers)
            {
                if (points.size() < 2) continue; // 跳过无效分图
                bufferPath.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    bufferPath.lineTo(points[i]);
                }
            }
        }
        bufferPath = bufferPath.subtracted(path);
    }

    // 绘制缓冲区路径
    painter.drawPath(bufferPath);

    // 恢复无填充状态
    painter.setBrush(Qt::NoBrush);
}
