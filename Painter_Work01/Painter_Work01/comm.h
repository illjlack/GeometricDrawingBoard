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


void initializeGlobalDrawSettings();



// ��Щ��ͼ����Ҫ�Ĳ�����Ϊ�˷������ã�ÿ��ͼ�ζ���ȫ��
struct GeoParameters {
    // ��Ա����
    NodeLineStyle nodeLineStyle;   // �ڵ�����
    PointShape pointShape;         // ����״
    QRgb pointColor;               // ����ɫ
    LineStyle lineStyle;           // ����ʽ
    float lineWidth;               // �߿�
    QRgb lineColor;                // ����ɫ
    float lineDashPattern;         // ���߶γ�
    QRgb fillColor;                // �����ɫ
    int splineOrder;               // ��������
    int splineNodeCount;           // �����ڵ���
    int steps;                     // ������ܶ�

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
    }
};
