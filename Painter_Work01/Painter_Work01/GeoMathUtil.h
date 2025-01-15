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
// ���������㣨����ʸ���Ļ����������㷨��ƽ���ߡ���ƽ���ߡ�͹Բ���ǣ��������ཻ������)
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


bool computeBufferBoundaryWithVector(const QVector<QVector<QPointF>>& pointss, double r, QVector<QVector<QPointF>>& boundaryPointss);

// ==========================================================================================
// ����դ��Ļ����������㷨������ö�����е㣬��������
// ==========================================================================================

// GridMap ���ݽṹ�����ڴ洢����ӳ����Ϣ
struct GridMap {
    double scale;     // ���ű���
    QPointF offset;   // ƫ����
    int sizeX;        // �������ߴ�
    int sizeY;        // ��������ߴ�
};

/**
 * ����㼯�ı߽�
 * @param pointss ����㼯
 * @return �㼯�ı߽����
 */
QRectF calculateBounds(const QVector<QVector<QPointF>>& pointss);

/**
 * ���㼯ӳ�䵽����
 * @param pointss ����㼯
 * @param r �����С
 * @param gridMap �������ӳ��
 */
void getGridMap(const QVector<QVector<QPointF>>& pointss, double r, GridMap& gridMap);

/**
 * ������ָ�ԭʼ�㼯
 * @param gridPointss ����㼯
 * @param gridMap ����ӳ��
 * @param pointss ����ָ���ĵ㼯
 */
void restoreFromGrid(const QVector<QVector<QPoint>>& gridPointss, const GridMap& gridMap, QVector<QVector<QPointF>>& pointss);

/**
 * ����ŷ����þ����ƽ��
 * @param x1 ��һ�����x����
 * @param y1 ��һ�����y����
 * @param x2 �ڶ������x����
 * @param y2 �ڶ������y����
 * @return ŷ����þ����ƽ��
 */
int euclideanDistance2(int x1, int y1, int x2, int y2);

/**
 * ����㵽�߶εĴ�ֱ����
 * @param point ��
 * @param start �߶����
 * @param end �߶��յ�
 * @return �㵽�߶εĴ�ֱ����
 */
double pointToSegmentDistance(const QPointF& point, const QPointF& start, const QPointF& end);

/**
 * �жϵ��Ƿ����������ߵľ���С�ڸ���ֵ
 * @param point ��
 * @param boundaryPointss ���߼���
 * @param distance �����ľ�����ֵ
 * @return ��������ĳ������С����ֵ������true
 */
bool isPointCloseToAnyPolyline(const QPointF& point, const QVector<QVector<QPointF>>& boundaryPointss, double distance);

/**
 * ʹ�ñ����㷨��Ǳ߽��
 * @param pointss ����㼯
 * @param gridMap ����ӳ��
 * @param r �����С
 * @param boundaryPointss ����߽�㼯��
 */
void markBoundaryPointsBruteForce(const QVector<QVector<QPointF>>& pointss, const GridMap& gridMap, double r, QVector<QVector<QPoint>>& boundaryPointss);

/**
 * Douglas-Peucker�㷨�����߼�
 * @param points �㼯
 * @param epsilon �򻯾���
 */
void douglasPeucker(QVector<QPointF>& points, double epsilon = 4);

/**
 * ���㻺�����߽磬ʹ��դ���㷨
 * @param pointss ����㼯
 * @param r �������뾶
 * @param boundaryPointss ����������߽�㼯��
 * @return ����ɹ�������true
 */
bool computeBufferBoundaryWithGrid(const QVector<QVector<QPointF>>& pointss, double r, QVector<QVector<QPointF>>& boundaryPointss);




bool computeBufferBoundary(BufferCalculationMode mode, const QVector<QVector<QPointF>>& pointss, double r, QVector<QVector<QPointF>>& boundaryPointss);