#include "PolygonBuffer.h"
#include "comm.h"
#include <map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <functional>

namespace GeoBuffer
{


bool calculateArcPoints(const Point& center, double radius, double startAngle, double angleDiff, int steps, Line& points)
{
    if (steps <= 0 || radius <= 0)
    {
        return false;
    }

    // 计算角度步长
    double angleStep = angleDiff / steps;

    for (int i = 0; i <= steps; ++i)
    {
        double angle = normalizeAngle(startAngle + i * angleStep);

        double x = center.x + radius * std::cos(angle);
        double y = center.y + radius * std::sin(angle);

        points.emplace_back(x, y);
    }
    return true;
}

bool calculateCircle(const Point& p1, const Point& p2, const Point& p3, Point& center, double& radius)
{
    if (p1 == p2 || p2 == p3 || p3 == p1)
    {
        // 代替方案
        if (p1 == p2)
        {
            center = (p1 + p3) / 2;
            radius = p1.distanceTo(p3) / 2;

        }
        else if (p1 == p3)
        {
            center = (p1 + p2) / 2;
            radius = p1.distanceTo(p2) / 2;
        }
        return true;
    }

    // 计算两条边的中垂线方程
    double x1 = p1.x, y1 = p1.y;
    double x2 = p2.x, y2 = p2.y;
    double x3 = p3.x, y3 = p3.y;

    double a1 = x2 - x1, b1 = y2 - y1;
    double a2 = x3 - x2, b2 = y3 - y2;

    double mid1_x = (x1 + x2) / 2.0, mid1_y = (y1 + y2) / 2.0;
    double mid2_x = (x2 + x3) / 2.0, mid2_y = (y2 + y3) / 2.0;

    double slope1, slope2;
    double c1, c2;

    // 处理竖直线情况
    if (b1 == 0) {  // p1 到 p2 竖直线
        slope1 = std::numeric_limits<double>::infinity();
        c1 = mid1_x;  // 中垂线为 x = mid1_x
    }
    else
    {
        slope1 = -a1 / b1;
        c1 = mid1_y - slope1 * mid1_x;
    }

    if (b2 == 0) {  // p2 到 p3 竖直线
        slope2 = std::numeric_limits<double>::infinity();
        c2 = mid2_x;  // 中垂线为 x = mid2_x
    }
    else
    {
        slope2 = -a2 / b2;
        c2 = mid2_y - slope2 * mid2_x;
    }

    // 计算交点
    if (slope1 == slope2)
    {
        return false; // 三点共线，无法确定唯一圆
    }

    double center_x = (c2 - c1) / (slope1 - slope2);
    double center_y = slope1 * center_x + c1;

    center = Point(center_x, center_y);
    radius = std::sqrt(std::pow(center.x - x1, 2) + std::pow(center.y - y1, 2));  // 计算半径
    return true;
}

bool calculateArcPointsFromThreePoints(const Point& point1, const Point& point2, const Point& point3, int steps, Line& arcPoints)
{
    Point center;
    double radius;

    if (!calculateCircle(point1, point2, point3, center, radius))
    {
        return false;
    }

    double startAngle = std::atan2(point1.y - center.y, point1.x - center.x);
    double endAngle = std::atan2(point3.y - center.y, point3.x - center.x);
    double middleAngle = std::atan2(point2.y - center.y, point2.x - center.x);

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
        angleDiff = angleDiffEnd - 2 * _M_PI;
    }

    return calculateArcPoints(center, radius, startAngle, angleDiff, steps, arcPoints);
}

bool calculateArcPointsFromStartEndCenter(const Point& startPoint, const Point& endPoint, const Point& center,
    int steps, Line& arcPoints)
{
    // 计算起点和终点的角度
    double startAngle = std::atan2(startPoint.y - center.y, startPoint.x - center.x);
    double endAngle = std::atan2(endPoint.y - center.y, endPoint.x - center.x);

    startAngle = normalizeAngle(startAngle);
    endAngle = normalizeAngle(endAngle);

    // 计算角度差（弧度差）
    double angleDiff = normalizeAngle(endAngle - startAngle);

    // 计算圆弧上的点
    return calculateArcPoints(center, std::sqrt((center.x - startPoint.x) * (center.x - startPoint.x)
        + (center.y - startPoint.y) * (center.y - startPoint.y)), startAngle, angleDiff, steps, arcPoints);
}

