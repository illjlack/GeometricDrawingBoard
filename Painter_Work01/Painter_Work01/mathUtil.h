#pragma once

#include <QPointF>
#include <QVector>
#include <Geo.h>

namespace mathUtil
{
    // �����������n ѡ i��
    int binomialCoefficient(int n, int i);
    
    // ����n�α��������ߵĵ㣨��ά��
    QPointF bezierN(const QVector<QPointF>& controlPoints, double t);
    
    // ����n�α����������ϵĶ���㣨��ά��
    QVector<QPointF> calculateBezierCurve(const QVector<QPointF>& controlPoints, int numPoints);

    // Cox-de Boor�㷨���ݹ����B����������ֵ
    double coxDeBoor(const QVector<double>& knots, int i, int p, double t);

    // ����B���������ϵĵ�, p��B�����Ľ���
    QVector<QPointF> calculateBSpline(const QVector<ControlPoint>& controlPoints, int p, int numPoints);
};

