
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

bool Geo::hitTesting(QPointF point)
{
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
    return selected;
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
    geoParameters = params;
    markBufferChanged();
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
    // 如果被选中,画控制点
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }
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

void SimpleLine::draw(QPainter& painter)
{
    if (!isStateInitialized() || isStateInvalid())
    {
        return;
    }
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

    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        markControlPointsChanged();
    }
    if (!isControlPointsChanged() || calculateLinePoints(geoParameters.nodeLineStyle, controlPoints, points))
    {
        resetControlPointsChanged();
        QPainterPath path;
        path.moveTo(points.first());
        for (int i = 1; i < points.size(); ++i)
        {
            path.lineTo(points[i]);
        }
        painter.drawPath(path);
    }
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        markControlPointsChanged();
        controlPoints.pop_back();
    }

    // 如果被选中,画控制点
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }

    if (geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }
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
    if (isDrawing && isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        component.last().len++;
        markControlPointsChanged();
    }
    if (!isControlPointsChanged() || calculateLinePoints(component, controlPoints, pointss))
    {
        resetControlPointsChanged();
        QPainterPath path;
        for (auto& points : pointss)
        {
            if (points.size() < 2)continue; // 这一个分图失效
            path.moveTo(points.first());
            for (int i = 1; i < points.size(); ++i)
            {
                path.lineTo(points[i]);
            }
        }
        painter.drawPath(path);
    }
    if (isDrawing && isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.pop_back();
        component.last().len--;
        markControlPointsChanged();
    }
    // 如果被选中,画控制点
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }
    if (isStateComplete() && geoParameters.bufferVisible)
    {
        drawBuffer(painter);
    }
}

void DoubleLine::drawBuffer(QPainter& painter)
{
    if (!geoParameters.bufferVisible)
    {
        return; // 如果缓冲区不可见，直接返回
    }

    // 设置缓冲区线的样式
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

    // 如果缓冲区需要边框
    if (geoParameters.bufferHasBorder)
    {
        // 绘制缓冲区边界的路径
        QPainterPath bufferPath;
        // 计算缓冲区的路径，这里是空的，需要您根据需求实现点的计算逻辑
        if (!isBufferChanged() || calculateBuffer(pointss, geoParameters.bufferDistance, buffers))
        {
            resetBufferChanged();
            QPainterPath path;
            for (auto& points : buffers)
            {
                if (points.size() < 2)continue; // 这一个分图失效
                path.moveTo(points.first());
                for (int i = 1; i < points.size(); ++i)
                {
                    path.lineTo(points[i]);
                }
            }
            painter.drawPath(path);
        }

        painter.drawPath(bufferPath);
    }

    // 绘制缓冲区面填充
    QBrush bufferBrush(QColor(geoParameters.bufferFillColor));
    painter.setBrush(bufferBrush);
    // 绘制填充区域
    //
    //
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

void ParallelLine::draw(QPainter& painter)
{
    if (!isStateInitialized() || isStateInvalid())
    {
        return;
    }
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
    
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        component.last().len++;
        markControlPointsChanged();
    }
    if (!isControlPointsChanged() || calculateParallelLinePoints(component, controlPoints, pointss))
    {
        resetControlPointsChanged();
        QPainterPath path;
        for (auto& points : pointss)
        {
            if (points.size() < 2)continue; // 这一个分图失效
            path.moveTo(points.first());
            for (int i = 1; i < points.size(); ++i)
            {
                path.lineTo(points[i]);
                // path.addEllipse(points[i], 2, 2);
            }
        }
        painter.drawPath(path);
    }
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.pop_back();
        component.last().len--;
        markControlPointsChanged();
    }

    // 如果被选中,画控制点
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }
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

    QBrush brush(geoParameters.fillColor);
    painter.setBrush(brush);
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        markControlPointsChanged();
    }
    if (!isControlPointsChanged() || calculateCloseLinePoints(NodeLineStyle::StyleTwoPointCircle, controlPoints, points))
    {
        resetControlPointsChanged();
        QPainterPath path;
        if (points.size())path.moveTo(points.first());
        for (int i = 1; i < points.size(); ++i)
        {
            path.lineTo(points[i]);
        }
        painter.drawPath(path);
    }
    painter.setBrush(Qt::NoBrush); // 恢复无填充   
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.pop_back();
        markControlPointsChanged();
    }

    // 如果被选中,画控制点
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }
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

void SimpleArea::draw(QPainter& painter)
{
    if (!isStateInitialized() || isStateInvalid())
    {
        return;
    }
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

    QBrush brush(geoParameters.fillColor);
    painter.setBrush(brush);
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        markControlPointsChanged();
    }
    if (!isControlPointsChanged() || calculateCloseLinePoints(geoParameters.nodeLineStyle, controlPoints, points))
    {
        resetControlPointsChanged();
        QPainterPath path;
        if(points.size())path.moveTo(points.first());
        for (int i = 1; i < points.size(); ++i)
        {
            path.lineTo(points[i]);
        }
        painter.drawPath(path);
    }
    painter.setBrush(Qt::NoBrush); // 恢复无填充   
    if (isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.pop_back();
        markControlPointsChanged();
    }

    // 如果被选中,画控制点
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }
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
    if (!isStateInitialized()||isStateInvalid())
    {
        return;
    }
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

    QBrush brush(geoParameters.fillColor);
    painter.setBrush(brush);
    if (isDrawing && isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.push_back(tempControlPoints);
        component.last().len++;
        markControlPointsChanged();
    }
    if (!isControlPointsChanged() || calculateCloseLinePoints(component, controlPoints, pointss))
    {
        resetControlPointsChanged();

        QPainterPath path;
        for (auto& points : pointss)
        {
            if (points.size() < 3)continue; // 这一个分图失效
            path.moveTo(points.first());
            for (int i = 1; i < points.size(); ++i)
            {
                path.lineTo(points[i]);
            }
        }
        painter.drawPath(path);
    }
    painter.setBrush(Qt::NoBrush); // 恢复无填充   
    if (isDrawing && isStateDrawing() && !tempControlPoints.isNull())
    {
        controlPoints.pop_back();
        component.last().len--;
        markControlPointsChanged();
    }
    // 如果被选中,画控制点
    if (isStateSelected())
    {
        drawControlPoints(painter);
    }
}
