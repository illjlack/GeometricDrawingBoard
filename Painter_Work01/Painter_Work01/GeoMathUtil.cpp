#include "GeoMathUtil.h"
#include <QVector>
#include <QPointF>
#include <cmath>

// =================================================================================================����������
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

// ==================================================================================================B����
// ���캯��
GeoSplineCurve::GeoSplineCurve(int p, int numPoints)
    : degree(p), numCurvePoints(numPoints) {}

// ��ӿ��Ƶ�
void GeoSplineCurve::addControlPoint(const QPointF& point) {
    controlPoints.append(point);
    recalculateCurve();
}

// ���¿��Ƶ�
void GeoSplineCurve::updateControlPoint(int index, const QPointF& point) {
    if (index >= 0 && index < controlPoints.size()) {
        controlPoints[index] = point;
        recalculateCurve();
    }
}

// �Ƴ����Ƶ�
void GeoSplineCurve::removeControlPoint(int index) {
    if (index >= 0 && index < controlPoints.size()) {
        controlPoints.removeAt(index);
        recalculateCurve();
    }
}

// ��ȡ���ߵ�
QVector<QPointF> GeoSplineCurve::getCurvePoints() const {
    return curvePoints;
}

// ��ȡ���Ƶ�
QVector<QPointF> GeoSplineCurve::getControlPoints() const {
    return controlPoints;
}

// �����������ߵĴ���
void GeoSplineCurve::setDegree(int p) {
    degree = std::max(1, p); // ��֤��������Ϊ1
    recalculateCurve();
}

// �������ɵ����ߵ���
void GeoSplineCurve::setNumCurvePoints(int numPoints) {
    numCurvePoints = std::max(2, numPoints); // ��֤��������Ϊ2
    recalculateCurve();
}

// ���¼�����������
void GeoSplineCurve::recalculateCurve() {
    curvePoints.clear();
    if (controlPoints.size() < degree + 1) {
        return; // ���Ƶ㲻�㣬�޷�����������
    }

    int n = controlPoints.size() - 1; // ���Ƶ���
    QVector<double> knots = generateKnotVector(n, degree);

    // ���������ϵĵ�
    for (int i = 0; i < numCurvePoints; ++i) {
        double t = i / static_cast<double>(numCurvePoints);
        QPointF point(0.0, 0.0);

        // �����Ƶ�����Ȩֵ
        for (int j = 0; j <= n; ++j) {
            double weight = coxDeBoor(knots, j, degree, t);
            point += weight * controlPoints[j];
        }

        curvePoints.append(point);
    }
}

// ���ɽڵ�����
QVector<double> GeoSplineCurve::generateKnotVector(int n, int degree) const {
    int m = n + degree + 2; // �ڵ�������С
    QVector<double> knots(m);

    for (int i = 0; i < m; ++i) {
        if (i <= degree) {
            knots[i] = 0.0; // ǰ degree+1 ��ֵΪ 0
        }
        else if (i >= m - degree - 1) {
            knots[i] = 1.0; // �� degree+1 ��ֵΪ 1
        }
        else {
            knots[i] = (i - degree) / static_cast<double>(n - degree + 1); // �м���ȷֲ�
        }
    }

    return knots;
}

// Cox-de Boor �ݹ鹫ʽ
double GeoSplineCurve::coxDeBoor(const QVector<double>& knots, int i, int p, double t) const {
    if (p == 0) {
        // �������
        return (knots[i] <= t && t < knots[i + 1]) ? 1.0 : 0.0;
    }

    double left = 0.0, right = 0.0;

    // ��ݹ鲿��
    if (knots[i + p] != knots[i]) {
        left = (t - knots[i]) / (knots[i + p] - knots[i]) * coxDeBoor(knots, i, p - 1, t);
    }

    // �ҵݹ鲿��
    if (knots[i + p + 1] != knots[i + 1]) {
        right = (knots[i + p + 1] - t) / (knots[i + p + 1] - knots[i + 1]) * coxDeBoor(knots, i + 1, p - 1, t);
    }

    return left + right;
}
