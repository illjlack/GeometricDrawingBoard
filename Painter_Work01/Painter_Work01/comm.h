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
#include <QStatusBar>
// ȫ�ֱ�������
extern DrawMode GlobalDrawMode;           // ����ģʽ���ߣ��棬������

extern NodeLineStyle GlobalNodeLineStyle; // �ڵ�����
extern PointShape GlobalPointShape;       // ����״
extern QColor GlobalPointColor;             // ����ɫ
extern LineStyle GlobalLineStyle;         // ����ʽ
extern float GlobalLineWidth;             // �߿�
extern QColor GlobalLineColor;              // ����ɫ
extern float GlobalLineDashPattern;       // ���߶γ�
extern QColor GlobalFillColor;              // �����ɫ
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
extern QColor GlobalBufferLineColor;            // ����������ɫ
extern float GlobalBufferLineDashPattern;     // ���������߶γ�

// �������������
extern QColor GlobalBufferFillColor;            // �����������ɫ


extern QStatusBar* GlobalStatusBar; // ״̬��

void initializeGlobalDrawSettings();




struct GeoParameters {
    // ��Ա����
    NodeLineStyle nodeLineStyle;           // �ڵ�����
    PointShape pointShape;                 // ����״
    QColor pointColor;                       // ����ɫ
    LineStyle lineStyle;                   // ����ʽ
    float lineWidth;                       // �߿�
    QColor lineColor;                        // ����ɫ
    float lineDashPattern;                 // ���߶γ�
    QColor fillColor;                        // �����ɫ
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
    QColor bufferLineColor;                  // ����������ɫ
    float bufferLineDashPattern;           // ���������߶γ�

    // �������������
    QColor bufferFillColor;                  // �����������ɫ

    // ��ʼ����Ա����
    inline void initGeoParameters()
    {
        // ����Ϊȫ��Ĭ��ֵ
        nodeLineStyle = GlobalNodeLineStyle;           // �ڵ�����
        pointShape = GlobalPointShape;                 // ����״
        pointColor = GlobalPointColor;                 // ����ɫ
        lineStyle = GlobalLineStyle;                   // ����ʽ
        lineWidth = GlobalLineWidth;                   // �߿�
        lineColor = GlobalLineColor;                   // ����ɫ
        lineDashPattern = GlobalLineDashPattern;       // ���߶γ�
        fillColor = GlobalFillColor;                   // �����ɫ
        splineOrder = GlobalSplineOrder;               // ��������
        splineNodeCount = GlobalSplineNodeCount;       // �����ڵ���
        steps = GlobalSteps;                           // ������ܶ�

        bufferVisible = GlobalBufferVisible;           // �Ƿ���ʾ������
        bufferCalculationMode = GlobalBufferCalculationMode; // ���������㷽ʽ
        bufferDistance = GlobalBufferDistance;         // ����������

        // �������ߵ�����
        bufferHasBorder = GlobalBufferHasBorder;       // �������Ƿ��б߽�
        bufferLineStyle = GlobalBufferLineStyle;       // ����������ʽ
        bufferLineWidth = GlobalBufferLineWidth;       // �������߿�
        bufferLineColor = GlobalBufferLineColor;       // ����������ɫ
        bufferLineDashPattern = GlobalBufferLineDashPattern; // ���������߶γ�

        // �������������
        bufferFillColor = GlobalBufferFillColor;       // �����������ɫ
    }
};



// debug
// #define DEBUG


#ifdef DEBUG
#include <QVector>
#include <QPointF>
extern QVector<QVector<QPointF>> Gpolygon;
extern QVector<QVector<QPointF>> Gpoints;
extern QVector<QVector<QPointF>> GsplitLines;
extern QVector<QVector<QPointF>> GfilteredSplitLines;
extern QVector<QVector<QPointF>> GboundaryPointss;
extern QVector<QVector<QPointF>> GsplitLines2;
#include <QPainter>
#include <QPainterPath>
#include <QVector>
#include <QPointF>
#include <QString>
#include <QColor>
#include <QPen>
#include <QFont>
#include <cmath> // ������ѧ����

void drawPolygons(QPainter& painter,
    const QVector<QVector<QPointF>>& polygons,
    const QString& title,
    int baseOffset);

#endif // DEBUG
