#include "GeoMathUtil.h"
#include "comm.h"
#include "PolygonBuffer.h"
#include <QVector>
#include <QPointF>
#include <cmath>
#include <QRectF>
#include <queue>
#include <stack>
#include <QApplication>
#include <unordered_map>
#include <unordered_set>
#include <QPainterPath>
#include <set>
#include <QVector2D>

using Point = QPointF;
using Line = QVector<Point>;                   // ��ʾһ����
using Polygon = QVector<Line>;                 // ��ʾһ����
using Polygons = QVector<Polygon>;             // ��ʾ�漯��

const double M_PI = 3.14159265358979323846;

const double EPSILON = 1e-2; // float�ľ���̫��

// ==========================================================================
// MathTools
// ==========================================================================

// �Ƚ�������std�����ã�
struct PointComparator 
{
    bool operator()(const Point& p1, const Point& p2) const 
    {
        if (std::fabs(p1.x() - p2.x()) > EPSILON) 
        {
            return p1.x() < p2.x();
        }
        return p1.y() < p2.y();
    }
};

struct PointEqual 
{
    bool operator()(const Point& p1, const Point& p2) const 
    {
        return std::fabs(p1.x() - p2.x()) < EPSILON && std::fabs(p1.y() - p2.y()) < EPSILON;
    }
};

// ���ڱȽ� Point �Ĺ�ϣ�����
struct PointHash
{
    std::size_t operator()(const Point& point) const
    {
        auto roundedX = std::round(point.x() / EPSILON) * EPSILON;
        auto roundedY = std::round(point.y() / EPSILON) * EPSILON;
        return std::hash<double>()(roundedX) ^ (std::hash<double>()(roundedY) << 1);
    }
};

struct PairIPEqual {
    bool operator()(const std::pair<int, Point>& a, const std::pair<int, Point>& b) const {
        return a.first == b.first && PointEqual{}(a.second, b.second);
    }
};

