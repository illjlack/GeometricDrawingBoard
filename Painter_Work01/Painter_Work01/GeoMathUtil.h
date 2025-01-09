#pragma once

#include <QPointF>
#include <QVector>
#define M_PI 3.14159265358979323846
#include "Enums.h"

#include <QRectF>

// ==========================================================================
// 计算线段上的点
// 对外的接口， 统一分配计算
// ==========================================================================

/**
* 能代替尽量不失败绘制
* 除了统一的接口，其他的都不清理传出参数
*/

/**
 * 根据节点类型和控制点计算线上的点
 * @param nodeLineStyle 线的类型（例如折线、样条、三点圆弧等）
 * @param controlPoints 控制点列表，用于计算线的形状
 * @param linePoints 输出参数，保存计算得到的线段上的点
 * @param steps 步数，决定计算多少个中间点
 * @return 如果成功计算则返回 true，失败则返回 false
 */
bool calculateLinePoints(NodeLineStyle nodeLineStyle, const QVector<QPointF>& controlPoints, QVector<QPointF>& linePoints, int steps = 200);

/**
 * 根据节点类型和控制点计算闭合线上的点（多做了一下闭合）
 * @param lineStyle 线的类型
 * @param controlPoints 控制点列表
 * @param linePoints 输出参数，保存计算得到的闭合线上的点
 * @param steps 步数，决定计算多少个中间点
 * @return 如果成功计算则返回 true，失败则返回 false
 */
bool calculateCloseLinePoints(NodeLineStyle lineStyle, const QVector<QPointF>& controlPoints, QVector<QPointF>& linePoints, int steps = 200);

// 多个线一起计算的结构体
struct Component {
    int len;
    NodeLineStyle nodeLineStyle;
};

/**
 * 根据多个线段的组件和控制点计算线上的点
 * @param component 组件列表
 * @param controlPoints 控制点列表
 * @param linePointss 输出参数，保存计算得到的线段上的点
 * @param steps 步数，决定计算多少个中间点
 * @return 返回成功绘制分图的个数
 */
int calculateLinePoints(const QVector<Component>& component, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps = 200);

/**
 * 根据多个线段的组件和控制点计算闭合线上的点
 * @param component 组件列表
 * @param controlPoints 控制点列表
 * @param linePointss 输出参数，保存计算得到的闭合线上的点
 * @param steps 步数，决定计算多少个中间点
 * @return 返回成功绘制分图的个数
 */
int calculateCloseLinePoints(const QVector<Component>& component, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps = 200);


/**
 * 根据多个线段的组件和控制点计算平行线（第二个分离一个点来控制距离）
 * @param component 组件列表
 * @param controlPoints 控制点列表
 * @param linePointss 输出参数，保存计算得到的闭合线上的点
 * @param steps 步数，决定计算多少个中间点
 * @return 返回成功绘制分图的个数
 */
int calculateParallelLinePoints(const QVector<Component>& components, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps = 200);

// ==========================================================================
// 样条计算
// 贝塞尔曲线 和 B样条
// ==========================================================================

/**
 * 计算组合数（n 选 i），即计算 C(n, i)
 * @param n 总数
 * @param i 选择数
 * @return 计算出的组合数值
 */
int binomialCoefficient(int n, int i);

/**
 * 计算n次贝塞尔曲线的点（二维）
 * @param controlPoints 贝塞尔曲线的控制点列表
 * @param t 插值因子，通常在[0, 1]之间变化
 * @return 曲线上的一个点
 */
QPointF bezierN(const QVector<QPointF>& controlPoints, double t);

/**
 * 计算n次贝塞尔曲线上的多个点（二维）
 * @param controlPoints 贝塞尔曲线的控制点列表
 * @param numPoints 计算多少个曲线点
 * @return 包含所有计算得到的贝塞尔曲线点的列表
 */
QVector<QPointF> calculateBezierCurve(const QVector<QPointF>& controlPoints, int numPoints);

/**
 * Cox-de Boor 递归公式，用于计算 B 样条曲线上的点
 * @param knots 节点向量
 * @param i 当前节点的索引
 * @param p B 样条的次数
 * @param t 插值因子
 * @return 当前插值位置的权重
 */