// 计算向量半角的tan
double computeHalfAngleTan(const Vector2D& v1, const Vector2D& v2) {
    const Vector2D& dv1 = v1.normalize();
    const Vector2D& dv2 = v2.normalize();

    // 计算 sin 和 cos
    double cosTheta = dv1 * dv2;
    double sinTheta = std::fabs(dv1 ^ dv2);

    // 计算 tan(θ / 2)
    return sinTheta / (1 + cosTheta);
};

// 计算折线的缓冲区
bool draftLineBuffer(const Line& polyline, double dis, Polygon& polygon, int step = 20)
{
    int plLen = polyline.size();
    if (plLen == 0)
    {
        return true;
    }
    if (plLen == 1)
    {
        polygon.push_back({});
        calculateArcPoints(polyline[0], dis, 0, 2 * _M_PI, step, polygon.back());
        return true;
    }

    int initSize = polygon.size();
    // 初始新建线
    polygon.push_back({});

    // 处理开始顶点
    auto start = [&](const Point& p1, const Point& p2)
    {
        // 指向p1的方向向量的右边单位法向量*dis
        Vector2D v = (p1 - p2).normal().normalize() * dis;

        // 从左边顺时针画圆到右边
        calculateArcPointsFromStartEndCenter(p1 - v, p1 + v, p1, step, polygon.back());
    };

    // 处理最后点
    auto end = [&](const Point& p1, const Point& p2)
    {
        // 所有的方向都指向折线前面，容易判断左右
        // 指向p1的方向向量的右边单位法向量*dis
        Vector2D v = (p1 - p2).normal().normalize() * dis;
        // 尾部从右边顺时针画圆到左边
        calculateArcPointsFromStartEndCenter(p2 + v, p2 - v, p2, step, polygon.back());
    };

    // 处理中间点
    auto mid = [&](const Point& p0, const Point& p1, const Point& p2)
    {
        // 计算前后两段方向向量
        const Vector2D& v1 = p0 - p1;
        const Vector2D& v2 = p1 - p2;

        if (sgn1(v1 ^ v2) == 0)
        {
            // 共线不用处理
            return;
        }

        // 选择凹凸角
        if (((v1.normalize()) ^ (v2.normalize())) > 0) // 正的右边是凹角
        {
            // 判断夹角是否过小
            double needLen = dis / computeHalfAngleTan(v1, -v2);
            if (v1.length() < needLen || v2.length() < needLen)
            {
                // 夹角小,画到底部,交叉
                polygon.back().push_back(p1 + v1.normal().normalize() * dis);

                // 新建线
                polygon.push_back({});
                polygon.back().push_back(p1 + v2.normal().normalize() * dis);
            }
            else
            {
                // 直接求交点
                // 计算角平分线的单位向量
                const Vector2D& dv = (v1.normalize() - v2.normalize()).normalize();

                // 计算角平分线的长度，用于确定平行线的偏移量
                double sinX = std::fabs(dv ^ (v1.normalize()));
                double disBisector = dis / sinX;

                polygon.back().push_back(p1 + dv * disBisector);
            }
        }
        else // 凸角画圆弧
        {
            calculateArcPointsFromStartEndCenter(p1 + v1.normal().normalize() * dis, p1 + v2.normal().normalize() * dis, p1, step, polygon.back());
        }
    };

    bool isClose = (polyline.front() == polyline.back());


    
    if(!isClose)
    {
        // 遍历折线的每个点(右边)
        for (int i = 0; i < plLen; ++i)
        {
            if (i == 0)
            {
                start(polyline[i], polyline[i + 1]);
            }
            else if (i == plLen - 1)
            {
                end(polyline[i - 1], polyline[i]);
            }
            else
            {
                mid(polyline[i - 1], polyline[i], polyline[i + 1]);
            }
        }

        // 遍历折线的每个点(左边)
        for (int i = plLen - 2; i > 0; --i)
        {
            mid(polyline[i + 1], polyline[i], polyline[i - 1]);
        }

        // 闭合
        if (polygon.size() > initSize && polygon[initSize].size())
        {
            polygon.back().push_back(polygon[initSize][0]);
        }
    }
    else
    {
        plLen--;
        for (int i = 0; i < plLen; ++i)
        {
            mid(polyline[(i - 1 + plLen)%plLen], polyline[i], polyline[(i + 1) % plLen]);
        }
        // 闭合
        if (polygon.size() > initSize && polygon[initSize].size())
        {
            polygon.back().push_back(polygon[initSize][0]);
        }

        initSize = polygon.size();
        polygon.push_back({});
        // 遍历折线的每个点(左边)
        for (int i = plLen - 1; i >= 0; --i)
        {
            mid(polyline[(i + 1) % plLen], polyline[i], polyline[(i - 1 + plLen) % plLen]);
        }
        // 闭合
        if (polygon.size() > initSize && polygon[initSize].size())
        {
            polygon.back().push_back(polygon[initSize][0]);
        }

    }




    return true;
}

