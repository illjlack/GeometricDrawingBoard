#pragma once

#include <QPointF>
#include <QVector>
#define M_PI 3.14159265358979323846
#include "Enums.h"

#include <QRectF>

// ==========================================================================
// �����߶��ϵĵ�
// ����Ľӿڣ� ͳһ�������
// ==========================================================================

/**
* �ܴ��澡����ʧ�ܻ���
* ����ͳһ�Ľӿڣ������Ķ�������������
*/

/**
 * ���ݽڵ����ͺͿ��Ƶ�������ϵĵ�
 * @param nodeLineStyle �ߵ����ͣ��������ߡ�����������Բ���ȣ�
 * @param controlPoints ���Ƶ��б����ڼ����ߵ���״
 * @param linePoints ����������������õ����߶��ϵĵ�
 * @param steps ����������������ٸ��м��
 * @return ����ɹ������򷵻� true��ʧ���򷵻� false
 */
bool calculateLinePoints(NodeLineStyle nodeLineStyle, const QVector<QPointF>& controlPoints, QVector<QPointF>& linePoints, int steps = 200);

/**
 * ���ݽڵ����ͺͿ��Ƶ����պ����ϵĵ㣨������һ�±պϣ�
 * @param lineStyle �ߵ�����
 * @param controlPoints ���Ƶ��б�
 * @param linePoints ����������������õ��ıպ����ϵĵ�
 * @param steps ����������������ٸ��м��
 * @return ����ɹ������򷵻� true��ʧ���򷵻� false
 */
bool calculateCloseLinePoints(NodeLineStyle lineStyle, const QVector<QPointF>& controlPoints, QVector<QPointF>& linePoints, int steps = 200);

// �����һ�����Ľṹ��
struct Component {
    int len;
    NodeLineStyle nodeLineStyle;
};

/**
 * ���ݶ���߶ε�����Ϳ��Ƶ�������ϵĵ�
 * @param component ����б�
 * @param controlPoints ���Ƶ��б�
 * @param linePointss ����������������õ����߶��ϵĵ�
 * @param steps ����������������ٸ��м��
 * @return ���سɹ����Ʒ�ͼ�ĸ���
 */
int calculateLinePoints(const QVector<Component>& component, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps = 200);

/**
 * ���ݶ���߶ε�����Ϳ��Ƶ����պ����ϵĵ�
 * @param component ����б�
 * @param controlPoints ���Ƶ��б�
 * @param linePointss ����������������õ��ıպ����ϵĵ�
 * @param steps ����������������ٸ��м��
 * @return ���سɹ����Ʒ�ͼ�ĸ���
 */
int calculateCloseLinePoints(const QVector<Component>& component, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps = 200);


/**
 * ���ݶ���߶ε�����Ϳ��Ƶ����ƽ���ߣ��ڶ�������һ���������ƾ��룩
 * @param component ����б�
 * @param controlPoints ���Ƶ��б�
 * @param linePointss ����������������õ��ıպ����ϵĵ�
 * @param steps ����������������ٸ��м��
 * @return ���سɹ����Ʒ�ͼ�ĸ���
 */
int calculateParallelLinePoints(const QVector<Component>& components, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps = 200);

// ==========================================================================
// ��������
// ���������� �� B����
// ==========================================================================

/**
 * �����������n ѡ i���������� C(n, i)
 * @param n ����
 * @param i ѡ����
 * @return ������������ֵ
 */
int binomialCoefficient(int n, int i);

/**
 * ����n�α��������ߵĵ㣨��ά��
 * @param controlPoints ���������ߵĿ��Ƶ��б�
 * @param t ��ֵ���ӣ�ͨ����[0, 1]֮��仯
 * @return �����ϵ�һ����
 */
QPointF bezierN(const QVector<QPointF>& controlPoints, double t);

/**
 * ����n�α����������ϵĶ���㣨��ά��
 * @param controlPoints ���������ߵĿ��Ƶ��б�
 * @param numPoints ������ٸ����ߵ�
 * @return �������м���õ��ı��������ߵ���б�
 */
QVector<QPointF> calculateBezierCurve(const QVector<QPointF>& controlPoints, int numPoints);

/**
 * Cox-de Boor �ݹ鹫ʽ�����ڼ��� B ���������ϵĵ�
 * @param knots �ڵ�����
 * @param i ��ǰ�ڵ������
 * @param p B �����Ĵ���
 * @param t ��ֵ����
 * @return ��ǰ��ֵλ�õ�Ȩ��
 */
