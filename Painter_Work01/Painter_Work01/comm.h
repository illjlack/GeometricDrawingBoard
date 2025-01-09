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


void initializeGlobalDrawSettings();