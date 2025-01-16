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
#include <QVector2D>

using Line = QVector<QPointF>;                 // ��ʾһ����
using Polygon = QVector<Line>;                 // ��ʾһ����
using Polygons = QVector<Polygon>;             // ��ʾ�漯��

const double EPSILON = 1e-4;

// ==========================================================================
// MathTools
// ==========================================================================

// �Ƚ�������std�����ã�
struct QPointFComparator 
{
    bool operator()(const QPointF& p1, const QPointF& p2) const 
    {
        if (std::fabs(p1.x() - p2.x()) > EPSILON) 
        {
            return p1.x() < p2.x();
        }
        return p1.y() < p2.y();
    }
};

struct QPointFEqual 
{
    bool operator()(const QPointF& p1, const QPointF& p2) const 
    {
        return std::fabs(p1.x() - p2.x()) < EPSILON && std::fabs(p1.y() - p2.y()) < EPSILON;
    }
};

// ���ڱȽ� QPointF �Ĺ�ϣ�����
struct QPointFHash
{
    std::size_t operator()(const QPointF& point) const
    {
        auto roundedX = std::round(point.x() / EPSILON) * EPSILON;
        auto roundedY = std::round(point.y() / EPSILON) * EPSILON;
        return std::hash<double>()(roundedX) ^ (std::hash<double>()(roundedY) << 1);
    }
};

// ���ȴ���
int sgn(double x) 
{
    if (x < -EPSILON) return -1;
    else if (x > EPSILON) return 1;
    else return 0;
}

// ������׼��
std::pair<double, double> normalize(double x, double y) 
{
    double dis = std::sqrt(x * x + y * y);
    return { x / dis, y / dis };
}

// �������
// ���������������������жϷ��� ����Ĵָ�⣬ ��ָ��1������2����ת�� ��ʱ��Ĵָ�����ǲ�˽������ ����Ϊ��;���Ļ���2��1�����
inline double cross(double x1, double y1, double x2, double y2)
{
    return x1 * y2 - x2 * y1;
}

// �������
// ��˵ļ������壺��ʾ����������ͶӰ��ϵ����ӳ���ǵļнǷ���
inline double dot(double x1, double y1, double x2, double y2)
{
    return x1 * x2 + y1 * y2;
}

// ������������
inline double length(double x, double y)
{
    return std::sqrt(x * x + y * y);
}

// QPointF�汾���
inline double cross(const QPointF& a, const QPointF& b)
{
    return cross(a.x(), a.y(), b.x(), b.y());
}

// QPointF�汾���
inline double dot(const QPointF& a, const QPointF& b)
{
    return dot(a.x(), a.y(), b.x(), b.y());
}

// QPointF�汾����
inline double length(const QPointF& a)
{
    return length(a.x(), a.y());
}

// �жϵ���������λ�ù�ϵ
int pointRelativeToVector(const QPointF& point, const QPointF& vectorStart, const QPointF& vectorEnd)
{
    // �������� AB �� AP �Ĳ��
    double crossProduct = cross(vectorEnd.x() - vectorStart.x(),
        vectorEnd.y() - vectorStart.y(),
        point.x() - vectorStart.x(),
        point.y() - vectorStart.y());

    // ���ݲ���ж�λ�ù�ϵ
    if (std::fabs(crossProduct) < EPSILON) {
        return 0; // ����������
    }
    else if (crossProduct > 0) {
        return 1; // �����������
    }
    else {
        return -1; // ���������Ҳ�
    }
}

// �жϵ� q �Ƿ����߶� pr ��
bool pointOnSegment(const QPointF& q, const QPointF& p, const QPointF& r)
{
    // ������ֱ�ӷ���
    if(pointRelativeToVector(p, q, r))return false; 

    // ��� q �Ƿ��ڵ� p �͵� r �ķ�Χ��
    return (q.x() <= std::max(p.x(), r.x()) && q.x() >= std::min(p.x(), r.x()) &&
        q.y() <= std::max(p.y(), r.y()) && q.y() >= std::min(p.y(), r.y()));
}

