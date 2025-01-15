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
#include <unordered_set>
#include <QPainterPath>
#include <set>

const double EPSILON = 1e-6;

// �Ƚ�������std�����ã�
struct QPointFComparator {
    bool operator()(const QPointF& p1, const QPointF& p2) const {
        if (std::fabs(p1.x() - p2.x()) > EPSILON) {
            return p1.x() < p2.x();
        }
        return p1.y() < p2.y();
    }
};

struct QPointFEqual {
    bool operator()(const QPointF& p1, const QPointF& p2) const {
        return std::fabs(p1.x() - p2.x()) < EPSILON && std::fabs(p1.y() - p2.y()) < EPSILON;
    }
};

// ���ڱȽ� QPointF �Ĺ�ϣ�����
struct QPointFHash
{
    std::size_t operator()(const QPointF& point) const
    {
        return std::hash<double>()(point.x()) ^ std::hash<double>()(point.y());
    }
};

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
 * �淶���Ƕȣ�ȷ���Ƕ���[0, 2��)��Χ�ڣ���֧��˳ʱ��Ϊ��
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
 * ����Բ�ġ��뾶����ʼ�Ƕȡ��ǶȲ�ֵ�Ͳ������㻡���ϵĵ�(������ʱ��)
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
// ����������(����ʸ���Ļ����������㷨��ƽ���ߡ���ƽ���ߡ�͹Բ����,�ں�path)
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

