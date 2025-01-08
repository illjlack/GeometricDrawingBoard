#pragma once

#include <QPointF>
#include <QVector>
#define M_PI 3.14159265358979323846
#include "Enums.h"




// ���ݽڵ����ͺͿ��Ƶ�������ϵĵ�
// ���ݸ����Ľڵ����ͣ��������ߡ������ߡ�����Բ���ȣ��Ϳ��Ƶ㣬
// ���㲢����ָ���������߶��ϵĵ㡣
// lineStyle: �ߵ����ͣ����ߡ�����������Բ���ȣ�
// controlPoints: ���Ƶ��б����ڼ����ߵ���״
// steps: ����������������ٸ��м��
// linePoints: ����������������õ����߶��ϵĵ�
// ����ֵ������ɹ������򷵻� true��ʧ���򷵻� false
bool calculateLinePoints(NodeLineStyle nodeLineStyle, const QVector<QPointF>& controlPoints, QVector<QPointF>& linePoints, int steps = 200);
bool calculateCloseLinePoints(NodeLineStyle lineStyle, const QVector<QPointF>& controlPoints, QVector<QPointF>& linePoints, int steps = 200);


// �����һ����
struct Component
{
	int len;
	NodeLineStyle nodeLineStyle;
};

int calculateLinePoints(const QVector<Component>& component, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps = 200);

int calculateCloseLinePoints(const QVector<Component>& component, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps = 200);

// ======================================================================= ����

// �����������n ѡ i���������� C(n, i)
// n: ����
// i: ѡ����
// ����ֵ��������������ֵ
int binomialCoefficient(int n, int i);

// ����n�α��������ߵĵ㣨��ά��
// controlPoints: ���������ߵĿ��Ƶ��б�
// t: ��ֵ���ӣ�ͨ����[0, 1]֮��仯
// ����ֵ�������ϵ�һ����
QPointF bezierN(const QVector<QPointF>& controlPoints, double t);

// ����n�α����������ϵĶ���㣨��ά��
// controlPoints: ���������ߵĿ��Ƶ��б�
// numPoints: ������ٸ����ߵ�
// ����ֵ���������м���õ��ı��������ߵ���б�
QVector<QPointF> calculateBezierCurve(const QVector<QPointF>& controlPoints, int numPoints);

// Cox-de Boor �ݹ鹫ʽ�����ڼ��� B ���������ϵĵ�
// knots: �ڵ�����
// i: ��ǰ�ڵ������
// p: B �����Ĵ���
// t: ��ֵ����
// ����ֵ����ǰ��ֵλ�õ�Ȩ��
double coxDeBoor(const QVector<double>& knots, int i, int p, double t);

// ���� B �������ߵĽڵ�����
// n: ���Ƶ���
// degree: ���ߵĴ���
// ����ֵ�����ɵĽڵ�����
QVector<double> generateKnotVector(int n, int degree);

// ���� B ���������ϵĶ���㣨��ά��
// controlPoints: ���Ƶ��б�
// degree: B �����Ĵ���
// numCurvePoints: Ҫ��������ߵ���
// curvePoints: ����������������õ������ߵ�
// ����ֵ���������ɹ��򷵻� true��ʧ���򷵻� false
bool calculateBSplineCurve(const QVector<QPointF>& controlPoints, int degree, int numCurvePoints, QVector<QPointF>& curvePoints);


// ======================================================================== ���㻭Բ

bool calculateThreeArcPointsFromControlPoints(const QVector<QPointF>& controlPoints, int steps, QVector<QPointF>& arcPoints);
bool calculateArcPointsFromControlPoints(const QVector<QPointF>& controlPoints, int steps, QVector<QPointF>& arcPoints);
// ����������㻡���ϵĵ�
// point1, point2, point3: Բ�ϵ����㣬����ȷ��Բ��Բ�ĺͰ뾶
// steps: ����������������ٸ���
// arcPoints: ����������������õ��Ļ����ϵĵ�
// ����ֵ���������ɹ��򷵻� true��ʧ���򷵻� false
bool calculateArcPointsFromThreePoints(const QPointF& point1, const QPointF& point2, const QPointF& point3, int steps, QVector<QPointF>& arcPoints);

// ���ݵ����Բ�ϵĵ�
// point1, point2: Բ�ϵ�������֪�㣬����ȷ��Բ��Բ�ĺͰ뾶
// steps: ����������������ٸ���
// arcPoints: ����������������õ���Բ�ϵĵ�
// ����ֵ���������ɹ��򷵻� true��ʧ���򷵻� false
bool calculateCirclePointsFromControlPoints(const QPointF& point1, const QPointF& point2, int steps, QVector<QPointF>& arcPoints);
bool calculateCirclePointsFromControlPoints(const QPointF& point1, const QPointF& point2, const QPointF& point3, int steps, QVector<QPointF>& arcPoints);


// ����ͨ������ȷ����Բ��Բ�ĺͰ뾶
// p1, p2, p3: Բ�ϵ�������֪�㣬���ڼ���Բ�ĺͰ뾶
// center: �������������õ���Բ������
// radius: �������������õ���Բ�İ뾶
// ����ֵ���������ɹ��򷵻� true��ʧ���򷵻� false
bool calculateCircle(const QPointF& p1, const QPointF& p2, const QPointF& p3, QPointF& center, double& radius);

// ����ͨ��������ȷ����Բ��Բ�ĺͰ뾶
// p1, p2: Բ�ϵ�������֪�㣬���ڼ���Բ�ĺͰ뾶
// center: �������������õ���Բ������
// radius: �������������õ���Բ�İ뾶
// ����ֵ���������ɹ��򷵻� true��ʧ���򷵻� false
bool calculateCircle(const QPointF& p1, const QPointF& p2, QPointF& center, double& radius);


// �淶���Ƕȣ�ȷ���Ƕ���[0, 2��)��Χ��
// angle: ����Ƕȣ������ƣ�
// ����ֵ���淶����ĽǶȣ������ƣ�
double normalizeAngle(double angle);


// ����Բ�ġ��뾶����ʼ�Ƕȡ��ǶȲ�ֵ�Ͳ������㻡���ϵĵ�
// center: Բ������
// radius: Բ�İ뾶
// startAngle: ��ʼ�Ƕȣ������ƣ�
// angleDiff: �ǶȲ�ֵ�������ƣ�
// steps: ����������������ٸ���
// points: ����������������õ��Ļ����ϵĵ�
// ����ֵ���������ɹ��򷵻� true��ʧ���򷵻� false
bool calculateArcPoints(const QPointF& center, double radius, double startAngle, double angleDiff, int steps, QVector<QPointF>& points);