double coxDeBoor(const QVector<double>& knots, int i, int p, double t);

/**
 * ���� B �������ߵĽڵ�����
 * @param n ���Ƶ���
 * @param degree ���ߵĴ���
 * @return ���ɵĽڵ�����
 */
QVector<double> generateKnotVector(int n, int degree);

/**
 * ���� B ���������ϵĶ���㣨��ά��
 * @param controlPoints ���Ƶ��б�
 * @param degree B �����Ĵ���
 * @param numCurvePoints Ҫ��������ߵ���
 * @param curvePoints ����������������õ������ߵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateBSplineCurve(const QVector<QPointF>& controlPoints, int degree, int numCurvePoints, QVector<QPointF>& curvePoints);

// ==========================================================================
// Բ������
// ==========================================================================

/**
 * �淶���Ƕȣ�ȷ���Ƕ���[0, 2��)��Χ��
 * @param angle ����Ƕȣ������ƣ�
 * @return �淶����ĽǶȣ������ƣ�
 */
double normalizeAngle(double angle);

/**
 * ���ݿ��Ƶ��������Բ���ĵ�
 * @param controlPoints ���Ƶ��б�
 * @param steps ����������������ٸ���
 * @param arcPoints ����������������õ��Ļ����ϵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateThreeArcPointsFromControlPoints(const QVector<QPointF>& controlPoints, int steps, QVector<QPointF>& arcPoints);

/**
 * ���ݿ��Ƶ����Բ���ĵ�
 * @param controlPoints ���Ƶ��б�
 * @param steps ����������������ٸ���
 * @param arcPoints ����������������õ��Ļ����ϵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateArcPointsFromControlPoints(const QVector<QPointF>& controlPoints, int steps, QVector<QPointF>& arcPoints);

/**
 * ����������㻡���ϵĵ�
 * @param point1 Բ�ϵĵ�һ����
 * @param point2 Բ�ϵĵڶ�����
 * @param point3 Բ�ϵĵ�������
 * @param steps ����������������ٸ���
 * @param arcPoints ����������������õ��Ļ����ϵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateArcPointsFromThreePoints(const QPointF& point1, const QPointF& point2, const QPointF& point3, int steps, QVector<QPointF>& arcPoints);

/**
 * �������������Բ�ϵĵ�
 * @param point1 Բ�ϵĵ�һ����
 * @param point2 Բ�ϵĵڶ�����
 * @param steps ����������������ٸ���
 * @param arcPoints ����������������õ���Բ�ϵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateCirclePointsFromControlPoints(const QPointF& point1, const QPointF& point2, int steps, QVector<QPointF>& arcPoints);

/**
 * ����ͨ������ȷ����Բ��Բ�ĺͰ뾶
 * @param p1 Բ�ϵĵ�һ����
 * @param p2 Բ�ϵĵڶ�����
 * @param p3 Բ�ϵĵ�������
 * @param center �������������õ���Բ������
 * @param radius �������������õ���Բ�İ뾶
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateCircle(const QPointF& p1, const QPointF& p2, const QPointF& p3, QPointF& center, double& radius);

/**
 * ����ͨ��������ȷ����Բ��Բ�ĺͰ뾶
 * @param p1 Բ�ϵĵ�һ����
 * @param p2 Բ�ϵĵڶ�����
 * @param center �������������õ���Բ������
 * @param radius �������������õ���Բ�İ뾶
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateCircle(const QPointF& p1, const QPointF& p2, QPointF& center, double& radius);

/**
 * ����Բ�ġ��뾶����ʼ�Ƕȡ��ǶȲ�ֵ�Ͳ������㻡���ϵĵ�
 * @param center Բ������
 * @param radius Բ�İ뾶
 * @param startAngle ��ʼ�Ƕȣ������ƣ�
 * @param angleDiff �ǶȲ�ֵ�������ƣ�
 * @param steps ����������������ٸ���
 * @param points ����������������õ��Ļ����ϵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateArcPoints(const QPointF& center, double radius, double startAngle, double angleDiff, int steps, QVector<QPointF>& points);

// ==========================================================================
// ƽ���߼���
// ==========================================================================
// �ο���https://zhuanlan.zhihu.com/p/536948720

// ���ȳ���
const double EPS = 1e-8;

// ���ȴ���
int sgn(double x);
// ������׼��
std::pair<double, double> normalize(double x, double y);

// �������
double cross(double x1, double y1, double x2, double y2);

/**
 * �������ߵ�˫��ƽ����
 * @param polyline �������ߵĵ��б�
 * @param dis ƽ���������ߵľ���
 * @param leftPolyline �������������ƽ���ߵ�����
 * @param rightPolyline �������������ƽ���ߵ��Ҳ��
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateParallelLine(const QVector<QPointF>& polyline, double dis, QVector<QPointF>& leftPolyline, QVector<QPointF>& rightPolyline);

/**
 * �������ߵĵ���ƽ����
 * @param polyline �������ߵĵ��б�
 * @param dis ƽ���������ߵľ���
 * @param parallelPolyline ������������浥��ƽ���ߵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateParallelLine(const QVector<QPointF>& polyline, double dis, QVector<QPointF>& parallelPolyline);


/**
 * ����㵽ֱ�ߵĴ�ֱ���룬�����ش�����Ľ��
 * @param point ��
 * @param lineStart ֱ�ߵ����
 * @param lineEnd ֱ�ߵ��յ�
 * @param direction ����ֵ�����ڱ�ʾ����ֱ�ߵ���һ�࣬��ֵΪ��࣬��ֵΪ�Ҳ࣬0Ϊֱ����
 * @return �㵽ֱ�ߵĴ�ֱ����
 */