// 判断点 q 是否在线段 pr 上(精度高一点，为了不多计算交点)
bool pointOnSegment1(const Point& q, const Point& p, const Point& r)
{
    // 不共线直接返回
    if ((q - p).directionTo1(r - p))return false;

    // 检查 q 是否在点 p 和点 r 的范围内
    return (q.x <= std::max(p.x, r.x) + EPSILON &&
        q.x >= std::min(p.x, r.x) - EPSILON &&
        q.y <= std::max(p.y, r.y) + EPSILON &&
        q.y >= std::min(p.y, r.y) - EPSILON);
}

// 判断点 q 是否在线段 pr 上(精度低一点，分割的是否分割到)
bool pointOnSegment2(const Point& q, const Point& p, const Point& r)
{
    // 不共线直接返回
    if ((q - p).directionTo2(r - p))return false;

    // 检查 q 是否在点 p 和点 r 的范围内
    return (q.x <= std::max(p.x, r.x) + EPSILON_POINT &&
        q.x >= std::min(p.x, r.x) - EPSILON_POINT &&
        q.y <= std::max(p.y, r.y) + EPSILON_POINT &&
        q.y >= std::min(p.y, r.y) - EPSILON_POINT);
}

// 线段是否相交和计算坐标
bool segmentsIntersect(const Point& p1Start, const Point& p1End,
    const Point& p2Start, const Point& p2End,
    Point& intersection)
{
    // 计算方向
    int o1 = (p1End - p1Start).directionTo1(p2Start - p1Start);
    int o2 = (p1End - p1Start).directionTo1(p2End - p1Start);
    int o3 = (p2End - p2Start).directionTo1(p1Start - p2Start);
    int o4 = (p2End - p2Start).directionTo1(p1End - p2Start);

    // 通用相交条件（两线段在对方两侧）
    if (o1 != o2 && o3 != o4)
    {
        // 计算交点
        double a1 = p1End.y - p1Start.y;
        double b1 = p1Start.x - p1End.x;
        double c1 = a1 * p1Start.x + b1 * p1Start.y;

        double a2 = p2End.y - p2Start.y;
        double b2 = p2Start.x - p2End.x;
        double c2 = a2 * p2Start.x + b2 * p2Start.y;

        double determinant = a1 * b2 - a2 * b1;

        if (std::fabs(determinant) < EPSILON)
        {
            return false; // 平行线段
        }

        // 计算交点坐标
        intersection.x = ((b2 * c1 - b1 * c2) / determinant);
        intersection.y = ((a1 * c2 - a2 * c1) / determinant);

        // 检查交点是否在线段范围内
        if (pointOnSegment1(intersection, p1Start, p1End) && pointOnSegment1(intersection, p2Start, p2End))
        {
            return true;
        }
        return false;
    }

    // 特殊情况：线段共线且有重叠（重叠的顺序无所谓）
    if (o1 == 0 && pointOnSegment1(p2Start, p1Start, p1End))
    {
        intersection = p2Start;
        return true;
    }
    if (o2 == 0 && pointOnSegment1(p2End, p1Start, p1End))
    {
        intersection = p2End;
        return true;
    }
    if (o3 == 0 && pointOnSegment1(p1Start, p2Start, p2End))
    {
        intersection = p1Start;
        return true;
    }
    if (o4 == 0 && pointOnSegment1(p1End, p2Start, p2End))
    {
        intersection = p1End;
        return true;
    }

    return false; // 没有相交
}

