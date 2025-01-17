#include "comm.h"

#include <QColor>
#include <QStatusBar>

DrawMode GlobalDrawMode;

NodeLineStyle GlobalNodeLineStyle;    // 节点线形
PointShape GlobalPointShape;           // 点形状
QColor GlobalPointColor;                 // 点颜色
LineStyle GlobalLineStyle;             // 线样式
float GlobalLineWidth;                 // 线宽
QColor GlobalLineColor;                  // 线颜色
float GlobalLineDashPattern;           // 虚线段长
QColor GlobalFillColor;                  // 填充颜色
int GlobalSplineOrder;                 // 样条阶数
int GlobalSplineNodeCount;             // 样条节点数
int GlobalSteps;                       // 计算点密度

bool GlobalBufferVisible;              // 是否显示缓冲区
BufferCalculationMode GlobalBufferCalculationMode; // 缓冲区计算方式（矢量、栅格）
double GlobalBufferDistance;           // 缓冲区距离

// 缓冲区线的属性
bool GlobalBufferHasBorder;
LineStyle GlobalBufferLineStyle;       // 缓冲区线样式
float GlobalBufferLineWidth;           // 缓冲区线宽
QColor GlobalBufferLineColor;            // 缓冲区线颜色
float GlobalBufferLineDashPattern;     // 缓冲区虚线段长

// 缓冲区面的属性
QColor GlobalBufferFillColor;            // 缓冲区填充颜色

QStatusBar* GlobalStatusBar;            // 状态栏

void initializeGlobalDrawSettings()
{
    // 全局绘图模式初始化为选择模式
    GlobalDrawMode = DrawMode::DrawSelect;

    // 点形状、颜色初始化
    GlobalPointShape = Circle;                          // 默认点形状为圆
    GlobalPointColor = QColor(Qt::blue);                // 默认点颜色为蓝色

    // 线条属性初始化
    GlobalLineStyle = Solid;                            // 默认线条样式为实线
    GlobalLineWidth = 1.5f;                             // 默认线宽为 1.5
    GlobalLineColor = QColor(Qt::darkGray);             // 默认线条颜色为深灰色
    GlobalLineDashPattern = 0.0f;                       // 实线，无虚线模式

    // 多段线属性初始化
    GlobalNodeLineStyle = StylePolyline;                // 多段线样式初始化为折线
    GlobalFillColor = QColor(Qt::transparent);          // 默认填充颜色为透明

    // 样条曲线相关初始化
    GlobalSplineOrder = 3;                              // 样条曲线阶数
    GlobalSplineNodeCount = 20;                        // 默认样条节点数为 500
    GlobalSteps = 20;                                  // 细分步数设为 500

    // 缓冲区属性初始化
    GlobalBufferVisible = false;                                    // 默认不显示缓冲区
    GlobalBufferCalculationMode = BufferCalculationMode::Vector;    // 默认栅格模式
    GlobalBufferDistance = 50.0;                                    // 默认缓冲区距离设为 50.0

    // 缓冲区线的属性初始化
    GlobalBufferHasBorder = true;                       // 默认显示缓冲区边界
    GlobalBufferLineStyle = LineStyle::Dashed;          // 缓冲区边界线为虚线
    GlobalBufferLineWidth = 1.0f;                       // 边界线宽设为 1.0
    GlobalBufferLineColor = QColor(Qt::darkBlue);       // 缓冲区边界颜色为深蓝色
    GlobalBufferLineDashPattern = 3.0f;                 // 点状线段长度设为 3.0

    // 缓冲区填充颜色初始化
    GlobalBufferFillColor = QColor(200, 200, 255, 100); // 浅蓝色，半透明

    // 状态栏初始化
    GlobalStatusBar = new QStatusBar();                 // 创建状态栏实例
}



#ifdef DEBUG
QVector<QVector<QPointF>> Gpolygon;
QVector<QVector<QPointF>> GsplitLines;
QVector<QVector<QPointF>> GfilteredSplitLines;
QVector<QVector<QPointF>> GboundaryPointss;
QVector<QVector<QPointF>> Gpoints;
QVector<QVector<QPointF>> GsplitLines2;