// �߶��Ƿ��ཻ�ͼ�������
bool segmentsIntersect(const QPointF& p1Start, const QPointF& p1End,
    const QPointF& p2Start, const QPointF& p2End,
    QPointF& intersection)
{
    // ���㷽��
    int o1 = pointRelativeToVector(p1Start, p1End, p2Start);
    int o2 = pointRelativeToVector(p1Start, p1End, p2End);
    int o3 = pointRelativeToVector(p2Start, p2End, p1Start);
    int o4 = pointRelativeToVector(p2Start, p2End, p1End);

    // ͨ���ཻ���������߶��ڶԷ����ࣩ
    if (o1 != o2 && o3 != o4)
    {
        // ���㽻��
        double a1 = p1End.y() - p1Start.y();
        double b1 = p1Start.x() - p1End.x();
        double c1 = a1 * p1Start.x() + b1 * p1Start.y();

        double a2 = p2End.y() - p2Start.y();
        double b2 = p2Start.x() - p2End.x();
        double c2 = a2 * p2Start.x() + b2 * p2Start.y();

        double determinant = a1 * b2 - a2 * b1;

        if (determinant == 0) 
        {
            return false;  // ƽ���߶�
        }

        // ���㽻������
        intersection.setX((b2 * c1 - b1 * c2) / determinant);
        intersection.setY((a1 * c2 - a2 * c1) / determinant);
        return true;
    }

    // ����������߶ι��������ص�
    if (o1 == 0 && pointOnSegment(p2Start, p1Start, p1End))
    {
        intersection = p2Start;
        return true;
    }
    if (o2 == 0 && pointOnSegment(p2End, p1Start, p1End))
    {
        intersection = p2End;
        return true;
    }
    if (o3 == 0 && pointOnSegment(p1Start, p2Start, p2End))
    {
        intersection = p1Start;
        return true;
    }
    if (o4 == 0 && pointOnSegment(p1End, p2Start, p2End))
    {
        intersection = p1End;
        return true;
    }

    return false;  // û���ཻ
}

// ���Ƿ�������
bool isPointInsidePolygon(const QPointF& point, const Polygon& polygon)
{
    for (const auto& ring : polygon) // ����ÿһ���պϻ����⻷���ڻ���
    {
        int windingNumber = 0; // ����ͳ�Ƶ���������ж��Ƿ��ڶ������

        for (int i = 0; i < ring.size(); ++i)
        {
            QPointF p1 = ring[i];
            QPointF p2 = ring[(i + 1) % ring.size()]; // ��һ����

            // �жϵ��ڱߵ����¹�ϵ
            if (p1.y() <= point.y())
            {
                if (p2.y() > point.y() && cross(p1 - point, p2 - point) > 0)
                {
                    ++windingNumber; // ��������ϴ�������α�
                }
            }
            else
            {
                if (p2.y() <= point.y() && cross(p1 - point, p2 - point) < 0)
                {
                    --windingNumber; // ��������´�������α�
                }
            }
        }

        // ���������Ϊ 0��˵�����ڶ������
        if (windingNumber != 0)
        {
            return true;
        }
    }

    return false; // �㲻�ڶ������
}

double pointToSegmentDistance(const QPointF& point, const QPointF& start, const QPointF& end) {
    // �����߶ε�����
    QPointF segment = end - start;
    QPointF vectorToPoint = point - start;

    // �����߶εĳ���
    double segmentLength = length(segment);

    // ����߶εĳ���Ϊ�㣬���ص㵽���ľ���
    if (segmentLength == 0) {
        return length(vectorToPoint);
    }

    // ����ͶӰ��ı���t
    double t = dot(vectorToPoint, segment) / (segmentLength * segmentLength);
    t = std::max(0.0, std::min(1.0, t));  // ����t��0��1֮��

    // ����ͶӰ�������
    QPointF projection = start + t * segment;

    // ���㲢���ص㵽ͶӰ��ľ���
    return length(point - projection);
}

