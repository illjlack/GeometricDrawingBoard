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

int GeoSplineCurve::getDegree()
{
    return degree;
}

int GeoSplineCurve::getNumControlPoints()
{
    return controlPoints.size();
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


// ========================================================================================Circle
bool calculateCircle(const QPointF& p1, const QPointF& p2, const QPointF& p3, QPointF& center, double& radius) 
{
    if (p1 == p2 || p2 == p3 || p3 == p1) // 因为像素坐标，可能相等
    {
        return false;
    }
    
    // 计算两条边的中垂线方程
    double x1 = p1.x(), y1 = p1.y();
    double x2 = p2.x(), y2 = p2.y();
    double x3 = p3.x(), y3 = p3.y();

    double a1 = x2 - x1, b1 = y2 - y1;
    double a2 = x3 - x2, b2 = y3 - y2;

    double mid1_x = (x1 + x2) / 2.0, mid1_y = (y1 + y2) / 2.0;
    double mid2_x = (x2 + x3) / 2.0, mid2_y = (y2 + y3) / 2.0;

    double slope1, slope2;
    double c1, c2;

    // 处理竖直线情况
    if (b1 == 0) {  // p1 到 p2 竖直线
        slope1 = std::numeric_limits<double>::infinity();  // 无穷大
        c1 = mid1_x;  // 中垂线为 x = mid1_x
    }
    else {
        slope1 = -a1 / b1;
        c1 = mid1_y - slope1 * mid1_x;
    }

    if (b2 == 0) {  // p2 到 p3 竖直线
        slope2 = std::numeric_limits<double>::infinity();  // 无穷大
        c2 = mid2_x;  // 中垂线为 x = mid2_x
    }
    else {
        slope2 = -a2 / b2;
        c2 = mid2_y - slope2 * mid2_x;
    }

    // 计算交点
    if (slope1 - slope2 == 0.0f) { // 可能是相等的 
        return false; //三点共线，无法确定唯一圆
    }

    double center_x = (c2 - c1) / (slope1 - slope2);
    double center_y = slope1 * center_x + c1;

    center = QPointF(center_x, center_y);
    // 计算半径
    radius = std::sqrt(std::pow(center.x() - x1, 2) + std::pow(center.y() - y1, 2));
    return true;
}

bool calculateCircle(const QPointF& p1, const QPointF& p2, QPointF& center, double& radius) {
    // 计算两个点的中点作为圆心
    double center_x = (p1.x() + p2.x()) / 2.0;
    double center_y = (p1.y() + p2.y()) / 2.0;

    center = QPointF(center_x, center_y);

    // 计算半径为两点之间的距离
    radius = std::sqrt(std::pow(p2.x() - p1.x(), 2) + std::pow(p2.y() - p1.y(), 2)) / 2.0;

    return true;
}
