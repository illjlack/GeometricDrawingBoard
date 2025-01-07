#pragma once

#include <QPointF>
#include <QVector>
#include <Geo.h>

namespace mathUtil
{
    // 计算组合数（n 选 i）
    int binomialCoefficient(int n, int i);
    
    // 计算n次贝塞尔曲线的点（二维）
    QPointF bezierN(const QVector<QPointF>& controlPoints, double t);
    
    // 计算n次贝塞尔曲线上的多个点（二维）
    QVector<QPointF> calculateBezierCurve(const QVector<QPointF>& controlPoints, int numPoints);

    // Cox-de Boor算法，递归计算B样条基函数值
    double coxDeBoor(const QVector<double>& knots, int i, int p, double t);

    // 计算B样条曲线上的点, p是B样条的阶数
    QVector<QPointF> calculateBSpline(const QVector<ControlPoint>& controlPoints, int p, int numPoints);
};