// 暴力求解所有交点（不计算同一条折线自己的交点）
void bruteForceFindIntersections(const Polygon& polygons, Line& intersections)
{

    // 定义Segment结构体
    struct Segment {
        Point start;
        Point end;
        size_t lineIndex;  // 所属折线的索引
        size_t segmentIndex;  // 线段在折线中的索引（顺序）

        Segment(const Point& start, const Point& end, size_t lineIndex, size_t segmentIndex)
            : start(start), end(end), lineIndex(lineIndex), segmentIndex(segmentIndex) {}
    };

    // 构造所有线段，并记录它们所属的折线索引和线段索引
    std::vector<Segment> segments;

    for (int lineIndex = 0; lineIndex < polygons.size(); ++lineIndex)
    {
        const auto& lines = polygons[lineIndex];
        for (int i = 0; i + 1 < lines.size(); ++i)
        {
            segments.emplace_back(Segment(lines[i], lines[i + 1], lineIndex, i));
        }
    }

    // 枚举两两线段组合，检查是否相交
    for (size_t i = 0; i < segments.size(); ++i)
    {
        for (size_t j = i + 1; j < segments.size(); ++j)
        {
            // 如果两条线段属于同一条折线
            if (segments[i].lineIndex == segments[j].lineIndex)
            {
                // 如果两条线段相邻，则跳过
                if (std::abs(static_cast<int>(segments[i].segmentIndex) - static_cast<int>(segments[j].segmentIndex)) == 1)
                {
                    continue; // 跳过相邻的线段
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

// 比较器（给std容器用）
struct PointComparator
{
    bool operator()(const Point& p1, const Point& p2) const
    {
        if (std::fabs(p1.x - p2.x) > EPSILON)
        {
            return p1.x < p2.x;
        }

        if (std::fabs(p1.y - p2.y) > EPSILON)
        {
            return p1.y < p2.y;
        }

        // 不能因为误差影响排序
        return true;
    }
};

struct PointEqual
{
    bool operator()(const Point& p1, const Point& p2) const
    {
        double x = std::fabs(p1.x - p2.x);
        double y = std::fabs(p1.y - p2.y);

        return std::fabs(p1.x - p2.x) < EPSILON && std::fabs(p1.y - p2.y) < EPSILON;
    }
};

// 用于比较 Point 的哈希和相等
struct PointHash
{
    std::size_t operator()(const Point& point) const
    {
        auto roundedX = std::round(point.x / EPSILON) * EPSILON;
        auto roundedY = std::round(point.y / EPSILON) * EPSILON;
        return std::hash<double>()(roundedX) ^ (std::hash<double>()(roundedY) << 1);
    }
};

// 用于比较 Point 的哈希和相等
struct PointHash2
{
    std::size_t operator()(const Point& point) const
    {
        auto roundedX = std::round(point.x / EPSILON_POINT) * EPSILON_POINT;
        auto roundedY = std::round(point.y / EPSILON_POINT) * EPSILON_POINT;
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
        return h1 ^ (h2 << 1); // XOR 和左移组合
    }
};

double pointToSegmentDistance2(const Point& point, const Point& start, const Point& end) {
    // 计算线段的向量
    const Point& segment = end - start;
    const Point& vectorToPoint = point - start;

    // 计算线段的长度
    double segmentLength = segment.length();

    // 如果线段的长度为零，返回点到起点的距离
    if (std::fabs(segmentLength) < EPSILON) {
        return vectorToPoint.length2();
    }

    // 计算投影点的比例t
    double t = (vectorToPoint * segment) / (segmentLength * segmentLength);
    t = std::max(0.0, std::min(1.0, t));  // 限制t在0到1之间

    // 计算投影点的坐标
    Point projection = start + segment * t;

    // 计算并返回点到投影点的距离
    return (point - projection).length2();
}

// 线段结构体
struct Segment
{
    Point start, end;

    // 确保线段的起点小于终点（按 x 排序）
    Segment(Point s, Point e)
    {
        if (s.x > e.x || (s.x == e.x && s.y > e.y))
        {
            start = e;
            end = s;
        }
        else
        {
            start = s;
            end = e;
        }

        if (end.x - start.x == 0)
        {
            if (end.y > start.y)
            {
                slope = std::numeric_limits<double>::infinity();  // 正无穷
            }
            else if (end.y < start.y)
            {
                slope = -std::numeric_limits<double>::infinity(); // 负无穷
            }
            else
            {
                slope = 0; // 同一点（两端相同）
            }
        }
        else slope = (end.y - start.y) / (end.x - start.x);
    }

    double slope; // 更新起点，不重复计算
};

void sweepLineFindIntersections(const Polygon& pointss, Line& intersections, bool isArea) {

    // 事件类型
    enum EventType { Intersection = 1, Start, End, ReStart };

    struct Event {
        Point point;                          // 当前事件的坐标
        int segmentIndex;                       // 所属线段的索引
        EventType type;                         // 事件类型

        // 重载比较运算符（用于优先队列）
        bool operator>(const Event& other) const
        {
            // 首先比较 x 坐标
            if (std::fabs(point.x - other.point.x) > EPSILON)
            {
                return point.x > other.point.x;
            }

            return type > other.type;
            // y 不重要
        }
    };

    std::vector<Segment> segments;

    // Step 1: 构造线段和事件点

    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> events; // 优先队列

    // 用于记录每个闭合曲线的索引区间（用来判断段的向量关系）
    std::vector<std::pair<int, int>> ringRanges;

    int cnt = 0;
    // 闭合曲线
    for (const auto& points : pointss) {

        int startIndex = cnt;
        for (int i = 0; i + 1 < points.size(); ++i) {
            // 创建线段
            Segment segment(points[i], points[i + 1]);
            segments.push_back(segment);
            cnt++;


            // 生成起点和终点事件
            Event startEvent{
                segment.start,                                  // 起点坐标
                cnt - 1,                                        // 线段索引
                Start                                           // 事件类型
            };

            Event endEvent{
                segment.end,                                    // 终点坐标
                cnt - 1,                                        // 线段索引
                End                                             // 事件类型
            };

            // 将事件插入优先队列
            events.push(startEvent);
            events.push(endEvent);
        }
        int endIndex = cnt - 1; // 最后一个线段索引
        ringRanges.push_back({ startIndex, endIndex });
    }

    // Step 2: 扫描事件点
    // 维护y的顺序对应的段 (y + x + 斜率 ,段号)
    struct statusTreeComparator {
        bool operator()(const std::pair<Point, double>& a, const std::pair<Point, double>& b) const {
            // 确定比较的 x 值（较大值）
            double x_target = std::max(a.first.x, b.first.x);

            double y_a;
            double y_b;

            if (x_target > a.first.x)
            {
                y_a = a.first.y + a.second * (x_target - a.first.x);
            }
            else
            {
                y_a = a.first.y; // 避免斜率是无穷大的时候nan
            }

            if (x_target > b.first.x)
            {
                y_b = b.first.y + b.second * (x_target - b.first.x);
            }
            else
            {
                y_b = b.first.y; // 避免斜率是无穷大的时候nan
            }

            // 比较 y 值，考虑精度
            if (std::fabs(y_a - y_b) > EPSILON)
            {
                return y_a < y_b;
            }

            return a.second < b.second;
        }
    };

    std::multimap<std::pair<Point, double>, int, statusTreeComparator >statusTree;

    auto isAdjacentSegments = [&](int segIndex1, int segIndex2) {

        if (segIndex2 == segIndex1)return true;

        // 使用 std::lower_bound 查找第一个区间的终点大于 segIndex 的位置
        auto it = std::lower_bound(ringRanges.begin(), ringRanges.end(), segIndex1,
            [](const std::pair<int, int>& range, int value) {
                return range.second < value; // 找到尾部大于 segIndex 的区间
            });

        const auto& range = *it;

        if (!isArea) // 不是围成区域，不能首尾相邻
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

    std::unordered_set<Point, PointHash, PointEqual> recordedIntersections;
    std::unordered_set<std::pair<int, Point>, PairIPHash, PairIPEqual> st;

    auto addIntersectionEvent = [&](int seg1, int seg2)
    {
        if (isAdjacentSegments(seg1, seg2))
        {
            return; // 如果两段相邻，忽略计算交点
        }
        Point intersecPoint;
        if (segmentsIntersect(segments[seg1].start, segments[seg1].end, segments[seg2].start, segments[seg2].end, intersecPoint))
        {
            // 限制条件：单线单交点事件放入一次
            if (!st.count({ seg1, intersecPoint }))
            {
                events.push({ intersecPoint, seg1, Intersection });
                st.insert({ seg1, intersecPoint });
            }
            if (!st.count({ seg2, intersecPoint }))
            {
                events.push({ intersecPoint, seg2, Intersection });
                st.insert({ seg2, intersecPoint });
            }

            // 如果已经有交点
            if (recordedIntersections.count(intersecPoint))return;
            else recordedIntersections.insert(intersecPoint);
            // 如果通过所有判断，记录交点和事件
            intersections.push_back(intersecPoint);
        }
    };

    while (!events.empty()) {
        Event event = events.top();
        events.pop();

        int segmentIdx = event.segmentIndex;

        if (event.type == Start || event.type == ReStart) {
            // 计算线段的 key 值
            std::pair<Point, double> key = { segments[segmentIdx].start, segments[segmentIdx].slope };

            // 插入线段到状态树
            auto inserted = statusTree.insert({ key, segmentIdx });
            auto it = inserted;

            // 检查新插入线段与相邻线段是否相交
            if (it != statusTree.begin()) {
                addIntersectionEvent(segmentIdx, std::prev(it)->second);
            }
            if (std::next(it) != statusTree.end()) {
                addIntersectionEvent(segmentIdx, std::next(it)->second);
            }
        }
        else if (event.type == End) {
            // 计算线段的 key 值
            std::pair<Point, double> key = { segments[segmentIdx].start, segments[segmentIdx].slope };
            int seg = event.segmentIndex;

            // 查找要移除的线段
            std::multimap<std::pair<Point, double>, int, statusTreeComparator >::iterator it;

            auto range1 = statusTree.equal_range(key);
            // 删除特定的线段
            for (it = range1.first; it != range1.second; ++it)
            {
                if (it->second == seg)
                {
                    break;
                }
            }

            if (it != statusTree.end())
            {
                // 获取相邻的线段
                auto prevIt = (it == statusTree.begin()) ? statusTree.end() : std::prev(it);
                auto nextIt = std::next(it);

                // 如果存在相邻线段，检查它们之间是否相交
                if (prevIt != statusTree.end() && nextIt != statusTree.end()) {
                    addIntersectionEvent(prevIt->second, nextIt->second);
                }
                // 从状态树中删除线段
                statusTree.erase(it);
            }
        }
        else if (event.type == Intersection) {
            // 处理交点事件
            int seg = event.segmentIndex;

            // 计算线段的 key 值
            std::pair<Point, double> key = { segments[seg].start, segments[seg].slope };

            // 删除原有线段
            // 使用 find 方法确认是否在状态树中存在相同 key 对应的段
            auto range1 = statusTree.equal_range(key);
            // 删除特定的线段
            for (auto it = range1.first; it != range1.second; ++it) {
                if (it->second == seg) {
                    it = statusTree.erase(it);
                    break;
                }
            }

            segments[seg].start = event.point;

            // 到交点了，继续插入检查
            // 把开始加入event
            if (segments[seg].start == segments[seg].end)
                events.push({ segments[seg].start, seg, ReStart });
        }
    }
}

void splitLineByIntersections(const Polygon& polygon,
    const Line& intersectionPoints,
    Polygon& splitLines)
{
    // 遍历每个多边形的每条边
    for (int polygonIndex = 0; polygonIndex < polygon.size(); ++polygonIndex)
    {
        const auto& line = polygon[polygonIndex];
        int n = line.size();
        splitLines.push_back({});

        for (int i = 0; i < n - 1; ++i)
        {
            const Point& start = line[i];
            const Point& end = line[i + 1];

            // 找到当前边上的交点
            Line pointsOnSegment;
            for (const Point& intersection : intersectionPoints)
            {
                if (pointOnSegment2(intersection, start, end))
                {
                    pointsOnSegment.push_back(intersection);
                }
            }

            splitLines.back().push_back(start); // 左闭右开

            if (pointsOnSegment.empty())
            {
                continue;
            }

            // 按离起点的距离对交点排序
            std::sort(pointsOnSegment.begin(), pointsOnSegment.end(),
                [&start](const Point& p1, const Point& p2)
                {
                    return (start - p1).length() < (start - p2).length();
                });

            // 根据排序后的点生成线段
            for (int j = 0; j < pointsOnSegment.size(); ++j)
            {
                splitLines.back().push_back(pointsOnSegment[j]); // 交点是段的起点和终点

                if (splitLines.back().size() == 2 && splitLines.back()[0] == splitLines.back()[1])
                {
                    splitLines.back().pop_back();
                }
                else
                {
                    // 新增一条线段并记录所属图形
                    splitLines.push_back({ pointsOnSegment[j] });
                }
            }
        }
        splitLines.back().push_back(line.back()); // 添加多边形最后一个点
    }
}


void filterSplitLinesCloseToPolyLines(const Polygon& splitLines,
    const Polygon& polyLines,
    const double dis,
    Polygon& filteredSplitLines)
{

    double dis2 = dis * dis - EPSILON_POINT * 2/GlobalScaleView; // 图形上的误差可能比较大

    for (auto& line : splitLines)
    {
        if (line.size() < 2)continue;

        Point point = line[line.size()/2];
        
        if (line.size() == 2)
        {
            point = (point + line[0]) / 2;
        }

        bool isCloseToPolyLines = false;

        for (auto& polyLine : polyLines)
        {
            if (polyLine.size() == 1)
            {
                if ((point - polyLine[0]).length2() < dis2)
                {
                    isCloseToPolyLines = true;
                    break;
                }
            }
            for (int i = 0; i + 1 < polyLine.size() ; i++)
            {
                // 这里误差很大,不能误判边在线内被去掉
                if (pointToSegmentDistance2(point, polyLine[i], polyLine[i + 1]) < dis2)
                {
                    isCloseToPolyLines = true;
                    break;
                }

            }
        }

        if (!isCloseToPolyLines)
        {
            filteredSplitLines.push_back(line);
        }
    }
}

void reconstructPolygons(const Polygon& splitLines, Polygon& mergedPolygons)
{
    // 创建表：记录每个交点的出向边和入向边
    std::unordered_map<Point, std::vector<int>, PointHash2> table;

    // 记录哪些线段已经被使用
    std::vector<bool> used(splitLines.size(), false);

    // 填充表：记录每个点的出向边和入向边
    for (int i = 0; i < splitLines.size(); i++)
    {
        const auto& line = splitLines[i];
        if (line.size() == 1 || (line.size() == 2 && line[0] == line[1]))
        {
            used[i] = true;
            continue;
        }
        table[line.front()].push_back(i); // 起点 -> 出向边
    }

    std::vector<int>path;
    bool isClose = false;
    std::function<void(int, int)> dfs = [&](int start, int pos)
    {
        if (splitLines[pos].back() == splitLines[start].front())
        {
            isClose = true;
            return;
        }

        for (auto& idx : table[splitLines[pos].back()])
        {
            if (used[idx])continue;

            used[idx] = true;
            path.push_back(idx);
            dfs(start, idx);
            if (isClose)return;
            path.pop_back();
            used[idx] = false;
        }
    };

    for (int i = 0; i < used.size(); i++)
    {
        if (!used[i])
        {
            path.push_back(i);
            used[i] = true;
            dfs(i, i);
            if (!isClose)
            {
                path.pop_back();
                used[i] = false;
            }
        }

        if (isClose)
        {
            mergedPolygons.push_back({});
            for (auto& idx : path)
            {
                used[idx] = true;
                mergedPolygons.back().insert(mergedPolygons.back().end(), splitLines[idx].begin(), splitLines[idx].end());
                path.clear();
                isClose = false;
            }
        }
    }
}

bool calculateCompleteLineBuffer(const Polygon& polygon, double dis, Polygon& lines, int step)
{
    // 初步生成缓冲区的线
    Polygon draftLines;
    for (auto& line : polygon)
    {
        draftLineBuffer(line, dis, draftLines, step);
    }

    Line intersections;
    bruteForceFindIntersections(draftLines, intersections);
    //sweepLineFindIntersections(draftLines, intersections);

    Polygon splitLines;
    splitLineByIntersections(draftLines, intersections, splitLines);

    Polygon filterSplitLines;
    filterSplitLinesCloseToPolyLines(splitLines, polygon, dis, filterSplitLines);

    reconstructPolygons(filterSplitLines, lines);



#ifdef DEBUG
    // 插入草图数据到 G1_draftLines
    qDebug() << L("缓冲区草图：%1").arg(draftLines.size());
    G1_draftLines.clear();
    for (auto& line : draftLines)
    {
        G1_draftLines.push_back({});
        for (auto& point : line)
        {
            G1_draftLines.back().push_back({ point.x, point.y });
        }
    }

    // 插入交点数据到 G1_intersections
    qDebug() << L("交点数：%1").arg(intersections.size());
    G1_intersections.clear();
    for (auto& point : intersections)
    {
        G1_intersections.push_back({ { point.x, point.y } });
    }

    // 插入线段数据到 G1_splitLines
    qDebug() << L("线段数：%1").arg(splitLines.size());
    G1_splitLines.clear();
    for (auto& line : splitLines)
    {
        G1_splitLines.push_back({});
        for (auto& point : line)
        {
            G1_splitLines.back().push_back({ point.x, point.y });
        }
    }

    // 插入过滤后线段数据到 G1_filterSplitLines
    qDebug() << L("过滤后线段数：%1").arg(filterSplitLines.size());
    G1_filterSplitLines.clear();
    for (auto& line : filterSplitLines)
    {
        G1_filterSplitLines.push_back({});
        for (auto& point : line)
        {
            G1_filterSplitLines.back().push_back({ point.x, point.y });
        }
    }

    // 插入闭合曲线数据到 G1_boundaryPointss
    qDebug() << L("闭合曲线数：%1").arg(lines.size());
    G1_boundaryPointss.clear();
    for (auto& line : lines)
    {
        G1_boundaryPointss.push_back({});
        for (auto& point : line)
        {
            G1_boundaryPointss.back().push_back({ point.x, point.y });
        }
    }
#endif // DEBUG


    return true;
}



} // namespace GeoBuffer