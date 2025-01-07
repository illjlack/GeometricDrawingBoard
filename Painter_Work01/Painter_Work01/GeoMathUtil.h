#pragma once

#include <QPointF>
#include <QVector>

// 计算组合数（n 选 i）
int binomialCoefficient(int n, int i);
// 计算n次贝塞尔曲线的点（二维）
QPointF bezierN(const QVector<QPointF>& controlPoints, double t);
// 计算n次贝塞尔曲线上的多个点（二维）
QVector<QPointF> calculateBezierCurve(const QVector<QPointF>& controlPoints, int numPoints);

// ========================================================================B样条

class GeoSplineCurve {
public:
    GeoSplineCurve(int p, int numPoints);

    void addControlPoint(const QPointF& point);
    void updateControlPoint(int index, const QPointF& point);
    void removeControlPoint(int index);
    QVector<QPointF> getCurvePoints() const;
    QVector<QPointF> getControlPoints() const;

    int getDegree();
    int getNumControlPoints();

    void setDegree(int p);
    void setNumCurvePoints(int numPoints);

private:
    void recalculateCurve();
    QVector<double> generateKnotVector(int n, int degree) const;
    double coxDeBoor(const QVector<double>& knots, int i, int p, double t) const;

    int degree;
    int numCurvePoints;
    QVector<QPointF> controlPoints;
    QVector<QPointF> curvePoints;
};

// ========================================================================三点画圆
// 计算三点确定的圆的圆心和半径
bool calculateCircle(const QPointF& p1, const QPointF& p2, const QPointF& p3, QPointF& center, float& radius);

// 计算两个点确定的圆的圆心和半径
bool calculateCircle(const QPointF& p1, const QPointF& p2, QPointF& center, float& radius);