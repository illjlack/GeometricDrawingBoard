#pragma once

#include <QPointF>
#include <QVector>
#define M_PI 3.14159265358979323846
#include "Enums.h"




// 根据节点类型和控制点计算线上的点
// 根据给定的节点类型（例如折线、样条线、三点圆弧等）和控制点，
// 计算并返回指定数量的线段上的点。
// lineStyle: 线的类型（折线、样条、三点圆弧等）
// controlPoints: 控制点列表，用于计算线的形状
// steps: 步数，决定计算多少个中间点
// linePoints: 输出参数，保存计算得到的线段上的点
// 返回值：如果成功计算则返回 true，失败则返回 false
bool calculateLinePoints(NodeLineStyle nodeLineStyle, const QVector<QPointF>& controlPoints, QVector<QPointF>& linePoints, int steps = 200);
bool calculateCloseLinePoints(NodeLineStyle lineStyle, const QVector<QPointF>& controlPoints, QVector<QPointF>& linePoints, int steps = 200);


// 多个线一起算
struct Component
{
	int len;
	NodeLineStyle nodeLineStyle;
};

int calculateLinePoints(const QVector<Component>& component, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps = 200);

int calculateCloseLinePoints(const QVector<Component>& component, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps = 200);

// ======================================================================= 样条

// 计算组合数（n 选 i），即计算 C(n, i)
// n: 总数
// i: 选择数
// 返回值：计算出的组合数值
int binomialCoefficient(int n, int i);

// 计算n次贝塞尔曲线的点（二维）
// controlPoints: 贝塞尔曲线的控制点列表
// t: 插值因子，通常在[0, 1]之间变化
// 返回值：曲线上的一个点
QPointF bezierN(const QVector<QPointF>& controlPoints, double t);

// 计算n次贝塞尔曲线上的多个点（二维）
// controlPoints: 贝塞尔曲线的控制点列表
// numPoints: 计算多少个曲线点
// 返回值：包含所有计算得到的贝塞尔曲线点的列表
QVector<QPointF> calculateBezierCurve(const QVector<QPointF>& controlPoints, int numPoints);

// Cox-de Boor 递归公式，用于计算 B 样条曲线上的点
// knots: 节点向量
// i: 当前节点的索引
// p: B 样条的次数
// t: 插值因子
// 返回值：当前插值位置的权重
double coxDeBoor(const QVector<double>& knots, int i, int p, double t);

// 生成 B 样条曲线的节点向量
// n: 控制点数
// degree: 曲线的次数
// 返回值：生成的节点向量
QVector<double> generateKnotVector(int n, int degree);

// 计算 B 样条曲线上的多个点（二维）
// controlPoints: 控制点列表
// degree: B 样条的次数
// numCurvePoints: 要计算的曲线点数
// curvePoints: 输出参数，保存计算得到的曲线点
// 返回值：如果计算成功则返回 true，失败则返回 false
bool calculateBSplineCurve(const QVector<QPointF>& controlPoints, int degree, int numCurvePoints, QVector<QPointF>& curvePoints);


// ======================================================================== 三点画圆

bool calculateThreeArcPointsFromControlPoints(const QVector<QPointF>& controlPoints, int steps, QVector<QPointF>& arcPoints);
bool calculateArcPointsFromControlPoints(const QVector<QPointF>& controlPoints, int steps, QVector<QPointF>& arcPoints);
// 根据三点计算弧线上的点
// point1, point2, point3: 圆上的三点，用于确定圆的圆心和半径
// steps: 步数，决定计算多少个点
// arcPoints: 输出参数，保存计算得到的弧线上的点
// 返回值：如果计算成功则返回 true，失败则返回 false
bool calculateArcPointsFromThreePoints(const QPointF& point1, const QPointF& point2, const QPointF& point3, int steps, QVector<QPointF>& arcPoints);

// 根据点计算圆上的点
// point1, point2: 圆上的两个已知点，用于确定圆的圆心和半径
// steps: 步数，决定计算多少个点
// arcPoints: 输出参数，保存计算得到的圆上的点
// 返回值：如果计算成功则返回 true，失败则返回 false
bool calculateCirclePointsFromControlPoints(const QPointF& point1, const QPointF& point2, int steps, QVector<QPointF>& arcPoints);
bool calculateCirclePointsFromControlPoints(const QPointF& point1, const QPointF& point2, const QPointF& point3, int steps, QVector<QPointF>& arcPoints);


// 计算通过三点确定的圆的圆心和半径
// p1, p2, p3: 圆上的三个已知点，用于计算圆心和半径
// center: 输出参数，计算得到的圆心坐标
// radius: 输出参数，计算得到的圆的半径
// 返回值：如果计算成功则返回 true，失败则返回 false
bool calculateCircle(const QPointF& p1, const QPointF& p2, const QPointF& p3, QPointF& center, double& radius);

// 计算通过两个点确定的圆的圆心和半径
// p1, p2: 圆上的两个已知点，用于计算圆心和半径
// center: 输出参数，计算得到的圆心坐标
// radius: 输出参数，计算得到的圆的半径
// 返回值：如果计算成功则返回 true，失败则返回 false
bool calculateCircle(const QPointF& p1, const QPointF& p2, QPointF& center, double& radius);


// 规范化角度，确保角度在[0, 2π)范围内
// angle: 输入角度（弧度制）
// 返回值：规范化后的角度（弧度制）
double normalizeAngle(double angle);


// 根据圆心、半径、起始角度、角度差值和步数计算弧线上的点
// center: 圆心坐标
// radius: 圆的半径
// startAngle: 起始角度（弧度制）
// angleDiff: 角度差值（弧度制）
// steps: 步数，决定计算多少个点
// points: 输出参数，保存计算得到的弧线上的点
// 返回值：如果计算成功则返回 true，失败则返回 false
bool calculateArcPoints(const QPointF& center, double radius, double startAngle, double angleDiff, int steps, QVector<QPointF>& points);
