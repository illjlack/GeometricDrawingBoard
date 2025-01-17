#include "comm.h"

#include <QColor>
#include <QStatusBar>

DrawMode GlobalDrawMode;

NodeLineStyle GlobalNodeLineStyle;    // �ڵ�����
PointShape GlobalPointShape;           // ����״
QColor GlobalPointColor;                 // ����ɫ
LineStyle GlobalLineStyle;             // ����ʽ
float GlobalLineWidth;                 // �߿�
QColor GlobalLineColor;                  // ����ɫ
float GlobalLineDashPattern;           // ���߶γ�
QColor GlobalFillColor;                  // �����ɫ
int GlobalSplineOrder;                 // ��������
int GlobalSplineNodeCount;             // �����ڵ���
int GlobalSteps;                       // ������ܶ�

bool GlobalBufferVisible;              // �Ƿ���ʾ������
BufferCalculationMode GlobalBufferCalculationMode; // ���������㷽ʽ��ʸ����դ��
double GlobalBufferDistance;           // ����������

// �������ߵ�����
bool GlobalBufferHasBorder;
LineStyle GlobalBufferLineStyle;       // ����������ʽ
float GlobalBufferLineWidth;           // �������߿�
QColor GlobalBufferLineColor;            // ����������ɫ
float GlobalBufferLineDashPattern;     // ���������߶γ�

// �������������
QColor GlobalBufferFillColor;            // �����������ɫ

QStatusBar* GlobalStatusBar;            // ״̬��

void initializeGlobalDrawSettings()
{
    // ȫ�ֻ�ͼģʽ��ʼ��Ϊѡ��ģʽ
    GlobalDrawMode = DrawMode::DrawSelect;

    // ����״����ɫ��ʼ��
    GlobalPointShape = Circle;                          // Ĭ�ϵ���״ΪԲ
    GlobalPointColor = QColor(Qt::blue);                // Ĭ�ϵ���ɫΪ��ɫ

    // �������Գ�ʼ��
    GlobalLineStyle = Solid;                            // Ĭ��������ʽΪʵ��
    GlobalLineWidth = 1.5f;                             // Ĭ���߿�Ϊ 1.5
    GlobalLineColor = QColor(Qt::darkGray);             // Ĭ��������ɫΪ���ɫ
    GlobalLineDashPattern = 0.0f;                       // ʵ�ߣ�������ģʽ

    // ��������Գ�ʼ��
    GlobalNodeLineStyle = StylePolyline;                // �������ʽ��ʼ��Ϊ����
    GlobalFillColor = QColor(Qt::transparent);          // Ĭ�������ɫΪ͸��

    // ����������س�ʼ��
    GlobalSplineOrder = 3;                              // �������߽���
    GlobalSplineNodeCount = 20;                        // Ĭ�������ڵ���Ϊ 500
    GlobalSteps = 20;                                  // ϸ�ֲ�����Ϊ 500

    // ���������Գ�ʼ��
    GlobalBufferVisible = false;                                    // Ĭ�ϲ���ʾ������
    GlobalBufferCalculationMode = BufferCalculationMode::Vector;    // Ĭ��դ��ģʽ
    GlobalBufferDistance = 50.0;                                    // Ĭ�ϻ�����������Ϊ 50.0

    // �������ߵ����Գ�ʼ��
    GlobalBufferHasBorder = true;                       // Ĭ����ʾ�������߽�
    GlobalBufferLineStyle = LineStyle::Dashed;          // �������߽���Ϊ����
    GlobalBufferLineWidth = 1.0f;                       // �߽��߿���Ϊ 1.0
    GlobalBufferLineColor = QColor(Qt::darkBlue);       // �������߽���ɫΪ����ɫ
    GlobalBufferLineDashPattern = 3.0f;                 // ��״�߶γ�����Ϊ 3.0

    // �����������ɫ��ʼ��
    GlobalBufferFillColor = QColor(200, 200, 255, 100); // ǳ��ɫ����͸��

    // ״̬����ʼ��
    GlobalStatusBar = new QStatusBar();                 // ����״̬��ʵ��
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
    int pathIndex = 0; // ��ͼ����

    // �������ж����ƽ���߶�
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

    // �������
    double averageHeight = (totalPoints > 0) ? (totalHeight / totalPoints) : 0;

    // �������ж���β�����
    for (const auto& points : polygons)
    {
        painter.setBrush(Qt::NoBrush);

        // ����������ɫ������ cnt ���ɲ�ͬ��ɫ��
        QColor lineColor = QColor::fromHsv(cnt % 255, 255, 255);
        lineColor.setAlpha(255); // ������ȫ��͸��
        QPen pen;
        pen.setColor(lineColor);
        painter.setPen(pen);

        QPainterPath singlePath; // �����ߵ�·��

        // ƫ������
        QPointF offset(cnt * 6, baseOffset); // �̶�ƫ�����������������

        // ƫ�ƺ��·��
        QVector<QPointF> offsetPoints;
        for (const auto& point : points)
        {
            offsetPoints.append(point + offset);
        }

        // ����ƫ�ƺ��·��
        singlePath.moveTo(offsetPoints.first());
        for (int i = 1; i < offsetPoints.size(); ++i)
        {
            singlePath.lineTo(offsetPoints[i]);
        }

        // ����·��
        painter.drawPath(singlePath);

        // ���������յ�ı�ǣ���ѡ��
        painter.setBrush(Qt::red);
        painter.drawEllipse(offsetPoints.first(), 2.0, 2.0); // ���
        painter.drawEllipse(offsetPoints.last(), 2.0, 2.0);  // �յ�

        // ����ÿ�����������
        QFont font = painter.font();
        font.setPointSize(10); // ���������С
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

        // ����·�������ĵ�
        QPointF pathCenter(0, 0);
        for (const auto& point : offsetPoints)
        {
            pathCenter += point;
        }
        pathCenter /= offsetPoints.size(); // ȡƽ��ֵ

        // ��ƽ���߶�λ����ӷ�ͼ�±�
        QString pathLabel = QString("%1 %2").arg(title).arg(pathIndex); // ����·���±�
        QPointF labelPos(pathCenter.x(), averageHeight + baseOffset + 150);   // ʹ��ƽ���߶���Ϊ��ǩλ��
        painter.drawText(labelPos, pathLabel);

        // ������ɫ������·������
        cnt += 50;
        ++pathIndex;
    }

    if (polygons.size())
    {
        // ���Ƶ���״̬��ͼ
        painter.setBrush(Qt::NoBrush);
        QPen overlayPen;
        overlayPen.setColor(Qt::blue); // ���õ���ͼ��������ɫΪ��ɫ
        overlayPen.setWidth(1);        // �����߿�
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
            painter.drawEllipse(points.first() + offset, 2.0, 2.0); // ���
            painter.drawEllipse(points.last() + offset, 2.0, 2.0);  // �յ�
            painter.setBrush(Qt::NoBrush);
        }

        painter.drawPath(overlayPath);

        // ��ӵ���ͼ����
        QFont font = painter.font();
        font.setPointSize(12); // ���������С
        font.setBold(true);    // ���üӴ�����
        painter.setFont(font);

        QPointF overlayTitlePos(-200, averageHeight + baseOffset + 200); // �������ӱ���λ��
        painter.setPen(Qt::black);
        painter.drawText(overlayTitlePos, QString("%1").arg(title));
    }
}

#endif // DEBUG
