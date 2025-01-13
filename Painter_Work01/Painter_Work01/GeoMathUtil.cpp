#include "GeoMathUtil.h"
#include <QVector>
#include <QPointF>
#include <cmath>
#include <QRectF>
#include <queue>
#include <stack>
#include <QApplication>
#include "comm.h"
#include <unordered_map>
#include <QPainterPath>


// ==========================================================================
// �����߶��ϵĵ�
// ==========================================================================

/**
 * ���ݽڵ����ͺͿ��Ƶ�������ϵĵ�
 * @param nodeLineStyle �ߵ����ͣ��������ߡ�����������Բ���ȣ�
 * @param controlPoints ���Ƶ��б����ڼ����ߵ���״
 * @param linePoints ����������������õ����߶��ϵĵ�
 * @param steps ����������������ٸ��м��
 * @return ����ɹ������򷵻� true��ʧ���򷵻� false
 */
bool calculateLinePoints(NodeLineStyle lineStyle, const QVector<QPointF>& controlPoints, QVector<QPointF>& linePoints, int steps)
{
    linePoints.clear();
    switch (lineStyle)
    {

    case NodeLineStyle::StyleStreamline: // ���ߺ�����һ��ֱ��ʹ�ÿ��Ƶ�
    case NodeLineStyle::StylePolyline:
        if (controlPoints.size() < 2)
        {
            return false;
        }
        linePoints = controlPoints;
        return true;

    case NodeLineStyle::StyleSpline:
        return calculateBSplineCurve(controlPoints, 3, steps, linePoints);

    case NodeLineStyle::StyleThreePointArc:
        if (controlPoints.size() < 3)
        {
            return false;
        }
        return calculateThreeArcPointsFromControlPoints(controlPoints, steps, linePoints);

    case NodeLineStyle::StyleArc:
        if (controlPoints.size() < 3)
        {
            return false;
        }
        return calculateArcPointsFromControlPoints(controlPoints, steps, linePoints);
    default:
        return false;
    }
}

/**
 * ���ݽڵ����ͺͿ��Ƶ����պ����ϵĵ㣨������һ�±պϣ�
 * @param lineStyle �ߵ�����
 * @param controlPoints ���Ƶ��б�
 * @param linePoints ����������������õ��ıպ����ϵĵ�
 * @param steps ����������������ٸ��м��
 * @return ����ɹ������򷵻� true��ʧ���򷵻� false
 */
bool calculateCloseLinePoints(NodeLineStyle lineStyle, const QVector<QPointF>& controlPoints, QVector<QPointF>& linePoints, int steps)
{
    linePoints.clear();
    QVector<QPointF> newControlPoints;

    switch (lineStyle)
    {
    case NodeLineStyle::StyleStreamline:  // ���ߺ�����һ��ֱ��ʹ�ÿ��Ƶ�
    case NodeLineStyle::StylePolyline:
        if (controlPoints.size() < 3)
        {
            return false;
        }
        newControlPoints = controlPoints;
        newControlPoints.push_back(newControlPoints[0]);
        linePoints = newControlPoints;
        return true;

    case NodeLineStyle::StyleSpline:
        if (controlPoints.size() < 4)
        {
            return false;
        }
        newControlPoints = controlPoints;
        newControlPoints.push_back(newControlPoints[0]);
        newControlPoints.push_back(newControlPoints[0]);
        newControlPoints.push_back(newControlPoints[0]);
        return calculateBSplineCurve(newControlPoints, 3, steps, linePoints);

    case NodeLineStyle::StyleThreePointArc:
        if (controlPoints.size() < 3)
        {
            return false;
        }

        if ((controlPoints.size() - 1) & 1)  // ��һ�����һ�����Ƶ�һ��һ��Բ
        {
            newControlPoints = controlPoints;
            newControlPoints.push_back(controlPoints[0]);
            return calculateThreeArcPointsFromControlPoints(newControlPoints, steps, linePoints);
        }
        else
        {
            if (calculateThreeArcPointsFromControlPoints(controlPoints, steps, linePoints))
            {
                linePoints.push_back(linePoints[0]); // ֱ�����ϵ�һ����
                return true;
            }
            return false;
        }

    case NodeLineStyle::StyleArc:
        if (controlPoints.size() < 3)
        {
            return false;
        }
        newControlPoints = controlPoints;
        newControlPoints.push_back(controlPoints[0]);
        return calculateArcPointsFromControlPoints(newControlPoints, steps, linePoints);

    case NodeLineStyle::StyleTwoPointCircle:
        if (controlPoints.size() < 2)
        {
            return false;
        }
        return calculateCirclePointsFromControlPoints(controlPoints[0], controlPoints[1], steps, linePoints);

    default:
        return false;
    }
}

/**
 * ���ݶ���߶ε�����Ϳ��Ƶ�������ϵĵ�
 * @param component ����б�
 * @param controlPoints ���Ƶ��б�
 * @param linePointss ����������������õ����߶��ϵĵ�
 * @param steps ����������������ٸ��м��
 * @return ���سɹ����Ʒ�ͼ�ĸ���
 */
int calculateLinePoints(const QVector<Component>& components, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps)
{
    linePointss.clear();  // ��ս������
    int startIdx = 0;
    int result = 0; // �������߻������

    for (const Component& component : components)
    {
        QVector<QPointF> linePoints;
        QVector<QPointF> controlSegment;

        if (startIdx + component.len > controlPoints.size())
        {
            return false;
        }

        for (int i = startIdx; i < startIdx + component.len; i++)
        {
            controlSegment.push_back(controlPoints[i]);
        }
        startIdx += component.len;

        result += calculateLinePoints(component.nodeLineStyle, controlSegment, linePoints, steps);
        linePointss.push_back(linePoints);
    }
    return result;
}

/**
 * ���ݶ���߶ε�����Ϳ��Ƶ����պ����ϵĵ�
 * @param component ����б�
 * @param controlPoints ���Ƶ��б�
 * @param linePointss ����������������õ��ıպ����ϵĵ�
 * @param steps ����������������ٸ��м��
 * @return ���سɹ����Ʒ�ͼ�ĸ���
 */
int calculateCloseLinePoints(const QVector<Component>& components, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps)
{
    linePointss.clear();  // ��ս������
    int startIdx = 0;
    int result = 0; // �������߻������

    for (const Component& component : components)
    {
        QVector<QPointF> linePoints;
        QVector<QPointF> controlSegment;

        if (startIdx + component.len > controlPoints.size())
        {
            return false;
        }

        for (int i = startIdx; i < startIdx + component.len; i++)
        {
            controlSegment.push_back(controlPoints[i]);
        }
        startIdx += component.len;

        result += calculateCloseLinePoints(component.nodeLineStyle, controlSegment, linePoints, steps);
        linePointss.push_back(linePoints);
    }
    return result;
}


/**
 * ���ݶ���߶ε�����Ϳ��Ƶ����ƽ���ߣ��ڶ�������һ���������ƾ��룩
 * @param component ����б�
 * @param controlPoints ���Ƶ��б�
 * @param linePointss ����������������õ��ıպ����ϵĵ�
 * @param steps ����������������ٸ��м��
 * @return ���سɹ����Ʒ�ͼ�ĸ���
 */
