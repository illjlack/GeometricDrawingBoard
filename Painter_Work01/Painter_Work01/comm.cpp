#include "comm.h"

#include <QColor>

DrawMode GlobalDrawMode;           // 绘制模式（线，面，。。）
NodeLineStyle GlobalNodeLineStyle; // 节点线形
PointShape GlobalPointShape;       // 点形状
QRgb GlobalPointColor;             // 点颜色
LineStyle GlobalLineStyle;         // 线样式
float GlobalLineWidth;             // 线宽
QRgb GlobalLineColor;              // 线颜色
float GlobalLineDashPattern;       // 虚线段长

QRgb GlobalFillColor;              // 填充颜色
int GlobalSplineOrder;             // 样条阶数
int GlobalSplineNodeCount;         // 样条节点数
int GlobalSteps;                   // 计算点密度




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