double pointToLineDistanceWithDirection(const QPointF& point, const QPointF& lineStart, const QPointF& lineEnd);

/**
 * ����ƽ�����ߣ���ƽ���߾���ָ���ĵ�
 * @param polyline �������ߵĵ��б�
 * @param targetPoint ָ��Ŀ��㣬ƽ���߽�ͨ���õ�
 * @param parallelPolyline ������������澭��Ŀ����ƽ����
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateParallelLineThroughPoint(const QVector<QPointF>& polyline, const QPointF& targetPoint, QVector<QPointF>& parallelPolyline);


// ==========================================================================
// ����������
// ==========================================================================

/**
 * ������㡢�յ��Բ�ļ����ص㷽���Բ���ϵĵ�(˳ʱ�뷽�򣬴�������)
 * @param startPoint Բ�������
 * @param endPoint Բ�����յ�
 * @param center Բ��
 * @param steps ����������������ٸ���
 * @param clockwise �Ƿ�˳ʱ�뷽�����Բ����true ��ʾ˳ʱ�룬false ��ʾ��ʱ��
 * @param arcPoints ����������������õ��Ļ����ϵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateArcPointsFromStartEndCenter(const QPointF& startPoint, const QPointF& endPoint, const QPointF& center,
    int steps, QVector<QPointF>& arcPoints);

/**
 * �жϵ���������λ�ù�ϵ
 * @param point ��
 * @param vectorStart ���������
 * @param vectorEnd �������յ�
 * @return ���أ�
 *         - 1���������������
 *         - -1�������������Ҳ�
 *         - 0������������ͬһֱ����
 */
int pointPositionRelativeToVector(const QPointF& point, const QPointF& vectorStart, const QPointF& vectorEnd);

/**
 * �������ߵĻ�����
 * @param polyline �������ߵĵ��б�
 * @param dis ƽ���������ߵľ���
 * @param points �������
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateLineBuffer(const QVector<QPointF>& polyline, double dis, QVector<QPointF>& points);

// ==========================================================================================
// ����������(����դ��Ļ����������㷨���������е�)
// ==========================================================================================
// 
// 1. �������е㣬�ҵ��������ұ߽�
// 2. ӳ�䵽���������,����rӳ��Ϊ����k��
// 3. ��ʼ����Ϊ��1��k��,����,ֻ��¼�����ķֽ��
// 4. ���������ҵ��ֽ��(1������������0)
// 5. ӳ��Ϊԭ������
// 6. ���Ӷ�O��n^2��;

// ����ӳ��ṹ��
struct GridMap
{
    QVector<QPoint> gridPoints;  // ӳ�䵽����ĵ�
    double scale;                // ���ű���
    QPointF offset;              // ƫ����(����ԭ���Ӧ��ԭ������)
    int sizeX, sizeY;            // ����ߴ�
};

// ��������
QRectF calculateBounds(const QVector<QPointF>& points);
void mapToGrid(const QVector<QPointF>& points, double r, int& k, GridMap& gridMap);
void restoreFromGrid(const GridMap& gridMap, QVector<QPointF>& points);
void markBoundaryPoints(const GridMap& gridMap, int k, GridMap& boundaryGridMap);
bool calculateBuffer(const QVector<QPointF>& points, double r, QVector<QPointF>& boundaryPoints);