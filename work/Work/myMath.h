#include <QVector>
#include <QVector3D>
#include <cmath>

namespace My
{
    // �����������n ѡ i��
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

    // ����n�α��������ߵĵ㣨��ά��
    QVector3D bezierN(const QVector<QVector3D>& controlPoints, double t)
    {
        int n = controlPoints.size() - 1;
        QVector3D point(0.0, 0.0, 0.0); // �����洢������

        // ���������ߵ���͹�ʽ
        for (int i = 0; i <= n; ++i) {
            int binom = binomialCoefficient(n, i);
            double weight = binom * pow(1 - t, n - i) * pow(t, i);
            point += weight * controlPoints[i];
        }

        return point;
    }

    // ����n�α����������ϵĶ���㣨��ά��
    QVector<QVector3D> calculateBezierCurve(const QVector<QVector3D>& controlPoints, int numPoints)
    {
        QVector<QVector3D> curvePoints;

        // ���������ϵĶ����
        for (int i = 0; i <= numPoints; ++i) {
            double t = i / (double)numPoints; // t ֵ�ı仯
            QVector3D point = bezierN(controlPoints, t); // ���㵱ǰtֵ��Ӧ�ı�������
            curvePoints.append(point);
        }

        return curvePoints;
    }
}
