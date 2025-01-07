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

int GeoSplineCurve::getDegree()
{
    return degree;
}

int GeoSplineCurve::getNumControlPoints()
{
    return controlPoints.size();
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


// ========================================================================================Circle
bool calculateCircle(const QPointF& p1, const QPointF& p2, const QPointF& p3, QPointF& center, double& radius) 
{
    if (p1 == p2 || p2 == p3 || p3 == p1) // ��Ϊ�������꣬�������
    {
        return false;
    }
    
    // ���������ߵ��д��߷���
    double x1 = p1.x(), y1 = p1.y();
    double x2 = p2.x(), y2 = p2.y();
    double x3 = p3.x(), y3 = p3.y();

    double a1 = x2 - x1, b1 = y2 - y1;
    double a2 = x3 - x2, b2 = y3 - y2;

    double mid1_x = (x1 + x2) / 2.0, mid1_y = (y1 + y2) / 2.0;
    double mid2_x = (x2 + x3) / 2.0, mid2_y = (y2 + y3) / 2.0;

    double slope1, slope2;
    double c1, c2;

    // ������ֱ�����
    if (b1 == 0) {  // p1 �� p2 ��ֱ��
        slope1 = std::numeric_limits<double>::infinity();  // �����
        c1 = mid1_x;  // �д���Ϊ x = mid1_x
    }
    else {
        slope1 = -a1 / b1;
        c1 = mid1_y - slope1 * mid1_x;
    }

    if (b2 == 0) {  // p2 �� p3 ��ֱ��
        slope2 = std::numeric_limits<double>::infinity();  // �����
        c2 = mid2_x;  // �д���Ϊ x = mid2_x
    }
    else {
        slope2 = -a2 / b2;
        c2 = mid2_y - slope2 * mid2_x;
    }

    // ���㽻��
    if (slope1 - slope2 == 0.0f) { // ��������ȵ� 
        return false; //���㹲�ߣ��޷�ȷ��ΨһԲ
    }

    double center_x = (c2 - c1) / (slope1 - slope2);
    double center_y = slope1 * center_x + c1;

    center = QPointF(center_x, center_y);
    // ����뾶
    radius = std::sqrt(std::pow(center.x() - x1, 2) + std::pow(center.y() - y1, 2));
    return true;
}

bool calculateCircle(const QPointF& p1, const QPointF& p2, QPointF& center, double& radius) {
    // ������������е���ΪԲ��
    double center_x = (p1.x() + p2.x()) / 2.0;
    double center_y = (p1.y() + p2.y()) / 2.0;

    center = QPointF(center_x, center_y);

    // ����뾶Ϊ����֮��ľ���
    radius = std::sqrt(std::pow(p2.x() - p1.x(), 2) + std::pow(p2.y() - p1.y(), 2)) / 2.0;

    return true;
}