void drawPolygons(QPainter& painter,
    const QVector<QVector<QPointF>>& polygons,
    const QString& title,
    int baseOffset)
{
    int cnt = 50;
    int pathIndex = 0; // 分图索引

    // 计算所有顶点的平均高度
    double totalHeight = 0;
    int totalPoints = 0;

    for (const auto& points : polygons)
    {
        for (const auto& point : points)
        {
            totalHeight += point.y();
            ++totalPoints;
        }
    }

    // 避免除零
    double averageHeight = (totalPoints > 0) ? (totalHeight / totalPoints) : 0;

    // 遍历所有多边形并绘制
    for (const auto& points : polygons)
    {
        painter.setBrush(Qt::NoBrush);

        // 设置线条颜色（根据 cnt 生成不同颜色）
        QColor lineColor = QColor::fromHsv(cnt % 255, 255, 255);
        lineColor.setAlpha(255); // 设置完全不透明
        QPen pen;
        pen.setColor(lineColor);
        painter.setPen(pen);

        QPainterPath singlePath; // 单条线的路径

        // 偏移向量
        QPointF offset(cnt * 6, baseOffset); // 固定偏移量，根据需求调整

        // 偏移后的路径
        QVector<QPointF> offsetPoints;
        for (const auto& point : points)
        {
            offsetPoints.append(point + offset);
        }

        // 绘制偏移后的路径
        singlePath.moveTo(offsetPoints.first());
        for (int i = 1; i < offsetPoints.size(); ++i)
        {
            singlePath.lineTo(offsetPoints[i]);
        }

        // 绘制路径
        painter.drawPath(singlePath);

        // 绘制起点和终点的标记（可选）
        painter.setBrush(Qt::red);
        painter.drawEllipse(offsetPoints.first(), 2.0, 2.0); // 起点
        painter.drawEllipse(offsetPoints.last(), 2.0, 2.0);  // 终点

        // 绘制每个点的坐标标记
        QFont font = painter.font();
        font.setPointSize(10); // 设置字体大小
        painter.setFont(font);

        painter.setPen(Qt::black);
        painter.drawText(
            offsetPoints.first(),
            QString("(%1, %2)").arg(QString::number(points.first().x(), 'f', 2),
                QString::number(points.first().y(), 'f', 2))
        );
        painter.drawText(
            offsetPoints.last(),
            QString("(%1, %2)").arg(QString::number(points.last().x(), 'f', 2),
                QString::number(points.last().y(), 'f', 2))
        );

        // 计算路径的中心点
        QPointF pathCenter(0, 0);
        for (const auto& point : offsetPoints)
        {
            pathCenter += point;
        }
        pathCenter /= offsetPoints.size(); // 取平均值

        // 在平均高度位置添加分图下标
        QString pathLabel = QString("%1 %2").arg(title).arg(pathIndex); // 构造路径下标
        QPointF labelPos(pathCenter.x(), averageHeight + baseOffset + 150);   // 使用平均高度作为标签位置
        painter.drawText(labelPos, pathLabel);

        // 更新颜色计数和路径索引
        cnt += 50;
        ++pathIndex;
    }

    if (polygons.size())
    {
        // 绘制叠加状态的图
        painter.setBrush(Qt::NoBrush);
        QPen overlayPen;
        overlayPen.setColor(Qt::blue); // 设置叠加图的线条颜色为蓝色
        overlayPen.setWidth(1);        // 设置线宽
        painter.setPen(overlayPen);

        QPointF offset(-300, baseOffset);

        QPainterPath overlayPath;
        for (const auto& points : polygons)
        {
            if (points.isEmpty())
                continue;

            overlayPath.moveTo(points.first() + offset);
            for (int i = 1; i < points.size(); ++i)
            {
                overlayPath.lineTo(points[i] + offset);
            }

            painter.setBrush(Qt::red);
            painter.drawEllipse(points.first() + offset, 2.0, 2.0); // 起点
            painter.drawEllipse(points.last() + offset, 2.0, 2.0);  // 终点
            painter.setBrush(Qt::NoBrush);
        }

        painter.drawPath(overlayPath);

        // 添加叠加图标题
        QFont font = painter.font();
        font.setPointSize(12); // 设置字体大小
        font.setBold(true);    // 设置加粗字体
        painter.setFont(font);

        QPointF overlayTitlePos(-200, averageHeight + baseOffset + 200); // 调整叠加标题位置
        painter.setPen(Qt::black);
        painter.drawText(overlayTitlePos, QString("%1").arg(title));
    }
}

#endif // DEBUG
