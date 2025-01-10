#pragma once

// 中文

#include <QTextCodec>

inline QString L(const char* str) {
    static QTextCodec* codec = QTextCodec::codecForName("GBK");
    return codec->toUnicode(str);
}


// 日志


#include <QDebug>

#define STR(x) # x

#define Log(str) \
    qDebug() << "Log:" << L(str) << ", file:" << __FILE__ << ", line:" << __LINE__;



// 配置
#include "Enums.h"
#include <QRgb>

// 全局变量声明
extern DrawMode GlobalDrawMode;           // 绘制模式（线，面，。。）

extern NodeLineStyle GlobalNodeLineStyle; // 节点线形
extern PointShape GlobalPointShape;       // 点形状
extern QRgb GlobalPointColor;             // 点颜色
extern LineStyle GlobalLineStyle;         // 线样式
extern float GlobalLineWidth;             // 线宽
extern QRgb GlobalLineColor;              // 线颜色
extern float GlobalLineDashPattern;       // 虚线段长
extern QRgb GlobalFillColor;              // 填充颜色
extern int GlobalSplineOrder;             // 样条阶数
extern int GlobalSplineNodeCount;         // 样条节点数
extern int GlobalSteps;                   // 计算点密度

extern bool GlobalBufferVisible;             // 是否显示缓冲区
extern BufferCalculationMode GlobalBufferCalculationMode; // 缓冲区计算方式（矢量、栅格）
extern double GlobalBufferDistance;       // 缓冲区距离


// 缓冲区线的属性
extern bool GlobalBufferHasBorder;            // 缓冲区是否有边框
extern LineStyle GlobalBufferLineStyle;       // 缓冲区线样式
extern float GlobalBufferLineWidth;           // 缓冲区线宽
extern QRgb GlobalBufferLineColor;            // 缓冲区线颜色
extern float GlobalBufferLineDashPattern;     // 缓冲区虚线段长

// 缓冲区面的属性
extern QRgb GlobalBufferFillColor;            // 缓冲区填充颜色


void initializeGlobalDrawSettings();




struct GeoParameters {
    // 成员变量
    NodeLineStyle nodeLineStyle;           // 节点线形
    PointShape pointShape;                 // 点形状
    QRgb pointColor;                       // 点颜色
    LineStyle lineStyle;                   // 线样式
    float lineWidth;                       // 线宽
    QRgb lineColor;                        // 线颜色
    float lineDashPattern;                 // 虚线段长
    QRgb fillColor;                        // 填充颜色
    int splineOrder;                       // 样条阶数
    int splineNodeCount;                   // 样条节点数
    int steps;                             // 计算点密度

    bool bufferVisible;                    // 是否显示缓冲区
    BufferCalculationMode bufferCalculationMode; // 缓冲区计算方式
    double bufferDistance;                 // 缓冲区距离

    // 缓冲区线的属性
    bool bufferHasBorder;
    LineStyle bufferLineStyle;             // 缓冲区线样式
    float bufferLineWidth;                 // 缓冲区线宽
    QRgb bufferLineColor;                  // 缓冲区线颜色
    float bufferLineDashPattern;           // 缓冲区虚线段长

    // 缓冲区面的属性
    QRgb bufferFillColor;                  // 缓冲区填充颜色

    // 初始化成员变量
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

        // 缓冲区线的默认值
        bufferLineStyle = GlobalLineStyle;
        bufferLineWidth = GlobalLineWidth;
        bufferLineColor = GlobalLineColor;
        bufferLineDashPattern = GlobalLineDashPattern;

        // 缓冲区面的默认值
        bufferFillColor = GlobalFillColor;

        bufferHasBorder = GlobalBufferHasBorder;
    }
};
