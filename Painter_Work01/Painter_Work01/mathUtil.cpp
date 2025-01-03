#include "mathUtil.h"

namespace mathUtil
{
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

    // Cox-de Boor算法，递归计算B样条基函数值
    double coxDeBoor(const QVector<double>& knots, int i, int p, double t)
    {
        // 基本情况
        if (p == 0) {
            if (knots[i] <= t && t < knots[i + 1]) {
                return 1.0;
            }
            else {
                return 0.0;
            }
        }

        double left = 0.0, right = 0.0;
        if (i + p < knots.size() && knots[i + p] != knots[i]) {
            left = (t - knots[i]) / (knots[i + p] - knots[i]) * coxDeBoor(knots, i, p - 1, t);
        }

        if (i + p + 1 < knots.size() && knots[i + p + 1] != knots[i + 1]) {
            right = (knots[i + p + 1] - t) / (knots[i + p + 1] - knots[i + 1]) * coxDeBoor(knots, i + 1, p - 1, t);
        }

        return left + right;
    }

    // 计算B样条曲线上的点, p是B样条的阶数
    QVector<QPointF> calculateBSpline(const QVector<QPointF>& controlPoints, int p, int numPoints)
    {
        QVector<QPointF> curvePoints;
        int n = controlPoints.size() - 1;  // 控制点的数量
        int m = n + p + 1;  // 节点矢量的长度

        // 计算节点矢量 (Uniform Knot Vector, 简单的均匀节点矢量)
        QVector<double> knots(m);
        for (int i = 0; i < m; ++i) {
            if (i <= p) {
                knots[i] = 0.0;
            }
            else if (i >= m - p) {
                knots[i] = 1.0;
            }
            else {
                knots[i] = (i - p) / (double)(n - p + 1);
            }
        }

        // 计算曲线上的多个点
        for (int i = 0; i < numPoints; ++i) {
            double t = i / (double)numPoints;
            QPointF point(0.0, 0.0);

            // 对于每个控制点，使用Cox-de Boor公式计算B样条值并加权
            for (int j = 0; j <= n; ++j) {
                double weight = coxDeBoor(knots, j, p, t);
                point += weight * controlPoints[j];
            }

            curvePoints.append(point);
        }

        return curvePoints;
    }

};

