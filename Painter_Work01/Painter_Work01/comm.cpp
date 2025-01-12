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
    GlobalSplineNodeCount = 500;                        // Ĭ�������ڵ���Ϊ 500
    GlobalSteps = 500;                                  // ϸ�ֲ�����Ϊ 500

    // ���������Գ�ʼ��
    GlobalBufferVisible = false;                                    // Ĭ�ϲ���ʾ������
    GlobalBufferCalculationMode = BufferCalculationMode::Raster;    // Ĭ��դ��ģʽ
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