double coxDeBoor(const QVector<double>& knots, int i, int p, double t);

/**
 * 生成 B 样条曲线的节点向量
 * @param n 控制点数
 * @param degree 曲线的次数
 * @return 生成的节点向量
 */
QVector<double> generateKnotVector(int n, int degree);

/**
 * 计算 B 样条曲线上的多个点（二维）
 * @param controlPoints 控制点列表
 * @param degree B 样条的次数
 * @param numCurvePoints 要计算的曲线点数
 * @param curvePoints 输出参数，保存计算得到的曲线点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateBSplineCurve(const QVector<QPointF>& controlPoints, int degree, int numCurvePoints, QVector<QPointF>& curvePoints);

// ==========================================================================
// 圆弧计算
// ==========================================================================

/**
 * 规范化角度，确保角度在[0, 2π)范围内
 * @param angle 输入角度（弧度制）
 * @return 规范化后的角度（弧度制）
 */
double normalizeAngle(double angle);

/**
 * 根据控制点计算三点圆弧的点
 * @param controlPoints 控制点列表
 * @param steps 步数，决定计算多少个点
 * @param arcPoints 输出参数，保存计算得到的弧线上的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateThreeArcPointsFromControlPoints(const QVector<QPointF>& controlPoints, int steps, QVector<QPointF>& arcPoints);

/**
 * 根据控制点计算圆弧的点
 * @param controlPoints 控制点列表
 * @param steps 步数，决定计算多少个点
 * @param arcPoints 输出参数，保存计算得到的弧线上的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateArcPointsFromControlPoints(const QVector<QPointF>& controlPoints, int steps, QVector<QPointF>& arcPoints);

/**
 * 根据三点计算弧线上的点
 * @param point1 圆上的第一个点
 * @param point2 圆上的第二个点
 * @param point3 圆上的第三个点
 * @param steps 步数，决定计算多少个点
 * @param arcPoints 输出参数，保存计算得到的弧线上的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateArcPointsFromThreePoints(const QPointF& point1, const QPointF& point2, const QPointF& point3, int steps, QVector<QPointF>& arcPoints);

/**
 * 根据两个点计算圆上的点
 * @param point1 圆上的第一个点
 * @param point2 圆上的第二个点
 * @param steps 步数，决定计算多少个点
 * @param arcPoints 输出参数，保存计算得到的圆上的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateCirclePointsFromControlPoints(const QPointF& point1, const QPointF& point2, int steps, QVector<QPointF>& arcPoints);

/**
 * 计算通过三点确定的圆的圆心和半径
 * @param p1 圆上的第一个点
 * @param p2 圆上的第二个点
 * @param p3 圆上的第三个点
 * @param center 输出参数，计算得到的圆心坐标
 * @param radius 输出参数，计算得到的圆的半径
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateCircle(const QPointF& p1, const QPointF& p2, const QPointF& p3, QPointF& center, double& radius);

/**
 * 计算通过两个点确定的圆的圆心和半径
 * @param p1 圆上的第一个点
 * @param p2 圆上的第二个点
 * @param center 输出参数，计算得到的圆心坐标
 * @param radius 输出参数，计算得到的圆的半径
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateCircle(const QPointF& p1, const QPointF& p2, QPointF& center, double& radius);

/**
 * 根据圆心、半径、起始角度、角度差值和步数计算弧线上的点
 * @param center 圆心坐标
 * @param radius 圆的半径
 * @param startAngle 起始角度（弧度制）
 * @param angleDiff 角度差值（弧度制）
 * @param steps 步数，决定计算多少个点
 * @param points 输出参数，保存计算得到的弧线上的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateArcPoints(const QPointF& center, double radius, double startAngle, double angleDiff, int steps, QVector<QPointF>& points);

// ==========================================================================
// 平行线计算
// ==========================================================================
// 参考：https://zhuanlan.zhihu.com/p/536948720

// 精度常量
const double EPS = 1e-8;

// 精度处理
int sgn(double x);
// 向量标准化
std::pair<double, double> normalize(double x, double y);

// 向量叉乘
double cross(double x1, double y1, double x2, double y2);

/**
 * 计算折线的双边平行线
 * @param polyline 输入折线的点列表
 * @param dis 平行线与折线的距离
 * @param leftPolyline 输出参数，保存平行线的左侧点
 * @param rightPolyline 输出参数，保存平行线的右侧点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateParallelLine(const QVector<QPointF>& polyline, double dis, QVector<QPointF>& leftPolyline, QVector<QPointF>& rightPolyline);

/**
 * 计算折线的单边平行线
 * @param polyline 输入折线的点列表
 * @param dis 平行线与折线的距离
 * @param parallelPolyline 输出参数，保存单边平行线的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateParallelLine(const QVector<QPointF>& polyline, double dis, QVector<QPointF>& parallelPolyline);


/**
 * 计算点到直线的垂直距离，并返回带方向的结果
 * @param point 点
 * @param lineStart 直线的起点
 * @param lineEnd 直线的终点
 * @param direction 返回值，用于表示点在直线的哪一侧，正值为左侧，负值为右侧，0为直线上
 * @return 点到直线的垂直距离
 */