struct PairIPHash {
    std::size_t operator()(const std::pair<int, Point>& p) const {
        auto h1 = std::hash<int>{}(p.first);
        auto h2 = PointHash{}(p.second);
        return h1 ^ (h2 << 1); // XOR ���������
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

// Point�汾���
inline double cross(const Point& a, const Point& b)
{
    return cross(a.x(), a.y(), b.x(), b.y());
}

// Point�汾���
inline double dot(const Point& a, const Point& b)
{
    return dot(a.x(), a.y(), b.x(), b.y());
}

// Point�汾����
inline double length(const Point& a)
{
    return length(a.x(), a.y());
}

// �жϵ���������λ�ù�ϵ
int pointRelativeToVector(const Point& point, const Point& vectorStart, const Point& vectorEnd)
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
bool pointOnSegment(const Point& q, const Point& p, const Point& r)
{
    // ������ֱ�ӷ���
    if(pointRelativeToVector(p, q, r))return false; 

    // ��� q �Ƿ��ڵ� p �͵� r �ķ�Χ��
    return (q.x() <= std::max(p.x(), r.x()) + EPSILON &&
        q.x() >= std::min(p.x(), r.x()) - EPSILON &&
        q.y() <= std::max(p.y(), r.y()) + EPSILON &&
        q.y() >= std::min(p.y(), r.y()) - EPSILON);
}

// �߶��Ƿ��ཻ�ͼ�������
bool segmentsIntersect(const Point& p1Start, const Point& p1End,
    const Point& p2Start, const Point& p2End,
    Point& intersection)
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

        if (std::fabs(determinant) < EPSILON)
        {
            return false; // ƽ���߶�
        }

        // ���㽻������
        intersection.setX((b2 * c1 - b1 * c2) / determinant);
        intersection.setY((a1 * c2 - a2 * c1) / determinant);

        // ��齻���Ƿ����߶η�Χ��
        if (pointOnSegment(intersection, p1Start, p1End) && pointOnSegment(intersection, p2Start, p2End))
        {
            return true;
        }
        return false;
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

    return false; // û���ཻ
}

// �ж��������������������������ʽ��(�����ߵ�˳��)
bool isClockwise(const Line& line) {
    double sum = 0.0;
    for (int i = 0; i < line.size() - 1; ++i) {
        const auto& p1 = line[i];
        const auto& p2 = line[i + 1];
        sum += (p2.x() - p1.x()) * (p2.y() + p1.y());
    }
    return sum < 0.0; // >0 ��ʾ˳ʱ�룬<0 ��ʾ��ʱ�� (��ʵ���������Ƿ��ģ���������ѧ�������ʱ��) 
}

// ���Ƿ�������
bool isPointInsidePolygon(const Point& point, const Polygon& polygon)
{
    bool insideOuterRing = false; // �Ƿ����⻷��

    for (const auto& ring : polygon) // ����ÿ����
    {
        int windingNumber = 0; // ����ͳ�Ƶ������

        for (int i = 0; i < ring.size(); ++i)
        {
            Point p1 = ring[i];
            Point p2 = ring[(i + 1) % ring.size()]; // ��һ����

            // �жϵ��Ƿ��ڱ߿���
            if (pointOnSegment(point, p1, p2))
            {
                return true; // ���ڱ��ϣ���Ϊ�ڶ������(��Ϊ�ߺ�ͼ�β��ཻ,�ڱ߿��Ͼ��Ǳ߿�,��Ҫ����)
            }

            // ��������
            if (p1.y() <= point.y())
            {
                if (p2.y() > point.y() && cross(p1 - point, p2 - point) > 0)
                {
                    ++windingNumber; // ��������ϴ�����
                }
            }
            else
            {
                if (p2.y() <= point.y() && cross(p1 - point, p2 - point) < 0)
                {
                    --windingNumber; // ��������´�����
                }
            }
        }

        // ���������жϵ��뵱ǰ����λ�ù�ϵ
        if (windingNumber != 0) // ���ڵ�ǰ����
        {
            if (isClockwise(ring)) // ������⻷
            {
                insideOuterRing = true; // ��ǵ����⻷��
                break;
            }
            else // ������ڻ�
            {
                return false; // ���ڿ��ڣ�ֱ�ӷ���false
            }
        }

        if(insideOuterRing)
        {
           break;
        }
    }

    return insideOuterRing; // ��������κ��⻷�ڵ����ڿ��ڣ��򷵻�true
}


double pointToSegmentDistance(const Point& point, const Point& start, const Point& end) {
    // �����߶ε�����
    Point segment = end - start;
    Point vectorToPoint = point - start;

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
    Point projection = start + t * segment;

    // ���㲢���ص㵽ͶӰ��ľ���
    return length(point - projection);
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

bool computeBufferBoundary(BufferCalculationMode mode, const Polygon& pointss, double r, Polygon& boundaryPointss)
{
    boundaryPointss.clear();
    // ���������
    if (pointss.size() == 1 && pointss[0].size() == 1)
    {
        Point point = pointss[0][0];
        boundaryPointss.push_back({});
        calculateArcPoints(point, r, 0, 2 * M_PI, GlobalSteps, boundaryPointss.last());
        return true;
    }

    Polygon polygon = pointss;

    for (auto& line : polygon)
        simpleLine(line);

    if (mode == BufferCalculationMode::Raster)return computeBufferBoundaryWithGrid(polygon, r, boundaryPointss);
    else return computeBufferBoundaryWithVector(polygon, r, boundaryPointss);
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
Point bezierN(const Line& controlPoints, double t)
{
    int n = controlPoints.size() - 1;
    Point point(0.0, 0.0); // �����洢������

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
        Point point = bezierN(controlPoints, t); // ���㵱ǰtֵ��Ӧ�ı�������
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

        Point point(0.0, 0.0);

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
bool calculateCircle(const Point& p1, const Point& p2, const Point& p3, Point& center, double& radius)
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

    center = Point(center_x, center_y);
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
bool calculateCircle(const Point& p1, const Point& p2, Point& center, double& radius)
{
    double center_x = (p1.x() + p2.x()) / 2.0;
    double center_y = (p1.y() + p2.y()) / 2.0;

    center = Point(center_x, center_y);
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
bool calculateArcPoints(const Point& center, double radius, double startAngle, double angleDiff, int steps, Line& points)
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

        points.append(Point(x, y));
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
bool calculateArcPointsFromThreePoints(const Point& point1, const Point& point2, const Point& point3, int steps, Line& arcPoints)
{
    Point center;
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
bool calculateCirclePointsFromControlPoints(const Point& point1, const Point& point2, int steps, Line& arcPoints)
{
    Point center;
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

            leftPolyline.append(Point(x1 + vx * dis, y1 + vy * dis));  // ����ƫ��
            rightPolyline.append(Point(x1 - vx * dis, y1 - vy * dis));  // ����ƫ��
        }
        else if (i == plLen - 1)
        {
            // ��������ߵ��յ�
            double x1 = polyline[i - 1].x(), y1 = polyline[i - 1].y();
            double x2 = polyline[i].x(), y2 = polyline[i].y();

            auto [vx, vy] = normalize(y1 - y2, x2 - x1);

            leftPolyline.append(Point(x2 + vx * dis, y2 + vy * dis));  // ����ƫ��
            rightPolyline.append(Point(x2 - vx * dis, y2 - vy * dis));  // ����ƫ��
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
                leftPolyline.append(Point(x0 - vx * disBisector, y0 - vy * disBisector));  // ����ƫ��
                rightPolyline.append(Point(x0 + vx * disBisector, y0 + vy * disBisector));  // ����ƫ��
            }
            else
            {
                leftPolyline.append(Point(x0 + vx * disBisector, y0 + vy * disBisector));  // ����ƫ��
                rightPolyline.append(Point(x0 - vx * disBisector, y0 - vy * disBisector));  // ����ƫ��
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

            parallelPolyline.append(Point(x1 +  vx * dis, y1 +  vy * dis));
        }
        else if (i == plLen - 1)
        {
            // ��������ߵ��յ�
            double x1 = polyline[i - 1].x(), y1 = polyline[i - 1].y();
            double x2 = polyline[i].x(), y2 = polyline[i].y();

            auto [vx, vy] = normalize(y1 - y2, x2 - x1);

            parallelPolyline.append(Point(x2 +  vx * dis, y2 +  vy * dis)); 
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
                parallelPolyline.append(Point(x0 - vx * disBisector, y0 - vy * disBisector));   // ����ƫ��(���dis�Ǹ���������ƫ����)
            }
            else
            {
                parallelPolyline.append(Point(x0 + vx * disBisector, y0 + vy * disBisector));   // ����ƫ��
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
double pointToLineDistanceWithDirection(const Point& point, const Point& lineStart, const Point& lineEnd)
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
bool calculateParallelLineThroughPoint(const Line& polyline, const Point& targetPoint, Line& parallelPolyline)
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
        for (const Point& point : points)
        {
            if (point.x() < minX) minX = point.x();
            if (point.x() > maxX) maxX = point.x();
            if (point.y() < minY) minY = point.y();
            if (point.y() > maxY) maxY = point.y();
        }
    }
    // �����������ұ߽�
    return QRectF(Point(minX, minY), Point(maxX, maxY));
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
    gridMap.offset = Point(bounds.left() - (k + 5) * gridMap.scale, bounds.top() - (k + 5) * gridMap.scale);

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
            points.append(Point(x, y)); // ��ԭԭʼ�����
        }
    }
}

// �жϵ��Ƿ����������ߵľ���С�ڸ���ֵ
bool isPointCloseToAnyPolyline(const Point& point, const Polygon& boundaryPointss, double distance)
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
            const Point& start = polyline[i];
            const Point& end = polyline[i + 1];

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
    Point start = points.first();
    Point end = points.last();

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
    if (points.size() < 2)return;
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




bool computeBufferBoundaryWithVector(const Polygon& pointss, double r, Polygon& boundaryPointss)
{
    GeoBuffer::Polygon polygon;
    for (auto& line : pointss)
    {
        polygon.push_back({});
        auto& polyline = polygon.back();
        for (auto& point : line)
        {
            polyline.emplace_back(point.x(), point.y());
        }
    }

    GeoBuffer::Polygon lines;
    GeoBuffer::calculateCompleteLineBuffer(polygon, r, lines);


    for (auto& line : lines)
    {
        boundaryPointss.push_back({});
        auto& polyline = boundaryPointss.back();
        for (auto& point : line)
        {
            polyline.push_back({ point.x , point.y });
        }
    }

    return true;
}








#ifdef DEBUG

// ����������н��㣨������ͬһ�����������߶εĽ��㣩
void bruteForceFindIntersections1(const Polygon& polygons, Line& intersections)
{

    // ����Segment�ṹ��
    struct Segment {
        Point start;
        Point end;
        size_t lineIndex;  // �������ߵ�����
        size_t segmentIndex;  // �߶��������е�������˳��

        Segment(const Point& start, const Point& end, size_t lineIndex, size_t segmentIndex)
            : start(start), end(end), lineIndex(lineIndex), segmentIndex(segmentIndex) {}
    };

    // ���������߶Σ�����¼���������������������߶�����
    QVector<Segment> segments;

    for (size_t lineIndex = 0; lineIndex < polygons.size(); ++lineIndex)
    {
        const auto& lines = polygons[lineIndex];
        for (size_t i = 0; i < lines.size() - 1; ++i)
        {
            segments.push_back(Segment(lines[i], lines[i + 1], lineIndex, i));
        }
    }

    // ö�������߶���ϣ�����Ƿ��ཻ
    for (size_t i = 0; i < segments.size(); ++i)
    {
        for (size_t j = i + 1; j < segments.size(); ++j)
        {
            // ��������߶�����ͬһ������
            if (segments[i].lineIndex == segments[j].lineIndex)
            {
                // ��������߶����ڣ�������
                if (std::abs(static_cast<int>(segments[i].segmentIndex) - static_cast<int>(segments[j].segmentIndex)) == 1 ||
                    (segments[i].segmentIndex == 0 && segments[j].segmentIndex == polygons[segments[i].lineIndex].size() - 2) ||
                    (segments[i].segmentIndex == polygons[segments[i].lineIndex].size() - 2 && segments[j].segmentIndex == 0))
                {
                    continue; // �������ڵ��߶�
                }
            }

            Point intersection;
            if (segmentsIntersect(segments[i].start, segments[i].end,
                segments[j].start, segments[j].end, intersection))
            {
                intersections.push_back(intersection);
            }
        }
    }
}



// ����������Ĺ��ߺ���
double randomInRange(double min, double max) {
    return min + static_cast<double>(rand()) / RAND_MAX * (max - min);
}

// ����һ������պ�����
Line generateRandomLine(int numPoints, int num) {
    QVector<Point> polygon;
    polygon.reserve(numPoints);

    // �����������
    srand(static_cast<unsigned int>(time(0) + num * 100));

    // ������ɶ���εĵ�
    for (int i = 0; i < numPoints; ++i) {
        double x = randomInRange(0, 1000);  // �������x����
        double y = randomInRange(0, 1000);  // �������y����
        polygon.push_back(Point(x, y));
    }

    // ȷ�����һ�������һ�����غϣ��պ϶����
    polygon.push_back(polygon[0]);

    // ����һ����������κ������߶ε�Polygon�ṹ��
    return polygon;  // ����ֻ����һ�������
}

void test()
{
    int flag = 0;
    int cnt = 0;
    while (!flag)
    {

        // �����������
        //Polygon polygons = { generateRandomLine(10),  generateRandomLine(6) };
        Polygon polygons;

        for (int i = 0; i < 2; i++)
        {
            auto x = generateRandomLine(20, i);
            polygons.push_back(x);
        }

        Line bruteIntersections;  // �������
        Line sweepIntersections;  // ɨ���߽��
        // ִ�б����㷨
        bruteForceFindIntersections1(polygons, bruteIntersections);
        // ִ��ɨ�����㷨
        //GeoBuffer::sweepLineFindIntersections(polygons, sweepIntersections);

        // ���ıȽ�
        std::unordered_set<Point, PointHash, PointEqual> bruteSet(bruteIntersections.begin(), bruteIntersections.end());
        std::unordered_set<Point, PointHash, PointEqual> sweepSet(sweepIntersections.begin(), sweepIntersections.end());



        // ���죺�������㲻��ɨ���߽����
        std::unordered_set<Point, PointHash, PointEqual> diffBrute;
        for (const auto& p : bruteSet)
        {
            if (sweepSet.find(p) == sweepSet.end())  // �ڱ�������е�����ɨ���߽����
            {
                diffBrute.insert(p);
            }
        }

        // ���죺ɨ���߽��㲻�ڱ��������
        std::unordered_set<Point, PointHash, PointEqual> diffSweep;
        for (const auto& p : sweepSet)
        {
            if (bruteSet.find(p) == bruteSet.end())  // ��ɨ���߽���е����ڱ��������
            {
                diffSweep.insert(p);
            }
        }

        if (diffBrute.size() || diffSweep.size())
        {
            flag = 1;
        }
        else
        {
            cnt++;
            continue;
        }

        qDebug() << L("��") << cnt << L("�ζ���");

        // �ԱȽ��
        qDebug() << L("����ʧ�ܣ������һ�¡�");
        qDebug() << L("�ڱ����㷨�е�����ɨ�����㷨���ҵ�: ") << diffBrute.size();
        qDebug() << L("��ɨ�����㷨�е����ڱ����㷨���ҵ�: ") << diffSweep.size();

        Line diff;
        for (auto& point : diffBrute) diff.push_back(point);
        for (auto& point : diffSweep) diff.push_back(point);
        G2_polygon = polygons;
        G2_intersections = diff;
    }

}

#endif // DEBUG