int calculateParallelLinePoints(const QVector<Component>& components, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps)
{
    linePointss.clear();  // ��ս������
    int startIdx = 0;
    int result = 0; // �������߻������

    if (components.size() >= 1)
    {
        QVector<QPointF> linePoints;
        QVector<QPointF> controlSegment;
        if (startIdx + components[0].len > controlPoints.size())
        {
            return false;
        }
        for (int i = startIdx; i < startIdx + components[0].len; i++)
        {
            controlSegment.push_back(controlPoints[i]);
        }
        startIdx += components[0].len;

        result += calculateLinePoints(components[0].nodeLineStyle, controlSegment, linePoints, steps);
        linePointss.push_back(linePoints);
    
    
        if (components.size() == 2 && components[1].len) // ����ƽ����
        {
            QVector<QPointF> linePoints;
            result += calculateParallelLineThroughPoint(linePointss[0], controlPoints.back(),linePoints);
            linePointss.push_back(linePoints);
        }
    }
    return result;
}


// ==========================================================================
// ��������
// ==========================================================================

/**
 * �����������n ѡ i���������� C(n, i)
 * @param n ����
 * @param i ѡ����
 * @return ������������ֵ
 */
int binomialCoefficient(int n, int i)
{
    int res = 1;
    for (int j = 0; j < i; ++j) {
        res *= (n - j);
        res /= (j + 1);
    }
    return res;
}

/**
 * ����n�α��������ߵĵ㣨��ά��
 * @param controlPoints ���������ߵĿ��Ƶ��б�
 * @param t ��ֵ���ӣ�ͨ����[0, 1]֮��仯
 * @return �����ϵ�һ����
 */
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

/**
 * ����n�α����������ϵĶ���㣨��ά��
 * @param controlPoints ���������ߵĿ��Ƶ��б�
 * @param numPoints ������ٸ����ߵ�
 * @return �������м���õ��ı��������ߵ���б�
 */
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

/**
 * Cox-de Boor �ݹ鹫ʽ�����ڼ��� B ���������ϵĵ�
 * @param knots �ڵ�����
 * @param i ��ǰ�ڵ������
 * @param p B �����Ĵ���
 * @param t ��ֵ����
 * @return ��ǰ��ֵλ�õ�Ȩ��
 */
