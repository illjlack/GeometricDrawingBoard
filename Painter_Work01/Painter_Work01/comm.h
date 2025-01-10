#pragma once

// ����

#include <QTextCodec>

inline QString L(const char* str) {
    static QTextCodec* codec = QTextCodec::codecForName("GBK");
    return codec->toUnicode(str);
}


// ��־


#include <QDebug>

#define STR(x) # x

#define Log(str) \
    qDebug() << "Log:" << L(str) << ", file:" << __FILE__ << ", line:" << __LINE__;



// ����
#include "Enums.h"
#include <QRgb>

// ȫ�ֱ�������
extern DrawMode GlobalDrawMode;           // ����ģʽ���ߣ��棬������

extern NodeLineStyle GlobalNodeLineStyle; // �ڵ�����
extern PointShape GlobalPointShape;       // ����״
extern QRgb GlobalPointColor;             // ����ɫ
extern LineStyle GlobalLineStyle;         // ����ʽ
extern float GlobalLineWidth;             // �߿�
extern QRgb GlobalLineColor;              // ����ɫ
extern float GlobalLineDashPattern;       // ���߶γ�
extern QRgb GlobalFillColor;              // �����ɫ
extern int GlobalSplineOrder;             // ��������
extern int GlobalSplineNodeCount;         // �����ڵ���
extern int GlobalSteps;                   // ������ܶ�

extern bool GlobalBufferVisible;             // �Ƿ���ʾ������
extern BufferCalculationMode GlobalBufferCalculationMode; // ���������㷽ʽ��ʸ����դ��
extern double GlobalBufferDistance;       // ����������


// �������ߵ�����
extern bool GlobalBufferHasBorder;            // �������Ƿ��б߿�
extern LineStyle GlobalBufferLineStyle;       // ����������ʽ
extern float GlobalBufferLineWidth;           // �������߿�
extern QRgb GlobalBufferLineColor;            // ����������ɫ
extern float GlobalBufferLineDashPattern;     // ���������߶γ�

// �������������
extern QRgb GlobalBufferFillColor;            // �����������ɫ


void initializeGlobalDrawSettings();




struct GeoParameters {
    // ��Ա����
    NodeLineStyle nodeLineStyle;           // �ڵ�����
    PointShape pointShape;                 // ����״
    QRgb pointColor;                       // ����ɫ
    LineStyle lineStyle;                   // ����ʽ
    float lineWidth;                       // �߿�
    QRgb lineColor;                        // ����ɫ
    float lineDashPattern;                 // ���߶γ�
    QRgb fillColor;                        // �����ɫ
    int splineOrder;                       // ��������
    int splineNodeCount;                   // �����ڵ���
    int steps;                             // ������ܶ�

    bool bufferVisible;                    // �Ƿ���ʾ������
    BufferCalculationMode bufferCalculationMode; // ���������㷽ʽ
    double bufferDistance;                 // ����������

    // �������ߵ�����
    bool bufferHasBorder;
    LineStyle bufferLineStyle;             // ����������ʽ
    float bufferLineWidth;                 // �������߿�
    QRgb bufferLineColor;                  // ����������ɫ
    float bufferLineDashPattern;           // ���������߶γ�

    // �������������
    QRgb bufferFillColor;                  // �����������ɫ

    // ��ʼ����Ա����
    inline void initGeoParameters()
    {
        nodeLineStyle = GlobalNodeLineStyle;
        pointShape = GlobalPointShape;
        pointColor = GlobalPointColor;
        lineStyle = GlobalLineStyle;
        lineWidth = GlobalLineWidth;
        lineColor = GlobalLineColor;
        lineDashPattern = GlobalLineDashPattern;
        fillColor = GlobalFillColor;
        splineOrder = GlobalSplineOrder;
        splineNodeCount = GlobalSplineNodeCount;
        steps = GlobalSteps;

        bufferVisible = GlobalBufferVisible;
        bufferCalculationMode = GlobalBufferCalculationMode;
        bufferDistance = GlobalBufferDistance;

        // �������ߵ�Ĭ��ֵ
        bufferLineStyle = GlobalLineStyle;
        bufferLineWidth = GlobalLineWidth;
        bufferLineColor = GlobalLineColor;
        bufferLineDashPattern = GlobalLineDashPattern;

        // ���������Ĭ��ֵ
        bufferFillColor = GlobalFillColor;

        bufferHasBorder = GlobalBufferHasBorder;
    }
};