double pointToLineDistanceWithDirection(const QPointF& point, const QPointF& lineStart, const QPointF& lineEnd);

/**
 * 计算平行折线，且平行线经过指定的点
 * @param polyline 输入折线的点列表
 * @param targetPoint 指定目标点，平行线将通过该点
 * @param parallelPolyline 输出参数，保存经过目标点的平行线
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateParallelLineThroughPoint(const QVector<QPointF>& polyline, const QPointF& targetPoint, QVector<QPointF>& parallelPolyline);


// ==========================================================================
// 缓冲区计算
// ==========================================================================

/**
 * 根据起点、终点和圆心计算特点方向的圆弧上的点(顺时针方向，从左往右)
 * @param startPoint 圆弧的起点
 * @param endPoint 圆弧的终点
 * @param center 圆心
 * @param steps 步数，决定计算多少个点
 * @param clockwise 是否顺时针方向绘制圆弧，true 表示顺时针，false 表示逆时针
 * @param arcPoints 输出参数，保存计算得到的弧线上的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateArcPointsFromStartEndCenter(const QPointF& startPoint, const QPointF& endPoint, const QPointF& center,
    int steps, QVector<QPointF>& arcPoints);

/**
 * 判断点与向量的位置关系
 * @param point 点
 * @param vectorStart 向量的起点
 * @param vectorEnd 向量的终点
 * @return 返回：
 *         - 1：点在向量的左侧
 *         - -1：点在向量的右侧
 *         - 0：点在向量的同一直线上
 */
int pointPositionRelativeToVector(const QPointF& point, const QPointF& vectorStart, const QPointF& vectorEnd);

/**
 * 计算折线的缓存区
 * @param polyline 输入折线的点列表
 * @param dis 平行线与折线的距离
 * @param points 输出参数
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateLineBuffer(const QVector<QPointF>& polyline, double dis, QVector<QPointF>& points);

// ==========================================================================================
// 缓冲区计算(基于栅格的缓冲区分析算法，广搜所有点)
// ==========================================================================================
// 
// 1. 遍历所有点，找到上下左右边界
// 2. 映射到网格里（方形,距离r映射为步数k）
// 3. 初始点标记为（1，k）,广搜,只记录步数的分界点
// 4. 遍历网格，找到分界点(1的上下左右有0)
// 5. 映射为原来坐标
// 6. 复杂度O（n^2）;

// 网格映射结构体
struct GridMap
{
    QVector<QPoint> gridPoints;  // 映射到网格的点
    double scale;                // 缩放比例
    QPointF offset;              // 偏移量(网格原点对应的原来坐标)
    int sizeX, sizeY;            // 网格尺寸
};

// 函数声明
QRectF calculateBounds(const QVector<QPointF>& points);
void mapToGrid(const QVector<QPointF>& points, double r, int& k, GridMap& gridMap);
void restoreFromGrid(const GridMap& gridMap, QVector<QPointF>& points);
void markBoundaryPoints(const GridMap& gridMap, int k, GridMap& boundaryGridMap);
bool calculateBuffer(const QVector<QPointF>& points, double r, QVector<QPointF>& boundaryPoints);