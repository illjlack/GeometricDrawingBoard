#include "GeoMathUtil.h"
#include <QVector>
#include <QPointF>
#include <cmath>

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
                continue;  // ������ߣ������õ�
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
            continue;  // ������ߣ������õ�
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