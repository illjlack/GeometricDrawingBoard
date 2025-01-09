#include "comm.h"

#include <QColor>

DrawMode GlobalDrawMode;           // ����ģʽ���ߣ��棬������
NodeLineStyle GlobalNodeLineStyle; // �ڵ�����
PointShape GlobalPointShape;       // ����״
QRgb GlobalPointColor;             // ����ɫ
LineStyle GlobalLineStyle;         // ����ʽ
float GlobalLineWidth;             // �߿�
QRgb GlobalLineColor;              // ����ɫ
float GlobalLineDashPattern;       // ���߶γ�

QRgb GlobalFillColor;              // �����ɫ
int GlobalSplineOrder;             // ��������
int GlobalSplineNodeCount;         // �����ڵ���
int GlobalSteps;                   // ������ܶ�




void initializeGlobalDrawSettings() 
{
    GlobalDrawMode = DrawMode::None;
    GlobalPointShape = Circle;
    GlobalPointColor = QColor(Qt::black).rgba();
    GlobalLineStyle = Dashed;
    GlobalLineWidth = 2.0f;
    GlobalLineColor = QColor(Qt::black).rgba();
    GlobalLineDashPattern = 5.0f;
    GlobalNodeLineStyle = StylePolyline;
    GlobalFillColor = QColor(Qt::white).rgba();
    GlobalSplineOrder = 3;
    GlobalSplineNodeCount = 1000;
    GlobalSteps = 1000;
}