// �����߶γ����Ƿ���������������������򷵻�true��ʾ��Ҫ���
bool isLengthEnough(double x1, double y1, double x2, double y2, double r) {
    
    auto computeHalfAngleTan = [](double x1, double y1, double x2, double y2) {
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
    double needLen = r / computeHalfAngleTan(x1, y1, x2, y2);
    // �����߶γ���
    double len1 = std::sqrt(x1 * x1 + y1 * y1);
    double len2 = std::sqrt(x2 * x2 + y2 * y2);
    return len1 >= needLen && len2 >= needLen;
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

            calculateArcPointsFromStartEndCenter(QPointF(x1 + vx * dis, y1 + vy * dis), QPointF(x1 - vx * dis, y1 - vy * dis), polyline[i], 20, points);
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
                points.append(QPointF(x0 - vx * dis, y0 - vy * dis));
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

// ���ߵĻ�����,�պϰ汾
bool calculateClosedLineBuffer(const QVector<QPointF>& polyline, double dis, QVector<QVector<QPointF>>& lines)
{
    // ˼·������պ����ߣ����ɻ�������������ƽ���ߺ�Բ���ļ���
    int plLen = polyline.size() - 1;
    if (plLen < 3)
    {
        return false; // �պ�����������Ҫ������
    }

    QVector<QPointF> currentLine;

    // �������ߵ�ÿ���㣨�պϴ�����β������
    for (int i = 0; i < plLen; ++i)
    {
        // ��ȡ��ǰ�㡢ǰһ�㡢��һ��
        double x0 = polyline[i].x(), y0 = polyline[i].y();
        double x1 = polyline[(i - 1 + plLen) % plLen].x(), y1 = polyline[(i - 1 + plLen) % plLen].y();
        double x2 = polyline[(i + 1) % plLen].x(), y2 = polyline[(i + 1) % plLen].y();


        // ����ǰ�����������ĵ�λ����
        auto [x01, y01] = normalize(x1 - x0, y1 - y0);
        auto [x02, y02] = normalize(x2 - x0, y2 - y0);

        if (sgn(cross(x01, y01, x02, y02)) == 0)
        {
            // ���ǰ���߶εķ�����������,ֱ���÷����������
            auto [vx, vy] = normalize(y01, -x01);// ��dy, -dx�� = p1 - p2 ������ָ��p1
            currentLine.append(QPointF(x0 - vx * dis, y0 - vy * dis));
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
            currentLine.append(QPointF(x0 + vx * disBisector, y0 + vy * disBisector));
        }
        else
        {
            auto [vx01, vy01] = normalize(y0 - y1, x1 - x0);
            auto [vx20, vy20] = normalize(y2 - y0, x0 - x2);
            calculateArcPointsFromStartEndCenter(QPointF(x0 + vx01 * dis, y0 + vy01 * dis), QPointF(x0 + vx20 * dis, y0 + vy20 * dis), polyline[i], 20, currentLine);
        }
    }
    // �պϵ�ǰ����������ӵ������
    if (!currentLine.isEmpty())
    {
        currentLine.append(currentLine[0]);
        lines.append(currentLine);
    }

    currentLine.clear();
    // �������ߵ�ÿ����(������)
    for (int i = plLen -1; i >= 0; --i)
    {
        // ��ȡ��ǰ�㡢ǰһ�㡢��һ��
        double x0 = polyline[i].x(), y0 = polyline[i].y();
        double x1 = polyline[(i + 1) % plLen].x(), y1 = polyline[(i + 1) % plLen].y();
        double x2 = polyline[(i - 1 + plLen) % plLen].x(), y2 = polyline[(i - 1 + plLen) % plLen].y();

        // ����ǰ�����������ĵ�λ����
        auto [x01, y01] = normalize(x1 - x0, y1 - y0);
        auto [x02, y02] = normalize(x2 - x0, y2 - y0);

        if (sgn(cross(x01, y01, x02, y02)) == 0)
        {
            // ���ǰ���߶εķ�����������,ֱ���÷����������
            auto [vx, vy] = normalize(y01, -x01);// ��dy, -dx�� = p1 - p2 ������ָ��p1
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
            currentLine.append(QPointF(x0 + vx * disBisector, y0 + vy * disBisector));
        }
        else
        {
            auto [vx01, vy01] = normalize(y0 - y1, x1 - x0);
            auto [vx20, vy20] = normalize(y2 - y0, x0 - x2);
            calculateArcPointsFromStartEndCenter(QPointF(x0 + vx01 * dis, y0 + vy01 * dis), QPointF(x0 + vx20 * dis, y0 + vy20 * dis), polyline[i], 20, currentLine);
        }
    }
    // �պϵ�ǰ����������ӵ������
    if (!currentLine.isEmpty())
    {
        currentLine.append(currentLine[0]);
        lines.append(currentLine);
    }


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

// �߶νṹ��
struct Segment
{
    QPointF start, end;

    // ȷ���߶ε����С���յ㣨�� x ����
    Segment(QPointF s, QPointF e)
    {
        if (s.x() > e.x() || (s.x() == e.x() && s.y() > e.y()))
        {
            start = e;
            end = s;
        }
        else
        {
            start = s;
            end = e;
        }

        if (end.x() - start.x() == 0) slope = std::numeric_limits<double>::infinity();
        else slope = (end.y() - start.y()) / (end.x() - start.x());
    }

    double slope; // ������㣬���ظ�����
};

// ��������߶��Ƿ��ཻ
bool doIntersect(const Segment& s1, const Segment& s2)
{
    auto orientation = [](const QPointF& p, const QPointF& q, const QPointF& r)
    {
        double val = (q.y() - p.y()) * (r.x() - q.x()) - (q.x() - p.x()) * (r.y() - q.y());
        if (std::abs(val) < EPSILON) return 0; // ���ߣ�������
        return (val > 0.0) ? 1 : 2; // ˳ʱ�루�Ҳࣩ or ��ʱ�루��ࣩ
    };

    int o1 = orientation(s1.start, s1.end, s2.start);
    int o2 = orientation(s1.start, s1.end, s2.end);
    int o3 = orientation(s2.start, s2.end, s1.start);
    int o4 = orientation(s2.start, s2.end, s1.end);

    // ���ͨ���ཻ�������������߶����ࣩ
    if (o1 != o2 && o3 != o4)
    {
        return true;
    }

    // ��鹲�������ʹ������
    auto onSegment = [](const QPointF& p, const QPointF& q, const QPointF& r)
    {
        return std::min(p.x(), r.x()) - EPSILON <= q.x() && q.x() <= std::max(p.x(), r.x()) + EPSILON &&
            std::min(p.y(), r.y()) - EPSILON <= q.y() && q.y() <= std::max(p.y(), r.y()) + EPSILON;
    };

    // ����߶ι��ߣ���������Ƿ��ص�
    if (o1 == 0 && onSegment(s1.start, s2.start, s1.end)) return true;
    if (o2 == 0 && onSegment(s1.start, s2.end, s1.end)) return true;
    if (o3 == 0 && onSegment(s2.start, s1.start, s2.end)) return true;
    if (o4 == 0 && onSegment(s2.start, s1.end, s2.end)) return true;

    return false; // û���ཻ
}

// ���������߶εĽ���
bool calculateIntersection(const Segment& s1, const Segment& s2, QPointF& point) {
    // �߶��������
    const QPointF& A = s1.start;
    const QPointF& B = s1.end;
    const QPointF& C = s2.start;
    const QPointF& D = s2.end;

    // �������� AB �� CD
    QPointF AB(B.x() - A.x(), B.y() - A.y());
    QPointF CD(D.x() - C.x(), D.y() - C.y());

    // ������
    double cross1 = AB.x() * CD.y() - AB.y() * CD.x(); // AB �� CD �Ĳ��

    // ���ȿ��ƣ����⸡������������
    const double epsilon = 1e-10; // �Զ��徫����ֵ
    if (std::fabs(cross1) < epsilon) {
        return false;
    }

    // ���� AC �� CD �Ĳ��
    QPointF AC(C.x() - A.x(), C.y() - A.y());
    double t = (AC.x() * CD.y() - AC.y() * CD.x()) / cross1;

    // ���ݲ��� t ���㽻��
    double x = A.x() + t * AB.x();
    double y = A.y() + t * AB.y();

    point = { x,y };
    return true;
}


void sweepLineFindIntersections(const QVector<QVector<QPointF>>& pointss, QVector<QPointF>& intersections, bool isArea = true) {

    // �¼�����
    enum EventType { Start, End, Intersection };

    struct Event {
        QPointF point;                          // ��ǰ�¼�������
        int segmentIndex;                       // �����߶ε�����
        int intersecIndex;                      // �ཻ�߶ε�����
        EventType type;                         // �¼�����

        // ���رȽ���������������ȶ��У�
        bool operator>(const Event& other) const {
            if (point.x() == other.point.x())
                return point.y() > other.point.y();
            return point.x() > other.point.x();
        }
    };

    std::vector<Segment> segments;

    // Step 1: �����߶κ��¼���

    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> events; // ���ȶ���

    // ���ڼ�¼ÿ���պ����ߵ��������䣨�����ж϶ε�������ϵ��
    std::vector<std::pair<int, int>> ringRanges;

    int cnt = 0;
    // �պ�����
    for (const auto& points : pointss) {

        int startIndex = cnt;
        for (int i = 0; i < points.size() - 1; ++i) {
            // �����߶�
            Segment segment(points[i], points[i + 1]);
            segments.push_back(segment);
            cnt++;


            // ���������յ��¼�
            Event startEvent{
                segment.start,                                  // �������
                cnt - 1,                                        // �߶�����
                -1,                                             // �������������û�н��㣩
                Start                                           // �¼�����
            };

            Event endEvent{
                segment.end,                                    // �յ�����
                cnt - 1,                                        // �߶�����
                -1,                                             // �����������յ�û�н��㣩
                End                                             // �¼�����
            };

            // ���¼��������ȶ���
            events.push(startEvent);
            events.push(endEvent);
        }
        int endIndex = cnt - 1; // ���һ���߶�����
        ringRanges.push_back({ startIndex, endIndex });
    }

    // Step 2: ɨ���¼���
    // ά��y��˳���Ӧ�Ķ� (y + x + б�� ,�κ�)
    struct CustomComparator {
        bool operator()(const std::pair<QPointF, double>& a, const std::pair<QPointF, double>& b) const {
            // ȷ���Ƚϵ� x ֵ���ϴ�ֵ��
            double x_target = std::max(a.first.x(), b.first.x());

            // ���� a �� b �� x = x_target ���� y ֵ
            double y_a = a.first.y() + a.second * (x_target - a.first.x()); // y = y0 + k * (x - x0)
            double y_b = b.first.y() + b.second * (x_target - b.first.x());

            // �Ƚ� y ֵ
            if (y_a != y_b) {
                return y_a < y_b;
            }

            // �����ͬ����б�ʽ��бȽ�
            return a.second < b.second;
        }
    };
    std::multimap<std::pair<QPointF, double>, int, CustomComparator >statusTree;

    auto isAdjacentSegments = [&](int segIndex1, int segIndex2) {

        if (segIndex2 == segIndex1)return true;
        if (segments[segIndex2].start == segments[segIndex1].start)return true;



        // ʹ�� std::lower_bound ���ҵ�һ��������յ���� segIndex ��λ��
        auto it = std::lower_bound(ringRanges.begin(), ringRanges.end(), segIndex1,
            [](const std::pair<int, int>& range, int value) {
                return range.second < value; // �ҵ�β������ segIndex ������
            });

        const auto& range = *it;

        if (!isArea) // ����Χ�����򣬲�����β����
        {
            return segIndex2 == std::max(segIndex1 - 1, range.first) ||
                segIndex2 == std::min(segIndex1 + 1, range.second);
        }
        else
        {
            int prevSegment = (segIndex1 == range.first) ? range.second : segIndex1 - 1;
            int nextSegment = (segIndex1 == range.second) ? range.first : segIndex1 + 1;

            return (prevSegment == segIndex2 || nextSegment == segIndex2);
        }
    };

    // һ��������һ�ξ��У�̫����������
    std::unordered_set<QPointF, QPointFHash, QPointFEqual> recordedIntersections;

    auto addIntersectionEvent = [&](int seg1, int seg2)
    {
        if (isAdjacentSegments(seg1, seg2))
        {
            return; // ����������ڣ����Լ��㽻��
        }
        if (doIntersect(segments[seg1], segments[seg2])) {
            QPointF intersecPoint;
            if (!calculateIntersection(segments[seg1], segments[seg2], intersecPoint))return;

            if (recordedIntersections.count(intersecPoint))return;
            else recordedIntersections.insert(intersecPoint);

            // ���ͨ�������жϣ���¼������¼�
            intersections.push_back(intersecPoint);
            events.push({ intersecPoint, seg1, seg2, Intersection });


        }
    };

    while (!events.empty()) {
        Event event = events.top();
        events.pop();

        int segmentIdx = event.segmentIndex;

        if (event.type == Start) {
            // �����߶ε� key ֵ
            std::pair<QPointF, double> key = { segments[segmentIdx].start, segments[segmentIdx].slope };

            // �����߶ε�״̬��
            auto inserted = statusTree.insert({ key, segmentIdx });
            auto it = inserted;

            // ����²����߶��������߶��Ƿ��ཻ
            if (it != statusTree.begin()) {
                addIntersectionEvent(segmentIdx, std::prev(it)->second);
            }
            if (std::next(it) != statusTree.end()) {
                addIntersectionEvent(segmentIdx, std::next(it)->second);
            }
        }
        else if (event.type == End) {
            // �����߶ε� key ֵ
            std::pair<QPointF, double> key = { segments[segmentIdx].start, segments[segmentIdx].slope };

            // ����Ҫ�Ƴ����߶�
            auto it = statusTree.find(key);
            if (it != statusTree.end()) {
                // ��ȡ���ڵ��߶�
                auto prevIt = (it == statusTree.begin()) ? statusTree.end() : std::prev(it);
                auto nextIt = std::next(it);

                // ������������߶Σ��������֮���Ƿ��ཻ
                if (prevIt != statusTree.end() && nextIt != statusTree.end()) {
                    addIntersectionEvent(prevIt->second, nextIt->second);
                }
                // ��״̬����ɾ���߶�
                statusTree.erase(it);
            }
        }
        else if (event.type == Intersection) {
            // �������¼�
            int seg1 = event.segmentIndex;
            int seg2 = event.intersecIndex;

            // �����߶ε� key ֵ
            std::pair<QPointF, double> key1 = { segments[seg1].start, segments[seg1].slope };
            std::pair<QPointF, double> key2 = { segments[seg2].start, segments[seg2].slope };

            // ɾ��ԭ���߶�
            // ʹ�� find ����ȷ���Ƿ���״̬���д�����ͬ key ��Ӧ�Ķ�
            auto range1 = statusTree.equal_range(key1);
            // ɾ���ض����߶�
            for (auto it = range1.first; it != range1.second; ++it) {
                if (it->second == seg1) {
                    it = statusTree.erase(it);
                    break;
                }
            }

            auto range2 = statusTree.equal_range(key2);
            for (auto it = range2.first; it != range2.second; ++it) {
                if (it->second == seg2) {
                    it = statusTree.erase(it);
                    break;
                }
            }

            segments[seg1].start = event.point;
            segments[seg2].start = event.point;

            // �������ˣ�����������
            // �ѿ�ʼ����event
            events.push({
                segments[seg1].start,                                   // �������
                seg1,                                                   // �߶�����
                -1,                                                     // �������������û�н��㣩
                Start                                                   // �¼�����
                });
            events.push({
                segments[seg2].start,                                   // �������
                seg2,                                                   // �߶�����
                -1,                                                     // �������������û�н��㣩
                Start                                                   // �¼�����
                });
        }
    }
}


void sweepLineFindIntersections(const QVector<QVector<QVector<QPointF>>>& pointsss, QVector<QPointF>& intersections)
{
    QVector < QVector<QPointF>>polygons;

    for (auto& pointss : pointsss)
    {
        for (auto& points : pointss)
        {
            polygons.push_back(points);
        }
    }
    sweepLineFindIntersections(polygons, intersections);
}

// �ж�һ�����Ƿ����߶���
bool isPointOnSegment(const QPointF& p, const QPointF& start, const QPointF& end, double epsilon = 1e-6)
{
    // �����Ƿ����߶εķ�Χ��
    if (p.x() < std::min(start.x(), end.x()) - epsilon || p.x() > std::max(start.x(), end.x()) + epsilon ||
        p.y() < std::min(start.y(), end.y()) - epsilon || p.y() > std::max(start.y(), end.y()) + epsilon)
    {
        return false;
    }

    // ����Ƿ���
    QLineF line1(start, p);
    QLineF line2(p, end);
    return std::fabs(line1.length() + line2.length() - QLineF(start, end).length()) < epsilon;
}

void splitLineByIntersections(const QVector<QVector<QPointF>>& polygons,
    const QVector<QPointF>& intersectionPoints,
    QVector<QVector<QPointF>>& splitLines,
    QVector<int>& belong)
{
    // ����ÿ������ε�ÿ����
    for (int polygonIndex = 0; polygonIndex < polygons.size(); ++polygonIndex)
    {
        const auto& polygon = polygons[polygonIndex];
        int n = polygon.size();
        splitLines.push_back({});
        belong.push_back(polygonIndex); // ��ʼ�߶����ڵ�ǰ�����

        for (int i = 0; i < n - 1; ++i)
        {
            const QPointF& start = polygon[i];
            const QPointF& end = polygon[i + 1];

            // �ҵ���ǰ���ϵĽ���
            QVector<QPointF> pointsOnSegment;
            for (const QPointF& intersection : intersectionPoints)
            {
                if (isPointOnSegment(intersection, start, end))
                {
                    pointsOnSegment.append(intersection);
                }
            }

            splitLines.last().append(start); // ����ҿ�

            if (pointsOnSegment.isEmpty())
            {
                continue;
            }

            // �������ľ���Խ�������
            std::sort(pointsOnSegment.begin(), pointsOnSegment.end(),
                [&start](const QPointF& p1, const QPointF& p2)
                {
                    return QLineF(start, p1).length() < QLineF(start, p2).length();
                });

            // ���������ĵ������߶�
            for (int j = 0; j < pointsOnSegment.size(); ++j)
            {
                splitLines.last().append(pointsOnSegment[j]); // �����Ƕε������յ�

                // ����һ���߶β���¼����ͼ��
                splitLines.push_back({ pointsOnSegment[j] });
                belong.push_back(polygonIndex); // �����߶�Ҳ���ڵ�ǰ�����
            }
        }
        splitLines.last().append(polygon.last()); // ��Ӷ�������һ����
    }
}

void splitLineByIntersections(const QVector<QVector<QVector<QPointF>>>& pointsss,
    const QVector<QPointF>& intersectionPoints,
    QVector<QVector<QPointF>>& splitLines,
    QVector<int>& belong)
{
    QVector<QVector<QPointF>> polygons;
    QVector<int> belongPolygon;
    for (int i = 0; i < pointsss.size(); i++)
    {
        for (auto& points : pointsss[i])
        {
            polygons.push_back(points);
            belongPolygon.push_back(i);
        }
    }

    // ����ʵ�ʵķָ�����д���
    QVector<int> tempBelong;
    splitLineByIntersections(polygons, intersectionPoints, splitLines, tempBelong);

    // ���� tempBelong �е������� polygonIndices��������յ� belong
    for (int i = 0; i < tempBelong.size(); ++i)
    {
        belong.push_back(belongPolygon[tempBelong[i]]);
    }
}

void reconstructPolygons(const QVector<QVector<QPointF>>& splitLines,
    QVector<QVector<QPointF>>& mergedPolygons)
{
    // ������ÿ������ĳ���ߺ������
    std::unordered_map<QPointF, std::pair<std::vector<int>, std::vector<int>>, QPointFHash, QPointFEqual> table;

    // ������¼ÿ����ĳ���ߺ������
    for (int i = 0; i < splitLines.size(); i++)
    {
        const auto& line = splitLines[i];
        table[line.first()].first.push_back(i); // ��� -> �����
        table[line.last()].second.push_back(i); // �յ� -> �����
    }

    // ��¼��Щ�߶��Ѿ���ʹ��
    std::vector<bool> used(splitLines.size(), false);

    // ���������߶β��ؽ������
    for (int i = 0; i < splitLines.size(); i++)
    {
        if (used[i])
            continue; // �����Ѿ�ʹ�õ��߶�

        QVector<QPointF> polygon;
        int currentLineIndex = i;

        // ���������
        while (true)
        {
            // ��ȡ��ǰ�߶�
            const auto& line = splitLines[currentLineIndex];

            for (auto& point : line) polygon.append(point);

            used[currentLineIndex] = true; // ��ǵ�ǰ�߶���ʹ��

            // ���ҵ�ǰ�߶��յ�ĳ����
            const QPointF& endPoint = line.last();
            auto& outEdges = table[endPoint].first;

            // �ҵ�δ��ʹ�õĳ����
            int nextLineIndex = -1;
            for (int edgeIndex : outEdges)
            {
                if (!used[edgeIndex])
                {
                    nextLineIndex = edgeIndex;
                    break;
                }
            }

            if (nextLineIndex == -1)
            {
                break;
            }

            currentLineIndex = nextLineIndex; // �ƶ�����һ���߶�
        }

        // ��ӵ������
        if (polygon.size() > 2) // ������������ܹ��ɶ����
        {
            mergedPolygons.append(polygon);
        }
    }
}

bool isPointInsidePolygonDirection(const QPointF& point, const QVector<QPointF>& polygon, bool horizontal)
{
    int intersections = 0;
    int n = polygon.size();

    if (n < 3)
    {
        return false; // �Ƿ�����Σ�����3���㣩
    }

    for (int i = 0; i < n; ++i)
    {
        const QPointF& p1 = polygon[i];
        const QPointF& p2 = polygon[(i + 1) % n];

        if (horizontal)
        {
            // ˮƽ���߼��
            if ((p1.y() > point.y()) != (p2.y() > point.y()))
            {
                double xIntersect = p1.x() + (point.y() - p1.y()) * (p2.x() - p1.x()) / (p2.y() - p1.y());
                if (xIntersect > point.x())
                {
                    ++intersections;
                }
            }
        }
        else
        {
            // ��ֱ���߼��
            if ((p1.x() > point.x()) != (p2.x() > point.x()))
            {
                double yIntersect = p1.y() + (point.x() - p1.x()) * (p2.y() - p1.y()) / (p2.x() - p1.x());
                if (yIntersect > point.y())
                {
                    ++intersections;
                }
            }
        }
    }

    return (intersections % 2 == 1);
}

bool isPointInsidePolygon(const QPointF& point, const QVector<QVector<QPointF>>& polygons)
{
    int totalVotes = 0;

    for (const auto& polygon : polygons)
    {
        int horizontalResult = isPointInsidePolygonDirection(point, polygon, true) ? 1 : -1;
        int verticalResult = isPointInsidePolygonDirection(point, polygon, false) ? 1 : -1;

        totalVotes += horizontalResult + verticalResult;
    }

    // ��ͶƱ��� > 0 ��ʾ�ڲ���<= 0 ��ʾ�ⲿ
    return totalVotes > 0;
}

void filterSplitLinesInsidePolygons(const QVector<QVector<QPointF>>& splitLines,
    const QVector<int>& belong,
    const QVector<QVector<QVector<QPointF>>>& polygons,
    QVector<QVector<QPointF>>& filteredSplitLines)
{
    for (int i = 0; i < splitLines.size(); ++i)
    {
        const auto& line = splitLines[i];
        int polygonIndex = belong[i]; // ��ȡ��ǰ�߶���������ε�����

        QPointF midPoint;
        // ���������ϵ�һ��
        if (line.size() > 2)
        {
            midPoint = line[line.size() / 2];
        }
        else midPoint = (line[0] + line[1]) * 0.5;

        bool isInsideOtherPolygon = false;

        // ����е��Ƿ��������պ϶���ε��ڲ�
        for (int j = 0; j < polygons.size(); ++j)
        {
            if (j == polygonIndex)
            {
                continue; // ������ǰ�߶������Ķ����
            }

            if (isPointInsidePolygon(midPoint, polygons[j]))
            {
                isInsideOtherPolygon = true;
                break;
            }
        }

        // ����е㲻������������ڲ����������߶�
        if (!isInsideOtherPolygon)
        {
            filteredSplitLines.append(line);
        }
    }
}

// �ж��������������������������ʽ��
bool isClockwise(const QVector<QPointF>& polygon) {
    double sum = 0.0;
    for (int i = 0; i < polygon.size() - 1; ++i) {
        const auto& p1 = polygon[i];
        const auto& p2 = polygon[i + 1];
        sum += (p2.x() - p1.x()) * (p2.y() + p1.y());
    }
    return sum > 0.0; // >0 ��ʾ˳ʱ�룬<0 ��ʾ��ʱ��
}

void breakLineOnLen(const QVector<QPointF>& points, double r, QVector<QVector<QPointF>>& segments)
{
    if (points.size() < 2) {
        return;
    }

    QVector<QPointF> tempSegment;
    tempSegment.push_back(points[0]);

    auto computeHalfAngleTan = [](double x1, double y1, double x2, double y2) {
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

    for (int i = 1; i < points.size() - 1; ++i) {
        double x0 = points[i].x(), y0 = points[i].y();
        double x1 = points[i + 1].x(), y1 = points[i + 1].y();
        double x2 = points[i - 1].x(), y2 = points[i - 1].y();

        // ����ǰ����������
        double x01 = x1 - x0, y01 = y1 - y0;
        double x02 = x2 - x0, y02 = y2 - y0;

        double needLen = r / computeHalfAngleTan(x01, y01, x02, y02);

        // �ж��Ƿ���Ҫ���
        if (std::sqrt(x01 * x01 + y01 * y01) < needLen || std::sqrt(x02 * x02 + y02 * y02) < needLen) {
            // ��ӵ�ǰ�ε������
            tempSegment.push_back(points[i]);
            segments.push_back(tempSegment);
            tempSegment.clear();
        }
        // ʼ����ӵ�ǰ�㵽��ʱ��
        tempSegment.push_back(points[i]);
    }

    // ������һ��
    tempSegment.push_back(points.last());
    if (!tempSegment.isEmpty()) {
        segments.push_back(tempSegment);
    }

    return;
}

bool computeBufferBoundaryWithVector(const QVector<QVector<QPointF>>& pointss, double r, QVector<QVector<QPointF>>& boundaryPointss)
{
    // ���ཻ��ǰ�������ɻ�����ǰ�ж�
    // Step 0: �������ཻ��,�н�С���ߵĴ�Ϸָ�

    // ��ά,�������,��������棨������������������
    QVector<QVector<QVector<QPointF>>> polygons;

    for (auto& points : pointss)
    {
        QVector<QVector<QPointF>> lines;

        //��Ϊ���Ȳ��������
        breakLineOnLen(points, r, lines);

        // ʹ��ɨ�����㷨�ҵ�����
        QVector<QPointF> intersectionPoints; // �洢���н���
        sweepLineFindIntersections(lines, intersectionPoints, false); // �Զ���ɨ�����㷨����

        QVector<QVector<QPointF>> splitLines;
        QVector<int> belong;

        splitLineByIntersections(lines, intersectionPoints, splitLines, belong); // �ָ��߶�

        for (auto& points : splitLines)
        {
            polygons.push_back({});
            if (points.first() == points.last())
            {
                calculateClosedLineBuffer(points, r, polygons.last());
            }
            else
            {
                polygons.last().push_back({});
                calculateLineBuffer(points, r, polygons.last().last());
            }
        }
    }


    // �Լ��ļ���ܶ������path
    {
        QPainterPath combinedPath;
        for (auto& pointss : polygons)
        {
            QPainterPath path;

            for (auto& points : pointss)
            {
                path.moveTo(points[0]);
                for (int i = 1; i < points.size(); ++i)
                {
                    path.lineTo(points[i]);
                }
            }
            path.closeSubpath();
            combinedPath = combinedPath.united(path);
        }

        for (int i = 0; i < combinedPath.elementCount(); ++i)
        {
            if (combinedPath.elementAt(i).isMoveTo())
            {
                boundaryPointss.push_back({}); // ��ʼ�µĵ㼯
            }
            boundaryPointss.last().append(QPointF(combinedPath.elementAt(i).x, combinedPath.elementAt(i).y));
        }
        return true;
    }


    // Step 1: ʹ��ɨ�����㷨�ҵ�����
    QVector<QPointF> intersectionPoints; // �洢���н���
    sweepLineFindIntersections(polygons, intersectionPoints); // �Զ���ɨ�����㷨����

    qDebug() << L("���������%1").arg(intersectionPoints.size());

    // Step 2: ���ݽ���ָ��߶�
    QVector<QVector<QPointF>> splitLines;
    QVector<int> belong;
    splitLineByIntersections(polygons, intersectionPoints, splitLines, belong); // �ָ��߶�  

    qDebug() << L("�߶θ�����%1").arg(splitLines.size());

    //boundaryPointss = splitLines;
    //return true;

    // Step 3: ����λ�ڶ�����ڲ����߶�
    QVector<QVector<QPointF>> filteredSplitLines;
    filterSplitLinesInsidePolygons(splitLines, belong, polygons, filteredSplitLines); // �㼯 pointss ��ʾԭʼ�����

    //boundaryPointss = filteredSplitLines;
    //return true;

    // Step 4: �ؽ����˽ṹ���ϲ������
    reconstructPolygons(filteredSplitLines, boundaryPointss);

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
    // ���������
    if (pointss.size() == 1 && pointss[0].size() == 1)
    {
        QPointF point = pointss[0][0];
        boundaryPointss.push_back({});
        calculateArcPoints(point, r, 0, 2*M_PI, 20, boundaryPointss.last());
        return true;
    }

    if (mode == BufferCalculationMode::Raster)return computeBufferBoundaryWithGrid(pointss, r, boundaryPointss);
    else return computeBufferBoundaryWithVector(pointss, r, boundaryPointss);
}
