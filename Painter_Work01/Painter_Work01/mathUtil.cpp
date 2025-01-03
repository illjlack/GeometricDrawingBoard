#include "mathUtil.h"

namespace mathUtil
{
    // �����������n ѡ i��
    int binomialCoefficient(int n, int i)
    {
        int res = 1;
        for (int j = 0; j < i; ++j) {
            res *= (n - j);
            res /= (j + 1);
        }
        return res;
    }

    // ����n�α��������ߵĵ㣨��ά��
    QPointF bezierN(const QVector<QPointF>& controlPoints, double t)
    {
        int n = controlPoints.size() - 1;
        QPointF point(0.0, 0.0); // �����洢������

        // ���������ߵ���͹�ʽ
        for (int i = 0; i <= n; ++i) {
            int binom = binomialCoefficient(n, i);
            double weight = binom * pow(1 - t, n - i) * pow(t, i);
            point += weight * controlPoints[i];
        }

        return point;
    }

    // ����n�α����������ϵĶ���㣨��ά��
    QVector<QPointF> calculateBezierCurve(const QVector<QPointF>& controlPoints, int numPoints)
    {
        QVector<QPointF> curvePoints;

        // ���������ϵĶ����
        for (int i = 0; i <= numPoints; ++i) {
            double t = i / (double)numPoints; // t ֵ�ı仯
            QPointF point = bezierN(controlPoints, t); // ���㵱ǰtֵ��Ӧ�ı�������
            curvePoints.append(point);
        }

        return curvePoints;
    }

    // Cox-de Boor�㷨���ݹ����B����������ֵ
    double coxDeBoor(const QVector<double>& knots, int i, int p, double t)
    {
        // �������
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

    // ����B���������ϵĵ�, p��B�����Ľ���
    QVector<QPointF> calculateBSpline(const QVector<QPointF>& controlPoints, int p, int numPoints)
    {
        QVector<QPointF> curvePoints;
        int n = controlPoints.size() - 1;  // ���Ƶ������
        int m = n + p + 1;  // �ڵ�ʸ���ĳ���

        // ����ڵ�ʸ�� (Uniform Knot Vector, �򵥵ľ��Ƚڵ�ʸ��)
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

        // ���������ϵĶ����
        for (int i = 0; i < numPoints; ++i) {
            double t = i / (double)numPoints;
            QPointF point(0.0, 0.0);

            // ����ÿ�����Ƶ㣬ʹ��Cox-de Boor��ʽ����B����ֵ����Ȩ
            for (int j = 0; j <= n; ++j) {
                double weight = coxDeBoor(knots, j, p, t);
                point += weight * controlPoints[j];
            }

            curvePoints.append(point);
        }

        return curvePoints;
    }

};

