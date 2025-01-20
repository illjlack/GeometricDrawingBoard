#pragma once
// ���㻺����̫�鷳�������ŵ�����

/**
* ˼·��
* 1.���㵥һ���ߵĻ�����
*	1.1 ����͹�Ƿ���, ����ֱ��ʹ��͹��Բ
*	1.2 ���ڽǶȴ�İ��Ƿ���ֱ��ʹ�ý�ƽ�ּ��㽻��
*	1.3 ���ڽǶ�С�����ĸ������, ����
*	1.4 ��ɨ�����󽻵㣨�������ཻ�ģ�
*	1.5 �ý�������
*	1.6 �޳������ߣ����Լ�������������Ƿ�С��r��
*	1.6 �������պ����ߵ�����
*
* 2.�պ����ߵ��㷨��������ͬ
*
* 3.����������ظ����㽻����޳���,����������
*/

#include <QVector>
#include <QPointF>

namespace GeoBuffer
{
const double _M_PI = 3.14159265358979323846;
const double EPSILON = 1e-4;
const double EPSILON_DRAW = 2; // ����������Բ���ϵ����ߵ�Բ�ĵľ���С�ڻ��������룩

// ���ȴ����;��ȣ�
inline int sgn2(double x)
{
    if (x < - EPSILON_DRAW) return -1;
    else if (x > EPSILON_DRAW) return 1;
    else return 0;
}

inline int sgn1(double x)
{
    if (x < -EPSILON) return -1;
    else if (x > EPSILON) return 1;
    else return 0;
}


// ��(-2pi, 0],˳ʱ�뷽��
inline double normalizeAngle(double angle)
{
    while (angle <= -2 * _M_PI)
    {
        angle += 2 * _M_PI;
    }
    while (angle > 0)
    {
        angle -= 2 * _M_PI;
    }
    return angle;
}

struct MyPoint
{
    double x;
    double y;

    // ��inline����Ϊ����ͷ�ļ���ֱ��д�������ֶ��ֶ�
    inline MyPoint(double x = 0, double y = 0) : x(x), y(y) {}

    inline MyPoint operator- () const
    {
        return MyPoint(-x, -y);
    }

    inline MyPoint operator+ (const MyPoint& other) const
    {
        return MyPoint(x + other.x, y + other.y);
    }

    inline MyPoint operator+ (double dx) const
    {
        return MyPoint(x + dx, y + dx);
    }

    inline MyPoint operator- (const MyPoint& other) const
    {
        return MyPoint(x - other.x, y - other.y);
    }

    inline double operator* (const MyPoint& other) const
    {
        return x * other.x + y * other.y;  // ���
    }

    inline MyPoint operator* (double xx) const
    {
        return MyPoint(x * xx , y * xx);
    }

    inline MyPoint operator/ (double scalar) const
    {
        return MyPoint(x / scalar, y / scalar);
    }

    inline double operator^ (const MyPoint& other) const
    {
        return x * other.y - y * other.x; // ���
    }

    inline bool operator== (const MyPoint& other) const
    {
        return std::fabs(x - other.x) < EPSILON && std::fabs(y - other.y) < EPSILON;
    }

    inline bool operator< (const MyPoint& other) const
    {
        if (std::fabs(x - other.x) > EPSILON)
        {
            return x < other.x;
        }

        if (std::fabs(y - other.y) > EPSILON)
        {
            return y < other.y;
        }

        // x �� y ���ӽ����ʱ���� false
        return false;
    }

    inline double length() const
    {
        return std::sqrt(x * x + y * y);
    }

    inline double length2() const
    {
        return x * x + y * y;
    }

    inline double distanceTo(const MyPoint& other) const
    {
        return (operator-(other).length());
    }

    inline MyPoint normalize() const
    {
        return operator/(length());
    }

    // �ұ߷�����
    inline MyPoint normal() const 
    {
        return MyPoint(y, -x);
    }

    // �Ƚ������ķ���
    inline int directionTo1(const MyPoint& other) const
    {
        return sgn1(operator^(other)); //����0��˳ʱ��
    }

    inline int directionTo2(const MyPoint& other) const
    {
        return sgn2(operator^(other));  //����0��˳ʱ��
    }
    
};

using Vector2D = MyPoint;
using Point = MyPoint;
using Line = std::vector<Point>;                   // ��ʾһ����
using Polygon = std::vector<Line>;                 // ��ʾһ����
using Polygons = std::vector<Polygon>;             // ��ʾ�漯��

// ����Բ���ϵĵ�
bool calculateArcPoints(const Point& center, double radius, double startAngle, double angleDiff, int steps, Line& points);

// ����ͨ���������Բ
bool calculateCircle(const Point& p1, const Point& p2, const Point& p3, Point& center, double& radius);

// �����������Բ���ϵĵ�
bool calculateArcPointsFromThreePoints(const Point& point1, const Point& point2, const Point& point3, int steps, Line& arcPoints);

// ����������յ㵽Բ�ĵ�Բ���ϵĵ�
bool calculateArcPointsFromStartEndCenter(const Point& startPoint, const Point& endPoint, const Point& center, int steps, Line& arcPoints);

// ��������������ǵ�tanֵ
double computeHalfAngleTan(const Vector2D& v1, const Vector2D& v2);

// �������ߵĻ�����
bool draftLineBuffer(const Line& polyline, double dis, Polygon& polygon, int step);

// �жϵ��Ƿ����߶���
bool pointOnSegment1(const Point& q, const Point& p, const Point& r);

// �ж������߶��Ƿ��ཻ�����㽻��
bool segmentsIntersect(const Point& p1Start, const Point& p1End,
    const Point& p2Start, const Point& p2End,
    Point& intersection);


// ������ε����н��㣨������ͬһ�������Լ��Ľ��㣩
void bruteForceFindIntersections(const Polygon& polygons, Line& intersections);

// ����㵽�߶εľ���
double pointToSegmentDistance2(const Point& point, const Point& start, const Point& end);

// ɨ�����㷨Ѱ�ҽ���
void sweepLineFindIntersections(const Polygon& pointss, Line& intersections, bool isArea = true);

// ͨ������ָ�����
void splitLineByIntersections(const Polygon& polygon, const Line& intersectionPoints, Polygon& splitLines);

// ���˽ӽ�����εķָ���
void filterSplitLinesCloseToPolyLines(const Polygon& splitLines,
    const Polygon& polyLines,
    const double dis,
    Polygon& filteredSplitLines);

// �ؽ������
void reconstructPolygons(const Polygon& splitLines, Polygon& mergedPolygons);

// �����������߻�����
bool calculateCompleteLineBuffer(const Polygon& polygon, double dis, Polygon& lines, int step = 20);

} // namespace GeoBuffer

