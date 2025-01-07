#pragma once

#include <QPointF>
#include <QVector>

// �����������n ѡ i��
int binomialCoefficient(int n, int i);
// ����n�α��������ߵĵ㣨��ά��
QPointF bezierN(const QVector<QPointF>& controlPoints, double t);
// ����n�α����������ϵĶ���㣨��ά��
QVector<QPointF> calculateBezierCurve(const QVector<QPointF>& controlPoints, int numPoints);

// ========================================================================B����

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

// ========================================================================���㻭Բ
// ��������ȷ����Բ��Բ�ĺͰ뾶
bool calculateCircle(const QPointF& p1, const QPointF& p2, const QPointF& p3, QPointF& center, float& radius);

// ����������ȷ����Բ��Բ�ĺͰ뾶
bool calculateCircle(const QPointF& p1, const QPointF& p2, QPointF& center, float& radius);