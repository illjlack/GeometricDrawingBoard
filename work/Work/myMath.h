#include <QVector>
#include <QVector3D>
#include <cmath>

namespace My
{
    // 计算组合数（n 选 i）
    int binomialCoefficient(int n, int i)
    {
        int res = 1;
        if (i > n - i) {
            i = n - i;
        }
        for (int j = 0; j < i; ++j) {
            res *= (n - j);
            res /= (j + 1);
        }
        return res;
    }

    // 计算n次贝塞尔曲线的点（三维）
    QVector3D bezierN(const QVector<QVector3D>& controlPoints, double t)
    {
        int n = controlPoints.size() - 1;
        QVector3D point(0.0, 0.0, 0.0); // 用来存储计算结果

        // 贝塞尔曲线的求和公式
        for (int i = 0; i <= n; ++i) {
            int binom = binomialCoefficient(n, i);
            double weight = binom * pow(1 - t, n - i) * pow(t, i);
            point += weight * controlPoints[i];
        }

        return point;
    }

    // 计算n次贝塞尔曲线上的多个点（三维）
    QVector<QVector3D> calculateBezierCurve(const QVector<QVector3D>& controlPoints, int numPoints)
    {
        QVector<QVector3D> curvePoints;

        // 计算曲线上的多个点
        for (int i = 0; i <= numPoints; ++i) {
            double t = i / (double)numPoints; // t 值的变化
            QVector3D point = bezierN(controlPoints, t); // 计算当前t值对应的贝塞尔点
            curvePoints.append(point);
        }

        return curvePoints;
    }
}
