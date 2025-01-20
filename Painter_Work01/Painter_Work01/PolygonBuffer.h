#pragma once
// 计算缓冲区太麻烦。单独放到这里

/**
* 思路：
* 1.计算单一折线的缓冲区
*	1.1 对于凸角方向, 可以直接使用凸角圆
*	1.2 对于角度大的凹角方向，直接使用角平分计算交点
*	1.3 对于角度小产生的复杂情况, 画满
*	1.4 用扫描线求交点（包括自相交的）
*	1.5 用交点打断线
*	1.6 剔除多余线（可以计算与最近距离是否小于r）
*	1.6 搜索出闭合折线的区间
*
* 2.闭合折线的算法和上面相同
*
* 3.多个缓冲区重复计算交点和剔除线,搜索出轮廓
*/

#include <QVector>
#include <QPointF>

namespace GeoBuffer
{
const double _M_PI = 3.14159265358979323846;
const double EPSILON = 1e-4;
const double EPSILON_DRAW = 2; // 绘制误差（比如圆弧上的折线到圆心的距离小于缓冲区距离）

// 精度处理（低精度）
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


// 到(-2pi, 0],顺时针方向
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

    // 用inline是因为想在头文件里直接写，函数又多又短
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
        return x * other.x + y * other.y;  // 点乘
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
        return x * other.y - y * other.x; // 叉乘
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

        // x 和 y 都接近相等时返回 false
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

    // 右边法向量
    inline MyPoint normal() const 
    {
        return MyPoint(y, -x);
    }

    // 比较向量的方向
    inline int directionTo1(const MyPoint& other) const
    {
        return sgn1(operator^(other)); //大于0是顺时针
    }

    inline int directionTo2(const MyPoint& other) const
    {
        return sgn2(operator^(other));  //大于0是顺时针
    }
    
};

using Vector2D = MyPoint;
using Point = MyPoint;
using Line = std::vector<Point>;                   // 表示一条线
using Polygon = std::vector<Line>;                 // 表示一个面
using Polygons = std::vector<Polygon>;             // 表示面集合

// 计算圆弧上的点
bool calculateArcPoints(const Point& center, double radius, double startAngle, double angleDiff, int steps, Line& points);

// 计算通过三个点的圆
bool calculateCircle(const Point& p1, const Point& p2, const Point& p3, Point& center, double& radius);

// 从三个点计算圆弧上的点
bool calculateArcPointsFromThreePoints(const Point& point1, const Point& point2, const Point& point3, int steps, Line& arcPoints);

// 计算从起点和终点到圆心的圆弧上的点
bool calculateArcPointsFromStartEndCenter(const Point& startPoint, const Point& endPoint, const Point& center, int steps, Line& arcPoints);

// 计算两个向量半角的tan值
double computeHalfAngleTan(const Vector2D& v1, const Vector2D& v2);

// 计算折线的缓冲区
bool draftLineBuffer(const Line& polyline, double dis, Polygon& polygon, int step);

// 判断点是否在线段上
bool pointOnSegment1(const Point& q, const Point& p, const Point& r);

// 判断两条线段是否相交并计算交点
bool segmentsIntersect(const Point& p1Start, const Point& p1End,
    const Point& p2Start, const Point& p2End,
    Point& intersection);


// 求解多边形的所有交点（不计算同一条折线自己的交点）
void bruteForceFindIntersections(const Polygon& polygons, Line& intersections);

// 计算点到线段的距离
double pointToSegmentDistance2(const Point& point, const Point& start, const Point& end);

// 扫描线算法寻找交点
void sweepLineFindIntersections(const Polygon& pointss, Line& intersections, bool isArea = true);

// 通过交点分割折线
void splitLineByIntersections(const Polygon& polygon, const Line& intersectionPoints, Polygon& splitLines);

// 过滤接近多边形的分割线
void filterSplitLinesCloseToPolyLines(const Polygon& splitLines,
    const Polygon& polyLines,
    const double dis,
    Polygon& filteredSplitLines);

// 重建多边形
void reconstructPolygons(const Polygon& splitLines, Polygon& mergedPolygons);

// 计算完整的线缓冲区
bool calculateCompleteLineBuffer(const Polygon& polygon, double dis, Polygon& lines, int step = 20);

} // namespace GeoBuffer

