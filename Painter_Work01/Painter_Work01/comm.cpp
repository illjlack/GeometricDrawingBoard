#include "comm.h"

#include <QColor>
#include <QStatusBar>

DrawMode GlobalDrawMode;

NodeLineStyle GlobalNodeLineStyle;    // 节点线形
PointShape GlobalPointShape;           // 点形状
QColor GlobalPointColor;                 // 点颜色
LineStyle GlobalLineStyle;             // 线样式
float GlobalLineWidth;                 // 线宽
QColor GlobalLineColor;                  // 线颜色
float GlobalLineDashPattern;           // 虚线段长
QColor GlobalFillColor;                  // 填充颜色
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
QColor GlobalBufferLineColor;            // 缓冲区线颜色
float GlobalBufferLineDashPattern;     // 缓冲区虚线段长

// 缓冲区面的属性
QColor GlobalBufferFillColor;            // 缓冲区填充颜色

QStatusBar* GlobalStatusBar;            // 状态栏

void initializeGlobalDrawSettings()
{
    // 全局绘图模式初始化为选择模式
    GlobalDrawMode = DrawMode::DrawSelect;

    // 点形状、颜色初始化
    GlobalPointShape = Circle;                          // 默认点形状为圆
    GlobalPointColor = QColor(Qt::blue);                // 默认点颜色为蓝色

    // 线条属性初始化
    GlobalLineStyle = Solid;                            // 默认线条样式为实线
    GlobalLineWidth = 1.5f;                             // 默认线宽为 1.5
    GlobalLineColor = QColor(Qt::darkGray);             // 默认线条颜色为深灰色
    GlobalLineDashPattern = 0.0f;                       // 实线，无虚线模式

    // 多段线属性初始化
    GlobalNodeLineStyle = StylePolyline;                // 多段线样式初始化为折线
    GlobalFillColor = QColor(Qt::transparent);          // 默认填充颜色为透明

    // 样条曲线相关初始化
    GlobalSplineOrder = 3;                              // 样条曲线阶数
    GlobalSplineNodeCount = 500;                        // 默认样条节点数为 500
    GlobalSteps = 500;                                  // 细分步数设为 500

    // 缓冲区属性初始化
    GlobalBufferVisible = false;                                    // 默认不显示缓冲区
    GlobalBufferCalculationMode = BufferCalculationMode::Raster;    // 默认栅格模式
    GlobalBufferDistance = 50.0;                                    // 默认缓冲区距离设为 50.0

    // 缓冲区线的属性初始化
    GlobalBufferHasBorder = true;                       // 默认显示缓冲区边界
    GlobalBufferLineStyle = LineStyle::Dashed;          // 缓冲区边界线为虚线
    GlobalBufferLineWidth = 1.0f;                       // 边界线宽设为 1.0
    GlobalBufferLineColor = QColor(Qt::darkBlue);       // 缓冲区边界颜色为深蓝色
    GlobalBufferLineDashPattern = 3.0f;                 // 点状线段长度设为 3.0

    // 缓冲区填充颜色初始化
    GlobalBufferFillColor = QColor(200, 200, 255, 100); // 浅蓝色，半透明

    // 状态栏初始化
    GlobalStatusBar = new QStatusBar();                 // 创建状态栏实例
}