double coxDeBoor(const QVector<double>& knots, int i, int p, double t)
{
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

/**
 * ���� B �������ߵĽڵ�����
 * @param n ���Ƶ���
 * @param degree ���ߵĴ���
 * @return ���ɵĽڵ�����
 */
QVector<double> generateKnotVector(int n, int degree)
{
    int m = n + degree + 2;  // �ڵ�������С
    QVector<double> knots(m);

    for (int i = 0; i < m; ++i) {
        if (i <= degree) {
            knots[i] = 0.0;  // ǰ degree+1 ��ֵΪ 0
        }
        else if (i >= m - degree - 1) {
            knots[i] = 1.0;  // �� degree+1 ��ֵΪ 1
        }
        else {
            knots[i] = (i - degree) / static_cast<double>(n - degree + 1);  // �м���ȷֲ�
        }
    }

    return knots;
}

/**
 * ���� B ���������ϵĶ���㣨��ά��
 * @param controlPoints ���Ƶ��б�
 * @param degree B �����Ĵ���
 * @param numCurvePoints Ҫ��������ߵ���
 * @param curvePoints ����������������õ������ߵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateBSplineCurve(const QVector<QPointF>& controlPoints, int degree, int numCurvePoints, QVector<QPointF>& curvePoints)
{
    // ���Ƶ�����С��degreeʱ���޷�����B��������
    int n = controlPoints.size() - 1;
    if (n < degree) {
        return false;  // �޷�������������
    }

    // ���ɽڵ�����
    QVector<double> knots = generateKnotVector(n, degree);

    // ���������ϵĵ�
    for (int i = 0; i < numCurvePoints; ++i) 
    {
        double t = i / static_cast<double>(numCurvePoints);  // t ��Χ [0, 1]

        QPointF point(0.0, 0.0);

        // �����Ƶ�����Ȩֵ
        for (int j = 0; j <= n; ++j) 
        {
            double weight = coxDeBoor(knots, j, degree, t);
            point += weight * controlPoints[j];
        }

        curvePoints.append(point);  // �洢����õ������ߵ�
    }

    return true;  // ����ɹ�
}

// ==========================================================================
// Բ������
// ==========================================================================

/**
 * �淶���Ƕȣ�ȷ���Ƕ���[0, 2��)��Χ��
 * @param angle ����Ƕȣ������ƣ�
 * @return �淶����ĽǶȣ������ƣ�
 */
double normalizeAngle(double angle)
{
    while (angle < 0)
    {
        angle += 2 * M_PI;
    }
    while (angle >= 2 * M_PI)
    {
        angle -= 2 * M_PI;
    }
    return angle;
}

/**
 * �������������Բ�ĺͰ뾶
 * @param p1 Բ�ϵĵ�һ����
 * @param p2 Բ�ϵĵڶ�����
 * @param p3 Բ�ϵĵ�������
 * @param center �������������õ���Բ������
 * @param radius �������������õ���Բ�İ뾶
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateCircle(const QPointF& p1, const QPointF& p2, const QPointF& p3, QPointF& center, double& radius)
{
    if (p1 == p2 || p2 == p3 || p3 == p1)
    {
        return false;  // ���غϣ��޷�����Բ
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
        slope1 = std::numeric_limits<double>::infinity();
        c1 = mid1_x;  // �д���Ϊ x = mid1_x
    }
    else
    {
        slope1 = -a1 / b1;
        c1 = mid1_y - slope1 * mid1_x;
    }

    if (b2 == 0) {  // p2 �� p3 ��ֱ��
        slope2 = std::numeric_limits<double>::infinity();
        c2 = mid2_x;  // �д���Ϊ x = mid2_x
    }
    else
    {
        slope2 = -a2 / b2;
        c2 = mid2_y - slope2 * mid2_x;
    }

    // ���㽻��
    if (slope1 == slope2)
    {
        return false; // ���㹲�ߣ��޷�ȷ��ΨһԲ
    }

    double center_x = (c2 - c1) / (slope1 - slope2);
    double center_y = slope1 * center_x + c1;

    center = QPointF(center_x, center_y);
    radius = std::sqrt(std::pow(center.x() - x1, 2) + std::pow(center.y() - y1, 2));  // ����뾶
    return true;
}

/**
 * �������������Բ�ĺͰ뾶
 * @param p1 Բ�ϵĵ�һ����
 * @param p2 Բ�ϵĵڶ�����
 * @param center �������������õ���Բ������
 * @param radius �������������õ���Բ�İ뾶
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateCircle(const QPointF& p1, const QPointF& p2, QPointF& center, double& radius)
{
    double center_x = (p1.x() + p2.x()) / 2.0;
    double center_y = (p1.y() + p2.y()) / 2.0;

    center = QPointF(center_x, center_y);
    radius = std::sqrt(std::pow(p2.x() - p1.x(), 2) + std::pow(p2.y() - p1.y(), 2)) / 2.0;

    return true;
}

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
bool calculateArcPoints(const QPointF& center, double radius, double startAngle, double angleDiff, int steps, QVector<QPointF>& points)
{
    if (steps <= 0 || radius <= 0)
    {
        return false;
    }

    // ����ǶȲ���
    double angleStep = angleDiff / steps;

    for (int i = 0; i <= steps; ++i)
    {
        double angle = normalizeAngle(startAngle + i * angleStep);

        double x = center.x() + radius * std::cos(angle);
        double y = center.y() + radius * std::sin(angle);

        points.append(QPointF(x, y));
    }
    return true;
}

/**
 * ������������㻡���ϵĵ�
 * @param point1 Բ�ϵĵ�һ����
 * @param point2 Բ�ϵĵڶ�����
 * @param point3 Բ�ϵĵ�������
 * @param steps ����������������ٸ���
 * @param arcPoints ����������������õ��Ļ����ϵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateArcPointsFromThreePoints(const QPointF& point1, const QPointF& point2, const QPointF& point3, int steps, QVector<QPointF>& arcPoints)
{
    QPointF center;
    double radius;

    if (!calculateCircle(point1, point2, point3, center, radius))
    {
        return false;
    }

    double startAngle = std::atan2(point1.y() - center.y(), point1.x() - center.x());
    double endAngle = std::atan2(point3.y() - center.y(), point3.x() - center.x());
    double middleAngle = std::atan2(point2.y() - center.y(), point2.x() - center.x());

    startAngle = normalizeAngle(startAngle);
    endAngle = normalizeAngle(endAngle);
    middleAngle = normalizeAngle(middleAngle);

    double angleDiffEnd = normalizeAngle(endAngle - startAngle);
    double angleDiffMid = normalizeAngle(middleAngle - startAngle);

    double angleDiff;
    if (angleDiffEnd > angleDiffMid)
    {
        angleDiff = angleDiffEnd;
    }
    else
    {
        angleDiff = angleDiffEnd - 2 * M_PI;
    }

    return calculateArcPoints(center, radius, startAngle, angleDiff, steps, arcPoints);
}

/**
 * �������������Բ�ϵĵ�
 * @param point1 Բ�ϵĵ�һ����
 * @param point2 Բ�ϵĵڶ�����
 * @param steps ����������������ٸ���
 * @param arcPoints ����������������õ���Բ�ϵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateCirclePointsFromControlPoints(const QPointF& point1, const QPointF& point2, int steps, QVector<QPointF>& arcPoints)
{
    QPointF center;
    double radius;

    // ׷��
    // arcPoints.clear();

    if (!calculateCircle(point1, point2, center, radius)) {
        return false;
    }

    // ������ʼ�����������м��ĽǶȣ�������ֱ�Ӽ���
    double startAngle = std::atan2(point1.y() - center.y(), point1.x() - center.x());

    return calculateArcPoints(center, radius, startAngle, 2 * M_PI, steps, arcPoints);
}

/**
 * ����Բ���ϵĵ㣨ͨ�����Ƶ㣩
 * @param controlPoints ���Ƶ��б�
 * @param steps ����������������ٸ���
 * @param arcPoints ����������������õ��Ļ����ϵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateArcPointsFromControlPoints(const QVector<QPointF>& controlPoints, int steps, QVector<QPointF>& arcPoints)
{
    if (!calculateArcPointsFromThreePoints(controlPoints[0], controlPoints[1], controlPoints[2], steps, arcPoints))
    {
        return false;
    }

    for (int i = 2; i + 1 < controlPoints.size(); i++)
    {
        // ����ʧ�ܾ�ֱ�ӷ���
        if (!calculateArcPointsFromThreePoints(arcPoints[arcPoints.size() - 2], controlPoints[i], controlPoints[i + 1], steps, arcPoints))
        {
            return true;
        }
    }
    return true;
}

/**
 * ��������Բ���ϵĵ㣨ͨ�����Ƶ㣩
 * @param controlPoints ���Ƶ��б�
 * @param steps ����������������ٸ���
 * @param arcPoints ����������������õ��Ļ����ϵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateThreeArcPointsFromControlPoints(const QVector<QPointF>& controlPoints, int steps, QVector<QPointF>& arcPoints)
{
    bool result = false;
    for (int i = 0; i + 2 < controlPoints.size(); i += 2)
    {
        if (calculateArcPointsFromControlPoints({ controlPoints[i], controlPoints[i + 1], controlPoints[i + 2] }, steps, arcPoints))
        {
            result = true;
        }
    }
    return result;
}

/**
 * ����Բ���ĽǶȲ�ֵ��ȷ����ָ���ĽǶȷ�Χ��
 * @param angleDiff �ǶȲ�ֵ
 * @param angleLimit �Ƕȷ�Χ
 * @return ������ĽǶȲ�ֵ
 */
double adjustArcAngleDiff(double angleDiff, double angleLimit)
{
    if (angleDiff > angleLimit)
    {
        return angleDiff - 2 * M_PI;
    }
    return angleDiff;
}



// ==========================================================================
// ƽ���߼���
// ֱ�߷���ͳһ�Ӻ���ĵ�ָ��ǰ��ĵ�
// ==========================================================================

// ���ȴ���
int sgn(double x) {
    if (x < -EPS) return -1;
    else if (x > EPS) return 1;
    else return 0;
}

// ������׼��
std::pair<double, double> normalize(double x, double y) {
    double dis = std::sqrt(x * x + y * y);
    return { x / dis, y / dis };
}

// �������
// ���������������������жϷ��� ����Ĵָ�⣬ ��ָ��1������2����ת�� ��ʱ��Ĵָ�����ǲ�˽������ ����Ϊ��;���Ļ���2��1�����
double cross(double x1, double y1, double x2, double y2) {
    return x1 * y2 - x2 * y1;
}

/**
 * �������ߵ�˫��ƽ����
 * @param polyline �������ߵĵ��б�
 * @param dis ƽ���������ߵľ���
 * @param leftPolyline �������������ƽ���ߵ�����
 * @param rightPolyline �������������ƽ���ߵ��Ҳ��
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateParallelLine(const QVector<QPointF>& polyline, double dis, QVector<QPointF>& leftPolyline, QVector<QPointF>& rightPolyline)
{
    // ˼·��ƽ���߶δ���������disԶ�ĵ㣨ʵ��ֻ���������յ㣩���нǴ�������ƽ���߶��ཻ�����ƽ���ߴ�����dis/sin(һ��н�)���ĵ�
    // 
    // �ж����ߵ����Ƿ���Ч�����߱���������������
    int plLen = polyline.size();
    if (plLen < 2)
    {
        return false; // ����������Ҫ������
    }

    // ����������
    // leftPolyline.clear();
    // rightPolyline.clear();

    // �������ߵ�ÿ���㣬����ƽ����
    for (int i = 0; i < plLen; ++i)
    {
        if (i == 0)
        {
            // ��������ߵ����
            double x1 = polyline[i].x(), y1 = polyline[i].y();
            double x2 = polyline[i + 1].x(), y2 = polyline[i + 1].y();

            // ��һ���㵽�ڶ�����Ĵ�ֱ������������������
            // (y1 - y2, x1 - x2) ��ֱ�� (y1 - y2, x2 - x1)
            auto [vx, vy] = normalize(y1 - y2, x2 - x1);

            leftPolyline.append(QPointF(x1 + vx * dis, y1 + vy * dis));  // ����ƫ��
            rightPolyline.append(QPointF(x1 - vx * dis, y1 - vy * dis));  // ����ƫ��
        }
        else if (i == plLen - 1)
        {
            // ��������ߵ��յ�
            double x1 = polyline[i - 1].x(), y1 = polyline[i - 1].y();
            double x2 = polyline[i].x(), y2 = polyline[i].y();

            auto [vx, vy] = normalize(y1 - y2, x2 - x1);

            leftPolyline.append(QPointF(x2 + vx * dis, y2 + vy * dis));  // ����ƫ��
            rightPolyline.append(QPointF(x2 - vx * dis, y2 - vy * dis));  // ����ƫ��
        }
        else
        {
            // �������ߵ��м��
            double x0 = polyline[i].x(), y0 = polyline[i].y();
            double x1 = polyline[i - 1].x(), y1 = polyline[i - 1].y();
            double x2 = polyline[i + 1].x(), y2 = polyline[i + 1].y();

            // ����ǰ�����������ĵ�λ����
            auto [x01, y01] = normalize(x1 - x0, y1 - y0);
            auto [x02, y02] = normalize(x2 - x0, y2 - y0);

            if (sgn(cross(x01, y01, x02, y02)) == 0)
            {
                continue;  // ������ߣ������õ�
            }

            // �����ƽ���ߵĵ�λ����������������ƽ����
            auto [vx, vy] = normalize((x01 + x02) / 2, (y01 + y02) / 2);

            // �����ƽ���ߵĳ��ȣ�����ȷ��ƽ���ߵ�ƫ����
            double sinX = std::fabs(cross(vx, vy, x02, y02));
            double disBisector = dis / sinX;  // ʹ�ò����ȷ���нǵĴ�С���ó�ƽ���ߵľ���

            // ���ݲ�˵ķ���ȷ�������Ҳ��ƽ����
            if (cross(x1 - x0, y1 - y0, x2 - x0, y2 - y0) > 0)
            {
                leftPolyline.append(QPointF(x0 - vx * disBisector, y0 - vy * disBisector));  // ����ƫ��
                rightPolyline.append(QPointF(x0 + vx * disBisector, y0 + vy * disBisector));  // ����ƫ��
            }
            else
            {
                leftPolyline.append(QPointF(x0 + vx * disBisector, y0 + vy * disBisector));  // ����ƫ��
                rightPolyline.append(QPointF(x0 - vx * disBisector, y0 - vy * disBisector));  // ����ƫ��
            }
        }
    }
    return true;
}

/**
 * �������ߵĵ���ƽ����(ids��ֵΪ��࣬��ֵΪ�Ҳ�)
 * @param polyline �������ߵĵ��б�
 * @param dis ƽ���������ߵľ���
 * @param parallelPolyline ������������浥��ƽ���ߵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateParallelLine(const QVector<QPointF>& polyline, double dis, QVector<QPointF>& parallelPolyline)
{
    // ˼·��ƽ���߶δ���������disԶ�ĵ㣨ʵ��ֻ���������յ㣩���нǴ�������ƽ���߶��ཻ�����ƽ���ߴ�����dis/sin(һ��н�)���ĵ�

    int plLen = polyline.size();
    if (plLen < 2)
    {
        return false; // ����������Ҫ������
    }

    // �������ߵ�ÿ���㣬���㵥��ƽ����
    for (int i = 0; i < plLen; ++i)
    {
        if (i == 0)
        {
            // ��������ߵ����
            double x1 = polyline[i].x(), y1 = polyline[i].y();
            double x2 = polyline[i + 1].x(), y2 = polyline[i + 1].y();

            auto [vx, vy] = normalize(y1 - y2, x2 - x1);// ��dy, -dx�� = p1 - p2 ������ָ��p1

            parallelPolyline.append(QPointF(x1 +  vx * dis, y1 +  vy * dis));
        }
        else if (i == plLen - 1)
        {
            // ��������ߵ��յ�
            double x1 = polyline[i - 1].x(), y1 = polyline[i - 1].y();
            double x2 = polyline[i].x(), y2 = polyline[i].y();

            auto [vx, vy] = normalize(y1 - y2, x2 - x1);

            parallelPolyline.append(QPointF(x2 +  vx * dis, y2 +  vy * dis)); 
        }
        else
        {
            // �������ߵ��м��
            double x0 = polyline[i].x(), y0 = polyline[i].y();
            double x1 = polyline[i - 1].x(), y1 = polyline[i - 1].y();
            double x2 = polyline[i + 1].x(), y2 = polyline[i + 1].y();

            // ����ǰ�����������ĵ�λ����
            auto [x01, y01] = normalize(x1 - x0, y1 - y0);
            auto [x02, y02] = normalize(x2 - x0, y2 - y0);

            if (sgn(cross(x01, y01, x02, y02)) == 0)
            {
                continue;  // ������ߣ������õ�
            }

            // �����ƽ���ߵĵ�λ����������������ƽ����
            auto [vx, vy] = normalize((x01 + x02) / 2, (y01 + y02) / 2);

            // �����ƽ���ߵĳ��ȣ�����ȷ��ƽ���ߵ�ƫ����
            double sinX = std::fabs(cross(vx, vy, x02, y02));
            double disBisector = dis / sinX;  // ʹ�ò����ȷ���нǵĴ�С���ó�ƽ���ߵľ���

            // ѡ�������Ҳ��ƽ����
            if (cross(x1 - x0, y1 - y0, x2 - x0, y2 - y0) > 0)
            {
                parallelPolyline.append(QPointF(x0 - vx * disBisector, y0 - vy * disBisector));   // ����ƫ��(���dis�Ǹ���������ƫ����)
            }
            else
            {
                parallelPolyline.append(QPointF(x0 + vx * disBisector, y0 + vy * disBisector));   // ����ƫ��
            }
        }
    }
    return true;
}

 /**
  * ����㵽ֱ�ߵĴ�ֱ���룬�����ش�����Ľ��
  * @param point ��
  * @param lineStart ֱ�ߵ����
  * @param lineEnd ֱ�ߵ��յ�
  * @param direction ����ֵ�����ڱ�ʾ����ֱ�ߵ���һ�࣬��ֵΪ��࣬��ֵΪ�Ҳ࣬0Ϊֱ����
  * @return �㵽ֱ�ߵĴ�ֱ����
  */
double pointToLineDistanceWithDirection(const QPointF& point, const QPointF& lineStart, const QPointF& lineEnd)
{
    // ����ֱ�ߵķ��� Ax + By + C = 0 ��ϵ�� A, B, C
    double A = lineEnd.y() - lineStart.y();  // y2 - y1
    double B = lineStart.x() - lineEnd.x();  // x1 - x2
    double C = lineEnd.x() * lineStart.y() - lineStart.x() * lineEnd.y();  // x2 * y1 - x1 * y2

    // ����㵽ֱ�ߵĴ�ֱ����
    double distance = std::fabs(A * point.x() + B * point.y() + C) / std::sqrt(A * A + B * B);

    // ͨ������жϵ���ֱ�ߵ���һ��
    // ʹ�õ㵽ֱ������������ֱ�ߵķ�������������
    double crossProduct = (point.x() - lineStart.x()) * (lineEnd.y() - lineStart.y()) - (point.y() - lineStart.y()) * (lineEnd.x() - lineStart.x());

    if (crossProduct > 0)
    {
        return distance;
    }
    else if (crossProduct < 0)
    {
        return - distance;
    }

    return distance;
}


/**
 * ����ƽ�����ߣ���ƽ���ߵĵ�һ���߶ξ���ָ���ĵ�
 * @param polyline �������ߵĵ��б�
 * @param targetPoint ָ��Ŀ��㣬ƽ���߽�ͨ���õ�
 * @param parallelPolyline ������������澭��Ŀ����ƽ����
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateParallelLineThroughPoint(const QVector<QPointF>& polyline, const QPointF& targetPoint, QVector<QPointF>& parallelPolyline)
{
    // ԭ�����Ǽ���㵽���һ���߶εľ���,Ȼ��������뻭����ƽ����
    int plLen = polyline.size();
    if (plLen < 2)
    {
        return false; // ����������Ҫ������
    }
    // ����㵽��һ���߶εĴ�ֱ����
    // ֱ�߷���ͳһ�Ӻ���ĵ�ָ��ǰ��ĵ�
    double distance = pointToLineDistanceWithDirection(targetPoint,polyline[plLen - 1],polyline[plLen - 2]);

    // ���ռ���ľ�������ƽ����
    return calculateParallelLine(polyline, distance, parallelPolyline);
}

// ==========================================================================================
// ����������(����ʸ���Ļ����������㷨��ƽ���ߡ���ƽ���ߡ�͹Բ���ǣ��������ཻ������)
// ==========================================================================================

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
    int steps, QVector<QPointF>& arcPoints)
{
    // ���������յ�ĽǶ�
    double startAngle = std::atan2(startPoint.y() - center.y(), startPoint.x() - center.x());
    double endAngle = std::atan2(endPoint.y() - center.y(), endPoint.x() - center.x());

    startAngle = normalizeAngle(startAngle);
    endAngle = normalizeAngle(endAngle);

    // ����ǶȲ���Ȳ
    double angleDiff = normalizeAngle(endAngle - startAngle);


        
    // ����Բ���ϵĵ�
    return calculateArcPoints(center, std::sqrt((center.x() - startPoint.x()) * (center.x() - startPoint.x())
        + (center.y() - startPoint.y()) * (center.y() - startPoint.y())), startAngle, angleDiff, steps, arcPoints);
}


/**
 * �жϵ���������λ�ù�ϵ
 * @param point ��
 * @param vectorStart ���������
 * @param vectorEnd �������յ�
 * @return ���أ�

 */
int pointPositionRelativeToVector(const QPointF& point, const QPointF& vectorStart, const QPointF& vectorEnd)
{
    // �������� AB �� AP �Ĳ��
    // x1*y2 - y1*x2
    //���������������������жϷ��� ����Ĵָ�⣬ ��ָ��1������2����ת�� ��ʱ��Ĵָ�����ǲ�˽������ ����Ϊ��
    double crossProduct = (point.x() - vectorStart.x()) * (vectorEnd.y() - vectorStart.y()) -
        (point.y() - vectorStart.y()) * (vectorEnd.x() - vectorStart.x());

    if (crossProduct > 0)
    {
        return 1;  // �������������
    }
    else if (crossProduct < 0)
    {
        return -1; // �����������Ҳ�
    }

    return 0;  // ����������ͬһֱ����
}

/**
 * �������ߵĻ�����
 * @param polyline �������ߵĵ��б�
 * @param dis ƽ���������ߵľ���
 * @param points �������
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
// �ο���https://zhuanlan.zhihu.com/p/539904045
bool calculateLineBuffer(const QVector<QPointF>& polyline, double dis, QVector<QPointF>& points)
{
    //// �����,ʹʸ��ͼ�ܼ�,���⽻��Խ��
    //QVector<QPointF> newPolyline;
    //for (int i = 0; i < polyline.size(); i++)
    //{
    //    if (!i)newPolyline.push_back(polyline[i]);
    //    else
    //    {
    //        double step = dis/20; // ����
    //        // ��ǰ���ǰһ�����λ��
    //        double dx = polyline[i].x() - polyline[i - 1].x();
    //        double dy = polyline[i].y() - polyline[i - 1].y();
    //        double length = std::sqrt(dx * dx + dy * dy);
    //        // ��λ��������
    //        double ux = dx / length;
    //        double uy = dy / length;

    //        // ���ղ��������
    //        for (double dist = step; dist < length; dist += step)
    //        {
    //            QPointF newPoint = QPointF(
    //                polyline[i - 1].x() + ux * dist,
    //                polyline[i - 1].y() + uy * dist
    //            );
    //            newPolyline.push_back(newPoint);
    //        }

    //        // ���뵱ǰ��
    //        newPolyline.push_back(polyline[i]);
    //    }
    //}

    // ˼·�������ߵ�ƽ���ߣ�����PI�Ľ���Բ�������ȴ������һ�

    int plLen = polyline.size();
    if (plLen < 2)
    {
        return false; // ����������Ҫ������
    }

    // �������ߵ�ÿ����
    for (int i = 0; i < plLen; ++i)
    {
        if (i == 0)
        {
            // ��������ߵ����
            double x1 = polyline[i].x(), y1 = polyline[i].y();
            double x2 = polyline[i + 1].x(), y2 = polyline[i + 1].y();

            auto [vx, vy] = normalize(y1 - y2, x2 - x1);// ��dy, -dx�� = p1 - p2 ������ָ��p1


            //points.append(QPointF(x1 + vx * dis, y1 + vy * dis));  // ��
            calculateArcPointsFromStartEndCenter(QPointF(x1 + vx * dis, y1 + vy * dis), QPointF(x1 - vx * dis, y1 - vy * dis), polyline[i], 20, points);
            //points.append(QPointF(x1 - vx * dis, y1 - vy * dis));  // ����ƫ��
        }
        else if (i == plLen - 1)
        {
            // ��������ߵ��յ�
            double x1 = polyline[i - 1].x(), y1 = polyline[i - 1].y();
            double x2 = polyline[i].x(), y2 = polyline[i].y();

            auto [vx, vy] = normalize(y1 - y2, x2 - x1);

            calculateArcPointsFromStartEndCenter( QPointF(x2 - vx * dis, y2 - vy * dis), QPointF(x2 + vx * dis, y2 + vy * dis), polyline[i], 20, points);
        }
        else
        {
            // �������ߵ��м��
            double x0 = polyline[i].x(), y0 = polyline[i].y();
            double x1 = polyline[i - 1].x(), y1 = polyline[i - 1].y();
            double x2 = polyline[i + 1].x(), y2 = polyline[i + 1].y();

            // ����ǰ�����������ĵ�λ����
            auto [x01, y01] = normalize(x1 - x0, y1 - y0);
            auto [x02, y02] = normalize(x2 - x0, y2 - y0);

            if (sgn(cross(x01, y01, x02, y02)) == 0)
            {
                // ���ǰ���߶εķ�����������,ֱ���÷����������
                auto [vx, vy] = normalize(y01, -x01);// ��dy, -dx�� = p1 - p2 ������ָ��p1
                //points.append(QPointF(x0 - vx * dis, y0 - vy * dis));
                continue;
            }

            // �����ƽ���ߵĵ�λ����������������ƽ����
            auto [vx, vy] = normalize((x01 + x02) / 2, (y01 + y02) / 2);

            // �����ƽ���ߵĳ��ȣ�����ȷ��ƽ���ߵ�ƫ����
            double sinX = std::fabs(cross(vx, vy, x02, y02));
            double disBisector = dis / sinX;  // ʹ�ò����ȷ���нǵĴ�С���ó�ƽ���ߵľ���

            // ѡ�������Ҳ��ƽ����
            if (cross(x1 - x0, y1 - y0, x2 - x0, y2 - y0) > 0) // p2����ֱ࣬�߷�����ָ��p1�ģ��ڻ��ұߣ��Ƕȴ���PI��
            {
                points.append(QPointF(x0 + vx * disBisector, y0 + vy * disBisector));
            }
            else
            {
                auto [vx01, vy01] = normalize(y0 - y1, x1 - x0);
                auto [vx20, vy20] = normalize(y2 - y0, x0 - x2);
                calculateArcPointsFromStartEndCenter(QPointF(x0 + vx01 * dis, y0 + vy01 * dis), QPointF(x0 + vx20 * dis, y0 + vy20 * dis), polyline[i], 20, points);
            }
        }
    }

    // �������ߵ�ÿ����(������)
    for (int i = plLen - 2; i > 0; --i)
    {
        // �������ߵ��м��
        double x0 = polyline[i].x(), y0 = polyline[i].y();
        double x1 = polyline[i + 1].x(), y1 = polyline[i + 1].y();
        double x2 = polyline[i - 1].x(), y2 = polyline[i - 1].y();

        // ����ǰ�����������ĵ�λ����
        auto [x01, y01] = normalize(x1 - x0, y1 - y0);
        auto [x02, y02] = normalize(x2 - x0, y2 - y0);

        if (sgn(cross(x01, y01, x02, y02)) == 0)
        {
            // ���ǰ���߶εķ�����������,ֱ���÷����������
            auto [vx, vy] = normalize(y01, -x01);// ��dy, -dx�� = p1 - p2 ������ָ��p1
            //points.append(QPointF(x0 - vx * dis, y0 - vy * dis));
            continue;
        }

        // �����ƽ���ߵĵ�λ����������������ƽ����
        auto [vx, vy] = normalize((x01 + x02) / 2, (y01 + y02) / 2);

        // �����ƽ���ߵĳ��ȣ�����ȷ��ƽ���ߵ�ƫ����
        double sinX = std::fabs(cross(vx, vy, x02, y02));
        double disBisector = dis / sinX;  // ʹ�ò����ȷ���нǵĴ�С���ó�ƽ���ߵľ���

        // ѡ�������Ҳ��ƽ����
        if (cross(x1 - x0, y1 - y0, x2 - x0, y2 - y0) > 0) // p2����ֱ࣬�߷�����ָ��p1�ģ��ڻ��ұߣ��Ƕȴ���PI��
        {
            points.append(QPointF(x0 + vx * disBisector, y0 + vy * disBisector));
        }
        else
        {
            auto [vx01, vy01] = normalize(y0 - y1, x1 - x0);
            auto [vx20, vy20] = normalize(y2 - y0, x0 - x2);
            calculateArcPointsFromStartEndCenter(QPointF(x0 + vx01 * dis, y0 + vy01 * dis), QPointF(x0 + vx20 * dis, y0 + vy20 * dis), polyline[i], 20, points);
        }
    }
    if (points.size())points.append(points[0]);

    return true;
}

void simpleLine(QVector<QPointF>& points)
{
    int midIndex = points.size() / 2;

    // ���߽�㼯���Ϊǰ������
    QVector<QPointF> firstHalf(points.begin(), points.begin() + midIndex + 1);
    QVector<QPointF> secondHalf(points.mid(midIndex));

    // ʹ��Douglas-Peucker������㼯
    douglasPeucker(firstHalf, 1);
    douglasPeucker(secondHalf, 1);

    // ��յ�ǰ���ϣ������򻯺��ǰ������ϲ�
    points.clear();
    points.append(firstHalf);
    points.append(secondHalf.mid(1));
}

bool computeBufferBoundaryWithVector(const QVector<QVector<QPointF>>& pointss, double r, QVector<QVector<QPointF>>& boundaryPointss)
{

    QPainterPath combinedPath;

    for (auto& points1 : pointss)
    {
        QVector<QPointF>points = points1;
        simpleLine(points);
        QVector<QPointF> tempPoints;
        if (points.size() < 2)continue;
        tempPoints.push_back(points[0]);
        tempPoints.push_back(points[1]);

        for (int i = 1; i < points.size() - 1; ++i)
        {
            double x0 = points[i].x(), y0 = points[i].y();
            double x1 = points[i + 1].x(), y1 = points[i + 1].y();
            double x2 = points[i - 1].x(), y2 = points[i - 1].y();

            // ����ǰ����������������
            auto x01 = x1 - x0, y01 = y1 - y0;
            auto x02 = x2 - x0, y02 = y2 - y0;


            auto computeHalfAngleTan = [](double x1, double y1, double x2, double y2)
            {
                // ��������ģ��
                double len1 = std::sqrt(x1 * x1 + y1 * y1);
                double len2 = std::sqrt(x2 * x2 + y2 * y2);

                // �������Ͳ��
                double dotProduct = x1 * x2 + y1 * y2;          // ���
                double crossProduct = x1 * y2 - y1 * x2;        // ���

                // ���� sin �� cos
                double cosTheta = dotProduct / (len1 * len2);
                double sinTheta = std::fabs(crossProduct) / (len1 * len2);

                // ���� tan(�� / 2)
                return sinTheta / (1 + cosTheta);
            };

            double needLen = r / computeHalfAngleTan(x01, y01, x02, y02); // С��������Ȼ�������ཻ
            if (std::sqrt(x01 * x01 + y01 * y01) < needLen || std::sqrt(x02 * x02 + y02 * y02) < needLen)
            {
                // һ��
                QPainterPath path;
                QVector<QPointF>outPoints;
                calculateLineBuffer(tempPoints, r, outPoints);

                simpleLine(outPoints);//��

                path.moveTo(outPoints[0]);
                for (auto& point : outPoints)
                {
                    path.lineTo(point);
                }
                path.closeSubpath();

                combinedPath = combinedPath.united(path);

                tempPoints.clear();
                tempPoints.push_back(points[i]);
                tempPoints.push_back(points[i + 1]);
            }
            else
            {
                tempPoints.push_back(points[i+1]);
            }
        }
        {
            // һ��
            QPainterPath path;
            QVector<QPointF>outPoints;
            calculateLineBuffer(tempPoints, r, outPoints);

            path.moveTo(outPoints[0]);
            for (auto& point : outPoints)
            {
                path.lineTo(point);
            }
            path.closeSubpath();

            combinedPath = combinedPath.united(path);
        }
    }
    
    // ���ϲ����·��ת��Ϊ����ε㼯
    boundaryPointss.clear();
    for (int i = 0; i < combinedPath.elementCount(); ++i)
    {
        if (combinedPath.elementAt(i).isMoveTo())
        {
            boundaryPointss.push_back({}); // ��ʼ�µĵ㼯
        }
        boundaryPointss.last().append(QPointF(combinedPath.elementAt(i).x, combinedPath.elementAt(i).y));
    }
    
    return !boundaryPointss.isEmpty();

    for (auto& points : pointss)
    {
        boundaryPointss.push_back({});
        calculateLineBuffer(points,r,boundaryPointss.last());
    }
    return true;
}




























// ==========================================================================================
// ����������(����դ��Ļ����������㷨)
// ==========================================================================================

QRectF calculateBounds(const QVector<QVector<QPointF>>& pointss)
{
    if (pointss.isEmpty() || pointss[0].isEmpty())
    {
        return QRectF();
    }
    qreal minX = pointss[0][0].x();
    qreal maxX = pointss[0][0].x();
    qreal minY = pointss[0][0].y();
    qreal maxY = pointss[0][0].y();
    for (auto& points : pointss)
    {
        // �������е㣬���±߽�
        for (const QPointF& point : points)
        {
            if (point.x() < minX) minX = point.x();
            if (point.x() > maxX) maxX = point.x();
            if (point.y() < minY) minY = point.y();
            if (point.y() > maxY) maxY = point.y();
        }
    }
    // �����������ұ߽�
    return QRectF(QPointF(minX, minY), QPointF(maxX, maxY));
}

void getGridMap(const QVector<QVector<QPointF>>& pointss, double r, GridMap& gridMap)
{
    // ����㼯�ϵ��������ұ߽�
    QRectF bounds = calculateBounds(pointss);

    // ����ʱ�临�Ӷȣ��������Ҫ������ 1s ����������1e8��
    // �������ű�����ȷ����������������� 1e6 ����
    double area = (bounds.width() + 2 * r) * (bounds.height() + 2 * r);

    int pointNum = 0;
    for (auto& points : pointss)pointNum += points.size();

    gridMap.scale = std::sqrt(area*pointNum / 10000000.0);  // ���ű���

    // �㷨���Ӷȣ�area*pointNum*scale���������뼶��

    int k = std::round(r / gridMap.scale);

    // ƫ��������������������� (0, 0) ��ʼ
    gridMap.offset = QPointF(bounds.left() - (k + 5) * gridMap.scale, bounds.top() - (k + 5) * gridMap.scale);

    // ��������ߴ磬����һ����������
    gridMap.sizeX = static_cast<int>(bounds.width() / gridMap.scale + 2 * k + 10);
    gridMap.sizeY = static_cast<int>(bounds.height() / gridMap.scale + 2 * k + 10);
}

void restoreFromGrid(const QVector<QVector<QPoint>>& gridPointss, const GridMap& gridMap, QVector<QVector<QPointF>>& pointss)
{
    // ������ά����㼯��
    for (const QVector<QPoint>& gridPoints : gridPointss)
    {
        pointss.push_back(QVector<QPointF>());
        QVector<QPointF>& points = pointss.last();
        for (const QPoint& gridPoint : gridPoints)
        {
            double x = gridPoint.x() * gridMap.scale + gridMap.offset.x();
            double y = gridPoint.y() * gridMap.scale + gridMap.offset.y();
            points.append(QPointF(x, y)); // ��ԭԭʼ�����
        }
    }
}

// n3��������
// ����ŷ����þ���
int euclideanDistance2(int x1, int y1, int x2, int y2)
{
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

// ����㵽�߶εĴ�ֱ����
double pointToSegmentDistance(const QPointF& point, const QPointF& start, const QPointF& end) {
    double x1 = start.x(), y1 = start.y();
    double x2 = end.x(), y2 = end.y();
    double x0 = point.x(), y0 = point.y();

    // �����߶εĳ���
    double dx = x2 - x1;
    double dy = y2 - y1;
    double segmentLength = std::sqrt(dx * dx + dy * dy);

    // ����߶εĳ���Ϊ�㣬���ص㵽���ľ���
    if (segmentLength == 0) {
        return std::sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));
    }

    // ����ͶӰ��ı���t
    double t = ((x0 - x1) * dx + (y0 - y1) * dy) / (segmentLength * segmentLength);
    t = std::max(0.0, std::min(1.0, t));  // ����t��0��1֮��

    // ����ͶӰ�������
    double projection_x = x1 + t * dx;
    double projection_y = y1 + t * dy;

    // ���㲢���ص㵽ͶӰ��ľ���
    return std::sqrt((x0 - projection_x) * (x0 - projection_x) + (y0 - projection_y) * (y0 - projection_y));
}

// �жϵ��Ƿ����������ߵľ���С�ڸ���ֵ
bool isPointCloseToAnyPolyline(const QPointF& point, const QVector<QVector<QPointF>>& boundaryPointss, double distance)
{
    for (const QVector<QPointF>& polyline : boundaryPointss)
    {
        double minDistance = std::numeric_limits<double>::infinity();

        if (polyline.size() == 1)
        {
            double d = euclideanDistance2(point.x(), point.y(), polyline[0].x(), polyline[0].y());
            if (d < distance*distance)
            {
                return true;
            }
        }
             
        // ������ǰ���ߵ�ÿ���߶�
        for (int i = 0; i < polyline.size() - 1; ++i)
        {
            const QPointF& start = polyline[i];
            const QPointF& end = polyline[i + 1];

            // ���㵱ǰ�㵽���߶εĴ�ֱ����
            double d = pointToSegmentDistance(point, start, end);

             // �����ǰ��С�����Ѿ�С����ֵ����ǰ����
            if (d < distance)
            {
                return true;
            }
        }
    }

    return false;
}

void markBoundaryPointsBruteForce(const QVector<QVector<QPointF>>& pointss,const GridMap& gridMap, double r, QVector<QVector<QPoint>>& boundaryPointss)
{
    // ��ȡ����ߴ�ͳ�ʼ�㼯��
    int sizeX = gridMap.sizeX, sizeY = gridMap.sizeY;


    // ��ʼ���������
    std::vector<std::vector<int>> mark(sizeX, std::vector<int>(sizeY, 0));

    int k = std::round(r / gridMap.scale);

    // ����ö�ٶ�ά�����е�ÿ����
    for (int i = 0; i < sizeX; i++) {
        for (int j = 0; j < sizeY; j++) {
            // �ж��Ƿ����κγ�ʼ��ľ��뷶Χ��
            double x = i * gridMap.scale + gridMap.offset.x();
            double y = j * gridMap.scale + gridMap.offset.y();
            if (isPointCloseToAnyPolyline({ x,y }, pointss, r))
            {
                mark[i][j] = 1;
            }
        }
    }

    QVector<QPoint> boundaryPoints;
    // �ҵ��߽��
    const int dx[8] = { 0, 0, 1, -1 , 1, 1, -1, -1 };
    const int dy[8] = { -1, 1, 0, 0 , 1, -1, 1, -1 };

    for (int i = 0; i < sizeX; i++) {
        for (int j = 0; j < sizeY; j++) {
            if (mark[i][j] == 1) { // ����㱻���Ϊ 1
                bool isBoundary = false;
                for (int z = 0; z < 8; z++)
                {
                    int x = i + dx[z], y = j + dy[z];
                    // �����Χ�Ƿ����δ��ǵĵ�
                    if (x < 0 || x >= sizeX || y < 0 || y >= sizeY || mark[x][y]) continue;
                    isBoundary = true;
                    break;
                }
                if (isBoundary) {
                    boundaryPoints.push_back(QPoint(i, j));
                }
            }
        }
    }

    // ���mark,��Ǳ߽�
    for (auto& row : mark)
    {
        std::fill(row.begin(), row.end(), 0);
    }
    for (auto& point : boundaryPoints)
    {
        mark[point.x()][point.y()] = true;
    }

    // =========================================================== dfs�����������
    // ��Ϊ���Ǳպ�·������û�н��㣬dfs�����ѳ�
    // ��ʼ���������
    std::function<void(int, int, QVector<QPoint>&)> dfs = [&](int startX, int startY, QVector<QPoint>& points)
    {
        std::stack<QPoint> stack;
        stack.push(QPoint(startX, startY));

        mark[startX][startY] = 0;
        points.push_back(QPoint(startX, startY));

        while (!stack.empty()) {
            QPoint current = stack.top();
            stack.pop();

            int x = current.x();
            int y = current.y();

            // ���� 8 ������
            for (int i = 0; i < 8; i++) {
                int x1 = x + dx[i];
                int y1 = y + dy[i];
                if (x1 < 0 || x1 >= sizeX || y1 < 0 || y1 >= sizeY || !mark[x1][y1]) {
                    continue;
                }
                mark[x1][y1] = 0;
                stack.push(QPoint(x1, y1));
                points.push_back(QPoint(x1, y1));
            }
        }
    };

    boundaryPointss.clear();
    for (auto& point : boundaryPoints)
    {
        if (mark[point.x()][point.y()])
        {
            boundaryPointss.push_back(QVector<QPoint>());
            dfs(point.x(), point.y(), boundaryPointss.last());
        }
    }
}


void douglasPeucker(QVector<QPointF>& points, double epsilon) {
    if (points.size() < 3) return;  // ��������3�����޷���

    // ��ȡ�����յ�
    QPointF start = points.first();
    QPointF end = points.last();

    // �ҵ�������ĵ�
    double maxDist = 0.0;
    int index = 0;
    for (int i = 1; i < points.size() - 1; ++i) {
        double dist = pointToSegmentDistance(points[i], start, end);
        if (dist > maxDist) {
            maxDist = dist;
            index = i;
        }
    }
    // ��������������ֵepsilon�������õ㲢�ݹ������������
    if (maxDist > epsilon) {
        // �ݹ����
        QVector<QPointF> left(points.begin(), points.begin() + index + 1);
        douglasPeucker(left, epsilon);

        // �ݹ��Ҳ�
        QVector<QPointF> right(points.begin() + index, points.end());
        douglasPeucker(right, epsilon);

        // �ϲ����Ҳ��֣�ȥ���ظ��Ķ˵�
        points.clear();
        points.append(left);
        points.append(right.mid(1)); // ȥ���Ҳ��һ���㣬��Ϊ���Ѿ���������ʱ����
    }
    else {
        // ���������С����ֵepsilon��ֱ�ӱ��������յ㣬��Ϊ�ò����Ѿ��򻯵��㹻�ӽ�
        points.clear();
        points.append(start);  // �������
        points.append(end);    // �����յ�
    }
}

#include <Eigen/Dense>
// ����������ֵ��ϵ��
struct SplineCoefficients {
    QVector<double> a; // ������
    QVector<double> b; // һ����
    QVector<double> c; // ������
    QVector<double> d; // ������
};

SplineCoefficients calculateSplineCoefficients(const QVector<QPointF>& controlPoints) {
    int n = controlPoints.size();
    if (n < 2) {
        return {};
    }

    QVector<double> x(n), y(n);
    for (int i = 0; i < n; ++i) {
        x[i] = controlPoints[i].x();
        y[i] = controlPoints[i].y();
    }

    QVector<double> dx(n - 1), dy(n - 1);
    QVector<QPointF> validPoints;

    // �����ظ���
    for (int i = 0; i < n - 1; ++i) {
        if (x[i] == x[i + 1] && y[i] != y[i + 1]) {
             continue;
        }
        validPoints.append(controlPoints[i]);
    }
    validPoints.append(controlPoints.last());  // ȷ�����һ�������

    n = validPoints.size();
    for (int i = 0; i < n - 1; ++i) {
        dx[i] = validPoints[i + 1].x() - validPoints[i].x();
        dy[i] = validPoints[i + 1].y() - validPoints[i].y();
    }

    Eigen::MatrixXd A = Eigen::MatrixXd::Zero(n, n);
    Eigen::VectorXd r = Eigen::VectorXd::Zero(n);

    // ������� A ������ r
    for (int i = 1; i < n - 1; ++i) {
        A(i, i - 1) = dx[i - 1];
        A(i, i) = 2 * (dx[i - 1] + dx[i]);
        A(i, i + 1) = dx[i];
        r[i] = 3 * (dy[i] / dx[i] - dy[i - 1] / dx[i - 1]);
    }

    // �߽�����
    A(0, 0) = 1;
    A(n - 1, n - 1) = 1;

    // �ⷽ��
    Eigen::VectorXd c = A.colPivHouseholderQr().solve(r);

    SplineCoefficients coeff;
    coeff.a = QVector<double>(y.begin(), y.end());
    coeff.b.resize(n - 1);
    coeff.c = QVector<double>(c.data(), c.data() + c.size());
    coeff.d.resize(n - 1);

    // ���� b �� d
    for (int i = 0; i < n - 1; ++i) {
        coeff.d[i] = (coeff.c[i + 1] - coeff.c[i]) / (3 * dx[i]);
        coeff.b[i] = dy[i] / dx[i] - dx[i] * (2 * coeff.c[i] + coeff.c[i + 1]) / 3;
    }

    return coeff;
}


// ��������������ֵ
void solveCubicSpline(const QVector<QPointF>& controlPoints, QVector<QPointF>& result, int samplesPerSegment = 100) {
    if (controlPoints.size() < 2) {
        result.clear();
        return; // ������Ҫ�������Ƶ�
    }

    QVector<QPointF> newControlPoints;
    for (int i = 0; i < controlPoints.size(); i++)
    {
        if (!i)
        {
            newControlPoints.push_back(controlPoints[i]);
        }
        else if (controlPoints[i].x() != controlPoints[i - 1].x())
        {
            newControlPoints.push_back(controlPoints[i]);
        }
    }

    SplineCoefficients coeff = calculateSplineCoefficients(newControlPoints);
    result.clear();

    if (coeff.a.isEmpty())return;

    // ����ÿ�����䣬���ɲ�ֵ��
    for (int i = 0; i < newControlPoints.size() - 1; ++i) {
        double x0 = newControlPoints[i].x();
        double x1 = newControlPoints[i + 1].x();
        double step = (x1 - x0) / samplesPerSegment;

        for (int j = 0; j < samplesPerSegment; ++j) {
            double dx = j * step;
            double x = x0 + dx;
            double y = coeff.a[i] + coeff.b[i] * dx + coeff.c[i] * dx * dx + coeff.d[i] * dx * dx * dx;
            result.append(QPointF(x, y));
        }
    }

    // ������һ�����Ƶ�
    result.append(newControlPoints.last());
}

// ���㻺�����߽磬ʹ��դ���㷨
bool computeBufferBoundaryWithGrid(const QVector<QVector<QPointF>>& pointss, double r, QVector<QVector<QPointF>>& boundaryPointss)
{
    boundaryPointss.clear();

    // ����뾶�Ƿ�������false
    if (r < 0)
        return false;

    GridMap gridMap;

    // ����ά�㼯��ӳ�䵽����
    getGridMap(pointss, r, gridMap);

    QVector<QVector<QPoint>> gridBoundaryPointss;

    // ʹ�ñ����㷨����ֽ��
    markBoundaryPointsBruteForce(pointss, gridMap, r, gridBoundaryPointss);

    // ������ָ���ά�߽�㼯��
    restoreFromGrid(gridBoundaryPointss, gridMap, boundaryPointss);

    // ��ÿ���߽�㼯�Ͻ���Douglas-Peucker�򻯣���Ӧ��B��������

    // �����scale

    for (auto& points : boundaryPointss)
    {
        int midIndex = points.size() / 2;

        // ���߽�㼯���Ϊǰ������
        QVector<QPointF> firstHalf(points.begin(), points.begin() + midIndex + 1);
        QVector<QPointF> secondHalf(points.mid(midIndex));

        // ʹ��Douglas-Peucker������㼯
        douglasPeucker(firstHalf, gridMap.scale*2);
        douglasPeucker(secondHalf, gridMap.scale * 2);

        // ��յ�ǰ���ϣ������򻯺��ǰ������ϲ�
        points.clear();
        points.append(firstHalf);
        points.append(secondHalf.mid(1));

        // ʹ��B��������ƽ���򻯺�ı߽�㼯
        QVector<QPointF> smoothedPoints;
        if (points.size())
            points.push_back(points[0]); // ��֤�պ�

        calculateBSplineCurve(points, 3, points.size() * 100, smoothedPoints);

        //solveCubicSpline(points, smoothedPoints);

        // �滻ԭʼ�㼯Ϊƽ����ĵ㼯
        points.swap(smoothedPoints);

        // ��֤�պ�
        if (points.size())
            points.push_back(points[0]);
    }

    return true;
}

bool computeBufferBoundary(BufferCalculationMode mode, const QVector<QVector<QPointF>>& pointss, double r, QVector<QVector<QPointF>>& boundaryPointss)
{
    boundaryPointss.clear();
    if (mode == BufferCalculationMode::Raster)return computeBufferBoundaryWithGrid(pointss, r, boundaryPointss);
    else return computeBufferBoundaryWithVector(pointss, r, boundaryPointss);
}