#include "comm.h"

#include <QColor>

DrawMode GlobalDrawMode;

NodeLineStyle GlobalNodeLineStyle;    // 节点线形
PointShape GlobalPointShape;           // 点形状
QRgb GlobalPointColor;                 // 点颜色
LineStyle GlobalLineStyle;             // 线样式
float GlobalLineWidth;                 // 线宽
QRgb GlobalLineColor;                  // 线颜色
float GlobalLineDashPattern;           // 虚线段长
QRgb GlobalFillColor;                  // 填充颜色
int GlobalSplineOrder;                 // 样条阶数
int GlobalSplineNodeCount;             // 样条节点数
int GlobalSteps;                       // 计算点密度

bool GlobalBufferVisible;              // 是否显示缓冲区
BufferCalculationMode GlobalBufferCalculationMode; // 缓冲区计算方式（矢量、栅格）
double GlobalBufferDistance;           // 缓冲区距离

// 缓冲区线的属性
bool GlobalBufferHasBorder;
LineStyle GlobalBufferLineStyle;       // 缓冲区线样式
float GlobalBufferLineWidth;           // 缓冲区线宽
QRgb GlobalBufferLineColor;            // 缓冲区线颜色
float GlobalBufferLineDashPattern;     // 缓冲区虚线段长

// 缓冲区面的属性
QRgb GlobalBufferFillColor;            // 缓冲区填充颜色

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

    GlobalBufferVisible = true;                          // 默认显示缓冲区
    GlobalBufferCalculationMode = BufferCalculationMode::Vector; // 默认矢量模式
    GlobalBufferDistance = 10.0;                         // 默认缓冲区距离为 10.0

    // 缓冲区线的属性初始化
    GlobalBufferHasBorder = true;
    GlobalBufferLineStyle = Dashed;                  // 缓冲区线样式（假设是虚线）
    GlobalBufferLineWidth = 1.0f;                    // 缓冲区线宽（默认 1.0）
    GlobalBufferLineColor = QColor(Qt::gray).rgba(); // 缓冲区线颜色（默认灰色）
    GlobalBufferLineDashPattern = 5.0f;              // 缓冲区虚线段长

    // 缓冲区面的填充颜色初始化
    GlobalBufferFillColor = QColor(Qt::lightGray).rgba(); // 缓冲区填充颜色（默认浅灰色）
}
