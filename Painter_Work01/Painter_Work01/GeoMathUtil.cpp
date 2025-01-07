#include "GeoMathUtil.h"
#include <QVector>
#include <QPointF>
#include <cmath>

// =================================================================================================贝塞尔曲线
    // 计算组合数（n 选 i）
int binomialCoefficient(int n, int i)
{
    int res = 1;
    for (int j = 0; j < i; ++j) {
        res *= (n - j);
        res /= (j + 1);
    }
    return res;
}

// 计算n次贝塞尔曲线的点（二维）
QPointF bezierN(const QVector<QPointF>& controlPoints, double t)
{
    int n = controlPoints.size() - 1;
    QPointF point(0.0, 0.0); // 用来存储计算结果

    // 贝塞尔曲线的求和公式
    for (int i = 0; i <= n; ++i) {
        int binom = binomialCoefficient(n, i);
        double weight = binom * pow(1 - t, n - i) * pow(t, i);
        point += weight * controlPoints[i];
    }

    return point;
}

// 计算n次贝塞尔曲线上的多个点（二维）
QVector<QPointF> calculateBezierCurve(const QVector<QPointF>& controlPoints, int numPoints)
{
    QVector<QPointF> curvePoints;

    // 计算曲线上的多个点
    for (int i = 0; i <= numPoints; ++i) {
        double t = i / (double)numPoints; // t 值的变化
        QPointF point = bezierN(controlPoints, t); // 计算当前t值对应的贝塞尔点
        curvePoints.append(point);
    }

    return curvePoints;
}

// ==================================================================================================B样条
// 构造函数
GeoSplineCurve::GeoSplineCurve(int p, int numPoints)
    : degree(p), numCurvePoints(numPoints) {}

// 添加控制点
void GeoSplineCurve::addControlPoint(const QPointF& point) {
    controlPoints.append(point);
    recalculateCurve();
}

// 更新控制点
void GeoSplineCurve::updateControlPoint(int index, const QPointF& point) {
    if (index >= 0 && index < controlPoints.size()) {
        controlPoints[index] = point;
        recalculateCurve();
    }
}

// 移除控制点
void GeoSplineCurve::removeControlPoint(int index) {
    if (index >= 0 && index < controlPoints.size()) {
        controlPoints.removeAt(index);
        recalculateCurve();
    }
}

// 获取曲线点
QVector<QPointF> GeoSplineCurve::getCurvePoints() const {
    return curvePoints;
}

// 获取控制点
QVector<QPointF> GeoSplineCurve::getControlPoints() const {
    return controlPoints;
}

// 设置样条曲线的次数
void GeoSplineCurve::setDegree(int p) {
    degree = std::max(1, p); // 保证次数至少为1
    recalculateCurve();
}

// 设置生成的曲线点数
void GeoSplineCurve::setNumCurvePoints(int numPoints) {
    numCurvePoints = std::max(2, numPoints); // 保证点数至少为2
    recalculateCurve();
}

// 重新计算样条曲线
void GeoSplineCurve::recalculateCurve() {
    curvePoints.clear();
    if (controlPoints.size() < degree + 1) {
        return; // 控制点不足，无法计算样条线
    }

    int n = controlPoints.size() - 1; // 控制点数
    QVector<double> knots = generateKnotVector(n, degree);

    // 计算曲线上的点
    for (int i = 0; i < numCurvePoints; ++i) {
        double t = i / static_cast<double>(numCurvePoints);
        QPointF point(0.0, 0.0);

        // 按控制点计算加权值
        for (int j = 0; j <= n; ++j) {
            double weight = coxDeBoor(knots, j, degree, t);
            point += weight * controlPoints[j];
        }

        curvePoints.append(point);
    }
}

// 生成节点向量
QVector<double> GeoSplineCurve::generateKnotVector(int n, int degree) const {
    int m = n + degree + 2; // 节点向量大小
    QVector<double> knots(m);

    for (int i = 0; i < m; ++i) {
        if (i <= degree) {
            knots[i] = 0.0; // 前 degree+1 个值为 0
        }
        else if (i >= m - degree - 1) {
            knots[i] = 1.0; // 后 degree+1 个值为 1
        }
        else {
            knots[i] = (i - degree) / static_cast<double>(n - degree + 1); // 中间均匀分布
        }
    }

    return knots;
}

// Cox-de Boor 递归公式
double GeoSplineCurve::coxDeBoor(const QVector<double>& knots, int i, int p, double t) const {
    if (p == 0) {
        // 基础情况
        return (knots[i] <= t && t < knots[i + 1]) ? 1.0 : 0.0;
    }

    double left = 0.0, right = 0.0;

    // 左递归部分
    if (knots[i + p] != knots[i]) {
        left = (t - knots[i]) / (knots[i + p] - knots[i]) * coxDeBoor(knots, i, p - 1, t);
    }

    // 右递归部分
    if (knots[i + p + 1] != knots[i + 1]) {
        right = (knots[i + p + 1] - t) / (knots[i + p + 1] - knots[i + 1]) * coxDeBoor(knots, i + 1, p - 1, t);
    }

    return left + right;
}
