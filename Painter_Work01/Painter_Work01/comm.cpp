#include "comm.h"

#include <QColor>

DrawMode GlobalDrawMode;

NodeLineStyle GlobalNodeLineStyle;    // �ڵ�����
PointShape GlobalPointShape;           // ����״
QRgb GlobalPointColor;                 // ����ɫ
LineStyle GlobalLineStyle;             // ����ʽ
float GlobalLineWidth;                 // �߿�
QRgb GlobalLineColor;                  // ����ɫ
float GlobalLineDashPattern;           // ���߶γ�
QRgb GlobalFillColor;                  // �����ɫ
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
QRgb GlobalBufferLineColor;            // ����������ɫ
float GlobalBufferLineDashPattern;     // ���������߶γ�

// �������������
QRgb GlobalBufferFillColor;            // �����������ɫ

void initializeGlobalDrawSettings()
{
    GlobalDrawMode = DrawMode::DrawSelect;
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

    GlobalBufferVisible = true;                          // Ĭ����ʾ������
    GlobalBufferCalculationMode = BufferCalculationMode::Vector; // Ĭ��ʸ��ģʽ
    GlobalBufferDistance = 10.0;                         // Ĭ�ϻ���������Ϊ 10.0

    // �������ߵ����Գ�ʼ��
    GlobalBufferHasBorder = true;
    GlobalBufferLineStyle = Dashed;                  // ����������ʽ�����������ߣ�
    GlobalBufferLineWidth = 1.0f;                    // �������߿�Ĭ�� 1.0��
    GlobalBufferLineColor = QColor(Qt::gray).rgba(); // ����������ɫ��Ĭ�ϻ�ɫ��
    GlobalBufferLineDashPattern = 5.0f;              // ���������߶γ�

    // ��������������ɫ��ʼ��
    GlobalBufferFillColor = QColor(Qt::lightGray).rgba(); // �����������ɫ��Ĭ��ǳ��ɫ��
}
