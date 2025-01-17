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
#include <QStatusBar>
// 全局变量声明
extern DrawMode GlobalDrawMode;           // 绘制模式（线，面，。。）

extern NodeLineStyle GlobalNodeLineStyle; // 节点线形
extern PointShape GlobalPointShape;       // 点形状
extern QColor GlobalPointColor;             // 点颜色
extern LineStyle GlobalLineStyle;         // 线样式
extern float GlobalLineWidth;             // 线宽
extern QColor GlobalLineColor;              // 线颜色
extern float GlobalLineDashPattern;       // 虚线段长
extern QColor GlobalFillColor;              // 填充颜色
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
extern QColor GlobalBufferLineColor;            // 缓冲区线颜色
extern float GlobalBufferLineDashPattern;     // 缓冲区虚线段长

// 缓冲区面的属性
extern QColor GlobalBufferFillColor;            // 缓冲区填充颜色


extern QStatusBar* GlobalStatusBar; // 状态栏

void initializeGlobalDrawSettings();




struct GeoParameters {
    // 成员变量
    NodeLineStyle nodeLineStyle;           // 节点线形
    PointShape pointShape;                 // 点形状
    QColor pointColor;                       // 点颜色
    LineStyle lineStyle;                   // 线样式
    float lineWidth;                       // 线宽
    QColor lineColor;                        // 线颜色
    float lineDashPattern;                 // 虚线段长
    QColor fillColor;                        // 填充颜色
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
    QColor bufferLineColor;                  // 缓冲区线颜色
    float bufferLineDashPattern;           // 缓冲区虚线段长

    // 缓冲区面的属性
    QColor bufferFillColor;                  // 缓冲区填充颜色

    // 初始化成员变量
    inline void initGeoParameters()
    {
        // 设置为全局默认值
        nodeLineStyle = GlobalNodeLineStyle;           // 节点线形
        pointShape = GlobalPointShape;                 // 点形状
        pointColor = GlobalPointColor;                 // 点颜色
        lineStyle = GlobalLineStyle;                   // 线样式
        lineWidth = GlobalLineWidth;                   // 线宽
        lineColor = GlobalLineColor;                   // 线颜色
        lineDashPattern = GlobalLineDashPattern;       // 虚线段长
        fillColor = GlobalFillColor;                   // 填充颜色
        splineOrder = GlobalSplineOrder;               // 样条阶数
        splineNodeCount = GlobalSplineNodeCount;       // 样条节点数
        steps = GlobalSteps;                           // 计算点密度

        bufferVisible = GlobalBufferVisible;           // 是否显示缓冲区
        bufferCalculationMode = GlobalBufferCalculationMode; // 缓冲区计算方式
        bufferDistance = GlobalBufferDistance;         // 缓冲区距离

        // 缓冲区线的属性
        bufferHasBorder = GlobalBufferHasBorder;       // 缓冲区是否有边界
        bufferLineStyle = GlobalBufferLineStyle;       // 缓冲区线样式
        bufferLineWidth = GlobalBufferLineWidth;       // 缓冲区线宽
        bufferLineColor = GlobalBufferLineColor;       // 缓冲区线颜色
        bufferLineDashPattern = GlobalBufferLineDashPattern; // 缓冲区虚线段长

        // 缓冲区面的属性
        bufferFillColor = GlobalBufferFillColor;       // 缓冲区填充颜色
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
#include <cmath> // 用于数学计算

void drawPolygons(QPainter& painter,
    const QVector<QVector<QPointF>>& polygons,
    const QString& title,
    int baseOffset);

#endif // DEBUG