// �ж��������������������������ʽ��(�����ߵ�˳��)
bool isClockwise(const Line& line) {
    double sum = 0.0;
    for (int i = 0; i < line.size() - 1; ++i) {
        const auto& p1 = line[i];
        const auto& p2 = line[i + 1];
        sum += (p2.x() - p1.x()) * (p2.y() + p1.y());
    }
    return sum > 0.0; // >0 ��ʾ˳ʱ�룬<0 ��ʾ��ʱ��
}


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
bool calculateLinePoints(NodeLineStyle lineStyle, const Line& controlPoints, Line& linePoints, int steps)
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
bool calculateCloseLinePoints(NodeLineStyle lineStyle, const Line& controlPoints, Line& linePoints, int steps)
{
    linePoints.clear();
    Line newControlPoints;

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
int calculateLinePoints(const QVector<Component>& components, const Line& controlPoints, Polygon& linePointss, int steps)
{
    linePointss.clear();  // ��ս������
    int startIdx = 0;
    int result = 0; // �������߻������

    for (const Component& component : components)
    {
        Line linePoints;
        Line controlSegment;

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
int calculateCloseLinePoints(const QVector<Component>& components, const Line& controlPoints, Polygon& linePointss, int steps)
{
    linePointss.clear();  // ��ս������
    int startIdx = 0;
    int result = 0; // �������߻������

    for (const Component& component : components)
    {
        Line linePoints;
        Line controlSegment;

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
int calculateParallelLinePoints(const QVector<Component>& components, const Line& controlPoints, Polygon& linePointss, int steps)
{
    linePointss.clear();  // ��ս������
    int startIdx = 0;
    int result = 0; // �������߻������

    if (components.size() >= 1)
    {
        Line linePoints;
        Line controlSegment;
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
            Line linePoints;
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
QPointF bezierN(const Line& controlPoints, double t)
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
Line calculateBezierCurve(const Line& controlPoints, int numPoints)
{
    Line curvePoints;

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
bool calculateBSplineCurve(const Line& controlPoints, int degree, int numCurvePoints, Line& curvePoints)
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
bool calculateArcPoints(const QPointF& center, double radius, double startAngle, double angleDiff, int steps, Line& points)
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
bool calculateArcPointsFromThreePoints(const QPointF& point1, const QPointF& point2, const QPointF& point3, int steps, Line& arcPoints)
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
bool calculateCirclePointsFromControlPoints(const QPointF& point1, const QPointF& point2, int steps, Line& arcPoints)
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
bool calculateArcPointsFromControlPoints(const Line& controlPoints, int steps, Line& arcPoints)
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
bool calculateThreeArcPointsFromControlPoints(const Line& controlPoints, int steps, Line& arcPoints)
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
/**
 * �������ߵ�˫��ƽ����
 * @param polyline �������ߵĵ��б�
 * @param dis ƽ���������ߵľ���
 * @param leftPolyline �������������ƽ���ߵ�����
 * @param rightPolyline �������������ƽ���ߵ��Ҳ��
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateParallelLine(const Line& polyline, double dis, Line& leftPolyline, Line& rightPolyline)
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
bool calculateParallelLine(const Line& polyline, double dis, Line& parallelPolyline)
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
bool calculateParallelLineThroughPoint(const Line& polyline, const QPointF& targetPoint, Line& parallelPolyline)
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
    int steps, Line& arcPoints)
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

// �����߶γ����Ƿ���������������������򷵻�true��ʾ��Ҫ���
bool isLengthEnough(double x1, double y1, double x2, double y2, double r) {
    
    auto computeHalfAngleTan = [](double x1, double y1, double x2, double y2) {
        // ������������
        double len1 = length(x1, y1);
        double len2 = length(x2, y2);

        // �������Ͳ��
        double dotProduct = dot(x1, y1, x2, y2);
        double crossProduct = std::fabs(cross(x1, y1, x2, y2));

        // ���� sin �� cos
        double cosTheta = dotProduct / (len1 * len2);
        double sinTheta = crossProduct / (len1 * len2);

        // ���� tan(�� / 2)
        return sinTheta / (1 + cosTheta);
    };
    double needLen = r / computeHalfAngleTan(x1, y1, x2, y2);
    // �����߶γ���
    double len1 = length(x1, y1);
    double len2 = length(x2, y2);
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
bool calculateLineBuffer(const Line& polyline, double dis, Line& points)
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
bool calculateClosedLineBuffer(const Line& polyline, double dis, Polygon& lines)
{
    // ˼·������պ����ߣ����ɻ�������������ƽ���ߺ�Բ���ļ���
    int plLen = polyline.size() - 1;
    if (plLen < 3)
    {
        return false; // �պ�����������Ҫ������
    }

    Line currentLine;

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


// ����нǴ�������
void computeAngleBufferIntersection(const QVector2D& v1, const QVector2D& v2, double r, QPointF o, Line& joinedPath)
{
    Line p1, p2;
    // ���㵥λ�����ͷ�����
    QVector2D u1 = v1.normalized();
    QVector2D u2 = v2.normalized();
    QVector2D normal1(-u1.y(), u1.x());
    QVector2D normal2(-u2.y(), u2.x());
    // ���� p1 ��Բ�������յ�
    QVector2D arcStart1 = v1 - normal1 * r;
    QVector2D arcEnd1 = v1 + normal1 * r;
    QVector2D end1 = normal1 * r;
    calculateArcPointsFromStartEndCenter(arcStart1.toPointF(), arcEnd1.toPointF(), v1.toPointF(), 20, p1);
    p1.push_back(end1.toPointF());
    // ���� p2 ��Բ�������յ�
    QVector2D arcStart2 = v2 + normal2 * r;
    QVector2D arcEnd2 = v2 - normal2 * r;
    QVector2D end2 = -normal2 * r;
    p2.push_back(end2.toPointF());
    calculateArcPointsFromStartEndCenter(arcEnd2.toPointF(), arcStart2.toPointF(), v2.toPointF(), 20, p2);
    // Ѱ�ҽ���
    QPointF intersection;
    int p1IntersectIndex = -1, p2IntersectIndex = -1;
    for (int i = 0; i < p1.size() - 1; ++i)
    {
        for (int j = 0; j < p2.size() - 1; ++j)
        {
            QPointF p1Start = p1[i], p1End = p1[i + 1];
            QPointF p2Start = p2[j], p2End = p2[j + 1];

            if (segmentsIntersect(p1Start, p1End, p2Start, p2End, intersection))
            {
                p1IntersectIndex = i;
                p2IntersectIndex = j;
                break;
            }
        }
        if (p1IntersectIndex != -1) break;
    }

    // ����ҵ����㣬�ضϲ�����·��
    if (p1IntersectIndex != -1 && p2IntersectIndex != -1)
    {
        // �� p1 �ضϵ�����
        for (int i = 0; i <= p1IntersectIndex; ++i)
        {
            joinedPath.push_back(p1[i] + o);
        }
        joinedPath.push_back(intersection + o);

        // �� p2 �ӽ��㿪ʼ���
        for (int j = p2IntersectIndex + 1; j < p2.size(); ++j)
        {
            joinedPath.push_back(p2[j] + o);
        }
    }
}


// ����н�С�������߻�����
bool calculateLittleLineBuffer(const Line& polyline, double dis, Line& points)
{
    // ˼·�������ߵ�ƽ���ߣ�����PI�Ľ���Բ�������ȴ������һ�

    int plLen = polyline.size();
    if (plLen != 3)
    {
        return false;
    }

    // ����,������������
    if (pointRelativeToVector(polyline[0], polyline[1], polyline[2]) == 0)
    {
        calculateLineBuffer(polyline, dis, points);
        return true;
    }

    auto draw = [&](QPointF point1, QPointF point2, QPointF point3)
    {
        // �������ߵ��м��
        float x0 = point2.x(), y0 = point2.y();
        float x1 = point1.x(), y1 = point1.y();
        float x2 = point3.x(), y2 = point3.y();

        // ����ǰ�����������ĵ�λ����
        auto [x01, y01] = normalize(x1 - x0, y1 - y0);
        auto [x02, y02] = normalize(x2 - x0, y2 - y0);

        // ѡ�������Ҳ��ƽ����
        if (cross(x1 - x0, y1 - y0, x2 - x0, y2 - y0) > 0) // p2����ֱ࣬�߷�����ָ��p1�ģ��ڻ��ұߣ��Ƕȴ���PI��
        {
            QVector2D v1 = { x1 - x0, y1 - y0 }, v2 = { x2 - x0, y2 - y0 };
            QPointF o = { x0,y0 };
            computeAngleBufferIntersection(v1, v2, dis, o, points);
        }
        else
        {
            auto [vx01, vy01] = normalize(y0 - y1, x1 - x0);
            auto [vx20, vy20] = normalize(y2 - y0, x0 - x2);
            calculateArcPointsFromStartEndCenter(QPointF(x0 + vx01 * dis, y0 + vy01 * dis), QPointF(x0 + vx20 * dis, y0 + vy20 * dis), point2, 20, points);
        }
    };

    draw(polyline[0], polyline[1], polyline[2]);
    draw(polyline[2], polyline[1], polyline[0]);

    points.push_back(points.first());
    return true;
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


void sweepLineFindIntersections(const Polygon& pointss, Line& intersections, bool isArea = true) {

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
        QPointF intersecPoint;
        if (segmentsIntersect(segments[seg1].start, segments[seg1].end, segments[seg2].start, segments[seg2].end, intersecPoint)) 
        {
            // ����Ѿ��н���
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


void sweepLineFindIntersections(const Polygons& pointsss, Line& intersections)
{
    QVector < Line>polygons;

    for (auto& pointss : pointsss)
    {
        for (auto& points : pointss)
        {
            polygons.push_back(points);
        }
    }
    sweepLineFindIntersections(polygons, intersections);
}

void splitLineByIntersections(const Polygon& polygons,
    const Line& intersectionPoints,
    Polygon& splitLines,
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
            Line pointsOnSegment;
            for (const QPointF& intersection : intersectionPoints)
            {
                if (pointOnSegment(intersection, start, end))
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

void splitLineByIntersections(const Polygons& pointsss,
    const Line& intersectionPoints,
    Polygon& splitLines,
    QVector<int>& belong)
{
    Polygon polygons;
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

void reconstructPolygons(const Polygon& splitLines,
    Polygon& mergedPolygons)
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

        Line polygon;
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

void filterSplitLinesInsidePolygons(const Polygon& splitLines,
    const QVector<int>& belong,
    const Polygons& polygons,
    Polygon& filteredSplitLines)
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

void breakLineOnLen(const Line& points, double r, Polygon& segments)
{
    if (points.size() < 2) {
        return;
    }

    QVector<bool> vis(points.size());
    Line tempSegment;           // ��ʱ��

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
        if (std::sqrt(x01 * x01 + y01 * y01) < needLen || std::sqrt(x02 * x02 + y02 * y02) < needLen) 
        {
            vis[i] = true; // ��Ҫ���
        }
    }

    for (int i = 0; i < points.size(); ++i)
    {
        if (tempSegment.size() && vis[i])
        {
            QPointF midPoint1 = (points[i - 1] + points[i]) / 2;
            tempSegment.push_back(midPoint1);
            segments.push_back(tempSegment);
            tempSegment.clear();

            QPointF midPoint2 = (points[i] + points[i + 1]) / 2;
            segments.push_back({ midPoint1, points[i], midPoint2 });

            tempSegment.push_back(midPoint2);
        }
        else
        {
            tempSegment.push_back(points[i]);
        }
    }
    if(tempSegment.size() > 1)segments.push_back(tempSegment);
    return;
}

void breakLineOnIntersections(const Polygon& polygons,
    const Line& intersectionPoints,
    Polygon& splitLines)
{
    // ����ÿ������ε�ÿ����
    for (int polygonIndex = 0; polygonIndex < polygons.size(); ++polygonIndex)
    {
        const auto& polygon = polygons[polygonIndex];
        int n = polygon.size();
        splitLines.push_back({});

        for (int i = 0; i < n - 1; ++i)
        {
            const QPointF& start = polygon[i];
            const QPointF& end = polygon[i + 1];

            if (QPointFEqual()(start, end))continue;

            // �ҵ���ǰ���ϵĽ���
            Line pointsOnSegment;
            for (const QPointF& intersection : intersectionPoints)
            {
                if (QPointFEqual()(intersection, start))continue;
                if (QPointFEqual()(intersection, end))continue;
                if (pointOnSegment(intersection, start, end))
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
                QPointF midPoint = (pointsOnSegment[j] + splitLines.last().last()) / 2;
                splitLines.last().append(midPoint);

                if (QPointFEqual()(midPoint, pointsOnSegment[j]))continue;

                splitLines.push_back({ midPoint, pointsOnSegment[j] });
            }
        }

        splitLines.last().append(polygon.last()); // ��Ӷ�������һ����
    }
}


bool computeBufferBoundaryWithVector(const Polygon& pointss, double r, Polygon& boundaryPointss)
{
    // ���ཻ��ǰ�������ɻ�����ǰ�ж�
    // Step 0: �������ཻ��,�н�С���ߵĴ�Ϸָ�

    // ��ά,�������,��������棨������������������
    Polygons polygons;

    for (auto& points : pointss)
    {
        Polygon lines;

        //��Ϊ���Ȳ��������
        breakLineOnLen(points, r, lines);

        // ʹ��ɨ�����㷨�ҵ�����
        Line intersectionPoints; // �洢���н���
        sweepLineFindIntersections(lines, intersectionPoints, false); // �Զ���ɨ�����㷨����

        Polygon splitLines;

        breakLineOnIntersections(lines, intersectionPoints, splitLines); // �ָ��߶�


        for (auto& points : splitLines)
        {
            polygons.push_back({});
            if (points.size() == 3)
            {
                polygons.last().push_back({});
                calculateLittleLineBuffer(points, r, polygons.last().last());
            }
            else if (points.first() == points.last())
            {
                calculateClosedLineBuffer(points, r, polygons.last());
            }
            else
            {
                polygons.last().push_back({});
                calculateLineBuffer(points, r, polygons.last().last());
            }
        }

        for (auto& polygon : polygons)
            qDebug() << polygon;
    }


    // �Լ��ļ���ܶ������path
    if(false)
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
    Line intersectionPoints; // �洢���н���
    sweepLineFindIntersections(polygons, intersectionPoints); // �Զ���ɨ�����㷨����

    qDebug() << L("��������%1").arg(intersectionPoints.size());

    // Step 2: ���ݽ���ָ��߶�
    Polygon splitLines;
    QVector<int> belong;
    splitLineByIntersections(polygons, intersectionPoints, splitLines, belong); // �ָ��߶�  

    qDebug() << L("�߶�����%1").arg(splitLines.size());

    // Step 3: ����λ�ڶ�����ڲ����߶�
    Polygon filteredSplitLines;
    filterSplitLinesInsidePolygons(splitLines, belong, polygons, filteredSplitLines); // �㼯 pointss ��ʾԭʼ�����

    //boundaryPointss = filteredSplitLines;
    //return true;

    qDebug() << L("���˺��߶�����%1").arg(filteredSplitLines.size());

    // Step 4: �ؽ����˽ṹ���ϲ������
    reconstructPolygons(filteredSplitLines, boundaryPointss);

    qDebug() << L("�պ���������%1").arg(boundaryPointss.size());

    for (int i = 0; i < boundaryPointss.size(); ++i)
    {
        qDebug() << boundaryPointss[i];
    }

    return true;
}


// ==========================================================================================
// ����������(����դ��Ļ����������㷨)
// ==========================================================================================

QRectF calculateBounds(const Polygon& pointss)
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

void getGridMap(const Polygon& pointss, double r, GridMap& gridMap)
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

void restoreFromGrid(const QVector<QVector<QPoint>>& gridPointss, const GridMap& gridMap, Polygon& pointss)
{
    // ������ά����㼯��
    for (const QVector<QPoint>& gridPoints : gridPointss)
    {
        pointss.push_back(Line());
        Line& points = pointss.last();
        for (const QPoint& gridPoint : gridPoints)
        {
            double x = gridPoint.x() * gridMap.scale + gridMap.offset.x();
            double y = gridPoint.y() * gridMap.scale + gridMap.offset.y();
            points.append(QPointF(x, y)); // ��ԭԭʼ�����
        }
    }
}

// �жϵ��Ƿ����������ߵľ���С�ڸ���ֵ
bool isPointCloseToAnyPolyline(const QPointF& point, const Polygon& boundaryPointss, double distance)
{
    for (const Line& polyline : boundaryPointss)
    {
        double minDistance = std::numeric_limits<double>::infinity();

        if (polyline.size() == 1)
        {
            double d = length(point - polyline[0]);
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

void markBoundaryPointsBruteForce(const Polygon& pointss,const GridMap& gridMap, double r, QVector<QVector<QPoint>>& boundaryPointss)
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

void douglasPeucker(Line& points, double epsilon) {
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
        Line left(points.begin(), points.begin() + index + 1);
        douglasPeucker(left, epsilon);

        // �ݹ��Ҳ�
        Line right(points.begin() + index, points.end());
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

void simpleLine(Line& points)
{
    int midIndex = points.size() / 2;

    // ���߽�㼯���Ϊǰ������
    Line firstHalf(points.begin(), points.begin() + midIndex + 1);
    Line secondHalf(points.mid(midIndex));

    // ʹ��Douglas-Peucker������㼯
    douglasPeucker(firstHalf, 1);
    douglasPeucker(secondHalf, 1);

    // ��յ�ǰ���ϣ������򻯺��ǰ������ϲ�
    points.clear();
    points.append(firstHalf);
    points.append(secondHalf.mid(1));
}


// ���㻺�����߽磬ʹ��դ���㷨
bool computeBufferBoundaryWithGrid(const Polygon& pointss, double r, Polygon& boundaryPointss)
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
        simpleLine(points);

        // ʹ��B��������ƽ���򻯺�ı߽�㼯
        Line smoothedPoints;
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

bool computeBufferBoundary(BufferCalculationMode mode, const Polygon& pointss, double r, Polygon& boundaryPointss)
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
