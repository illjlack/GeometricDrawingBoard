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

    // ����ǶȲ���
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
        // ���淽��
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

    // ���������ߵ��д��߷���
    double x1 = p1.x, y1 = p1.y;
    double x2 = p2.x, y2 = p2.y;
    double x3 = p3.x, y3 = p3.y;

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
    radius = std::sqrt(std::pow(center.x - x1, 2) + std::pow(center.y - y1, 2));  // ����뾶
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
    // ���������յ�ĽǶ�
    double startAngle = std::atan2(startPoint.y - center.y, startPoint.x - center.x);
    double endAngle = std::atan2(endPoint.y - center.y, endPoint.x - center.x);

    startAngle = normalizeAngle(startAngle);
    endAngle = normalizeAngle(endAngle);

    // ����ǶȲ���Ȳ
    double angleDiff = normalizeAngle(endAngle - startAngle);

    // ����Բ���ϵĵ�
    return calculateArcPoints(center, std::sqrt((center.x - startPoint.x) * (center.x - startPoint.x)
        + (center.y - startPoint.y) * (center.y - startPoint.y)), startAngle, angleDiff, steps, arcPoints);
}

// ����������ǵ�tan
double computeHalfAngleTan(const Vector2D& v1, const Vector2D& v2) {
    const Vector2D& dv1 = v1.normalize();
    const Vector2D& dv2 = v2.normalize();

    // ���� sin �� cos
    double cosTheta = dv1 * dv2;
    double sinTheta = std::fabs(dv1 ^ dv2);

    // ���� tan(�� / 2)
    return sinTheta / (1 + cosTheta);
};

// �������ߵĻ�����
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
    // ��ʼ�½���
    polygon.push_back({});

    // ����ʼ����
    auto start = [&](const Point& p1, const Point& p2)
    {
        // ָ��p1�ķ����������ұߵ�λ������*dis
        Vector2D v = (p1 - p2).normal().normalize() * dis;

        // �����˳ʱ�뻭Բ���ұ�
        calculateArcPointsFromStartEndCenter(p1 - v, p1 + v, p1, step, polygon.back());
    };

    // ��������
    auto end = [&](const Point& p1, const Point& p2)
    {
        // ���еķ���ָ������ǰ�棬�����ж�����
        // ָ��p1�ķ����������ұߵ�λ������*dis
        Vector2D v = (p1 - p2).normal().normalize() * dis;
        // β�����ұ�˳ʱ�뻭Բ�����
        calculateArcPointsFromStartEndCenter(p2 + v, p2 - v, p2, step, polygon.back());
    };

    // �����м��
    auto mid = [&](const Point& p0, const Point& p1, const Point& p2)
    {
        // ����ǰ�����η�������
        const Vector2D& v1 = p0 - p1;
        const Vector2D& v2 = p1 - p2;

        if (sgn1(v1 ^ v2) == 0)
        {
            // ���߲��ô���
            return;
        }

        // ѡ��͹��
        if (((v1.normalize()) ^ (v2.normalize())) > 0) // �����ұ��ǰ���
        {
            // �жϼн��Ƿ��С
            double needLen = dis / computeHalfAngleTan(v1, -v2);
            if (v1.length() < needLen || v2.length() < needLen)
            {
                // �н�С,�����ײ�,����
                polygon.back().push_back(p1 + v1.normal().normalize() * dis);

                // �½���
                polygon.push_back({});
                polygon.back().push_back(p1 + v2.normal().normalize() * dis);
            }
            else
            {
                // ֱ���󽻵�
                // �����ƽ���ߵĵ�λ����
                const Vector2D& dv = (v1.normalize() - v2.normalize()).normalize();

                // �����ƽ���ߵĳ��ȣ�����ȷ��ƽ���ߵ�ƫ����
                double sinX = std::fabs(dv ^ (v1.normalize()));
                double disBisector = dis / sinX;

                polygon.back().push_back(p1 + dv * disBisector);
            }
        }
        else // ͹�ǻ�Բ��
        {
            calculateArcPointsFromStartEndCenter(p1 + v1.normal().normalize() * dis, p1 + v2.normal().normalize() * dis, p1, step, polygon.back());
        }
    };

    bool isClose = (polyline.front() == polyline.back());


    
    if(!isClose)
    {
        // �������ߵ�ÿ����(�ұ�)
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

        // �������ߵ�ÿ����(���)
        for (int i = plLen - 2; i > 0; --i)
        {
            mid(polyline[i + 1], polyline[i], polyline[i - 1]);
        }

        // �պ�
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
        // �պ�
        if (polygon.size() > initSize && polygon[initSize].size())
        {
            polygon.back().push_back(polygon[initSize][0]);
        }

        initSize = polygon.size();
        polygon.push_back({});
        // �������ߵ�ÿ����(���)
        for (int i = plLen - 1; i >= 0; --i)
        {
            mid(polyline[(i + 1) % plLen], polyline[i], polyline[(i - 1 + plLen) % plLen]);
        }
        // �պ�
        if (polygon.size() > initSize && polygon[initSize].size())
        {
            polygon.back().push_back(polygon[initSize][0]);
        }

    }




    return true;
}

// �жϵ� q �Ƿ����߶� pr ��(���ȸ�һ�㣬Ϊ�˲�����㽻��)
bool pointOnSegment1(const Point& q, const Point& p, const Point& r)
{
    // ������ֱ�ӷ���
    if ((q - p).directionTo1(r - p))return false;

    // ��� q �Ƿ��ڵ� p �͵� r �ķ�Χ��
    return (q.x <= std::max(p.x, r.x) + EPSILON &&
        q.x >= std::min(p.x, r.x) - EPSILON &&
        q.y <= std::max(p.y, r.y) + EPSILON &&
        q.y >= std::min(p.y, r.y) - EPSILON);
}

// �жϵ� q �Ƿ����߶� pr ��(���ȵ�һ�㣬�ָ���Ƿ�ָ)
bool pointOnSegment2(const Point& q, const Point& p, const Point& r)
{
    // ������ֱ�ӷ���
    if ((q - p).directionTo2(r - p))return false;

    // ��� q �Ƿ��ڵ� p �͵� r �ķ�Χ��
    return (q.x <= std::max(p.x, r.x) + EPSILON_POINT &&
        q.x >= std::min(p.x, r.x) - EPSILON_POINT &&
        q.y <= std::max(p.y, r.y) + EPSILON_POINT &&
        q.y >= std::min(p.y, r.y) - EPSILON_POINT);
}

// �߶��Ƿ��ཻ�ͼ�������
bool segmentsIntersect(const Point& p1Start, const Point& p1End,
    const Point& p2Start, const Point& p2End,
    Point& intersection)
{
    // ���㷽��
    int o1 = (p1End - p1Start).directionTo1(p2Start - p1Start);
    int o2 = (p1End - p1Start).directionTo1(p2End - p1Start);
    int o3 = (p2End - p2Start).directionTo1(p1Start - p2Start);
    int o4 = (p2End - p2Start).directionTo1(p1End - p2Start);

    // ͨ���ཻ���������߶��ڶԷ����ࣩ
    if (o1 != o2 && o3 != o4)
    {
        // ���㽻��
        double a1 = p1End.y - p1Start.y;
        double b1 = p1Start.x - p1End.x;
        double c1 = a1 * p1Start.x + b1 * p1Start.y;

        double a2 = p2End.y - p2Start.y;
        double b2 = p2Start.x - p2End.x;
        double c2 = a2 * p2Start.x + b2 * p2Start.y;

        double determinant = a1 * b2 - a2 * b1;

        if (std::fabs(determinant) < EPSILON)
        {
            return false; // ƽ���߶�
        }

        // ���㽻������
        intersection.x = ((b2 * c1 - b1 * c2) / determinant);
        intersection.y = ((a1 * c2 - a2 * c1) / determinant);

        // ��齻���Ƿ����߶η�Χ��
        if (pointOnSegment1(intersection, p1Start, p1End) && pointOnSegment1(intersection, p2Start, p2End))
        {
            return true;
        }
        return false;
    }

    // ����������߶ι��������ص����ص���˳������ν��
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

    return false; // û���ཻ
}

// ����������н��㣨������ͬһ�������Լ��Ľ��㣩
void bruteForceFindIntersections(const Polygon& polygons, Line& intersections)
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
    std::vector<Segment> segments;

    for (int lineIndex = 0; lineIndex < polygons.size(); ++lineIndex)
    {
        const auto& lines = polygons[lineIndex];
        for (int i = 0; i + 1 < lines.size(); ++i)
        {
            segments.emplace_back(Segment(lines[i], lines[i + 1], lineIndex, i));
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
                if (std::abs(static_cast<int>(segments[i].segmentIndex) - static_cast<int>(segments[j].segmentIndex)) == 1)
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

// �Ƚ�������std�����ã�
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

        // ������Ϊ���Ӱ������
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

// ���ڱȽ� Point �Ĺ�ϣ�����
struct PointHash
{
    std::size_t operator()(const Point& point) const
    {
        auto roundedX = std::round(point.x / EPSILON) * EPSILON;
        auto roundedY = std::round(point.y / EPSILON) * EPSILON;
        return std::hash<double>()(roundedX) ^ (std::hash<double>()(roundedY) << 1);
    }
};

// ���ڱȽ� Point �Ĺ�ϣ�����
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
        return h1 ^ (h2 << 1); // XOR ���������
    }
};

double pointToSegmentDistance2(const Point& point, const Point& start, const Point& end) {
    // �����߶ε�����
    const Point& segment = end - start;
    const Point& vectorToPoint = point - start;

    // �����߶εĳ���
    double segmentLength = segment.length();

    // ����߶εĳ���Ϊ�㣬���ص㵽���ľ���
    if (std::fabs(segmentLength) < EPSILON) {
        return vectorToPoint.length2();
    }

    // ����ͶӰ��ı���t
    double t = (vectorToPoint * segment) / (segmentLength * segmentLength);
    t = std::max(0.0, std::min(1.0, t));  // ����t��0��1֮��

    // ����ͶӰ�������
    Point projection = start + segment * t;

    // ���㲢���ص㵽ͶӰ��ľ���
    return (point - projection).length2();
}

// �߶νṹ��
struct Segment
{
    Point start, end;

    // ȷ���߶ε����С���յ㣨�� x ����
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
                slope = std::numeric_limits<double>::infinity();  // ������
            }
            else if (end.y < start.y)
            {
                slope = -std::numeric_limits<double>::infinity(); // ������
            }
            else
            {
                slope = 0; // ͬһ�㣨������ͬ��
            }
        }
        else slope = (end.y - start.y) / (end.x - start.x);
    }

    double slope; // ������㣬���ظ�����
};

void sweepLineFindIntersections(const Polygon& pointss, Line& intersections, bool isArea) {

    // �¼�����
    enum EventType { Intersection = 1, Start, End, ReStart };

    struct Event {
        Point point;                          // ��ǰ�¼�������
        int segmentIndex;                       // �����߶ε�����
        EventType type;                         // �¼�����

        // ���رȽ���������������ȶ��У�
        bool operator>(const Event& other) const
        {
            // ���ȱȽ� x ����
            if (std::fabs(point.x - other.point.x) > EPSILON)
            {
                return point.x > other.point.x;
            }

            return type > other.type;
            // y ����Ҫ
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
        for (int i = 0; i + 1 < points.size(); ++i) {
            // �����߶�
            Segment segment(points[i], points[i + 1]);
            segments.push_back(segment);
            cnt++;


            // ���������յ��¼�
            Event startEvent{
                segment.start,                                  // �������
                cnt - 1,                                        // �߶�����
                Start                                           // �¼�����
            };

            Event endEvent{
                segment.end,                                    // �յ�����
                cnt - 1,                                        // �߶�����
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
    struct statusTreeComparator {
        bool operator()(const std::pair<Point, double>& a, const std::pair<Point, double>& b) const {
            // ȷ���Ƚϵ� x ֵ���ϴ�ֵ��
            double x_target = std::max(a.first.x, b.first.x);

            double y_a;
            double y_b;

            if (x_target > a.first.x)
            {
                y_a = a.first.y + a.second * (x_target - a.first.x);
            }
            else
            {
                y_a = a.first.y; // ����б����������ʱ��nan
            }

            if (x_target > b.first.x)
            {
                y_b = b.first.y + b.second * (x_target - b.first.x);
            }
            else
            {
                y_b = b.first.y; // ����б����������ʱ��nan
            }

            // �Ƚ� y ֵ�����Ǿ���
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

    std::unordered_set<Point, PointHash, PointEqual> recordedIntersections;
    std::unordered_set<std::pair<int, Point>, PairIPHash, PairIPEqual> st;

    auto addIntersectionEvent = [&](int seg1, int seg2)
    {
        if (isAdjacentSegments(seg1, seg2))
        {
            return; // ����������ڣ����Լ��㽻��
        }
        Point intersecPoint;
        if (segmentsIntersect(segments[seg1].start, segments[seg1].end, segments[seg2].start, segments[seg2].end, intersecPoint))
        {
            // �������������ߵ������¼�����һ��
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

            // ����Ѿ��н���
            if (recordedIntersections.count(intersecPoint))return;
            else recordedIntersections.insert(intersecPoint);
            // ���ͨ�������жϣ���¼������¼�
            intersections.push_back(intersecPoint);
        }
    };

    while (!events.empty()) {
        Event event = events.top();
        events.pop();

        int segmentIdx = event.segmentIndex;

        if (event.type == Start || event.type == ReStart) {
            // �����߶ε� key ֵ
            std::pair<Point, double> key = { segments[segmentIdx].start, segments[segmentIdx].slope };

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
            std::pair<Point, double> key = { segments[segmentIdx].start, segments[segmentIdx].slope };
            int seg = event.segmentIndex;

            // ����Ҫ�Ƴ����߶�
            std::multimap<std::pair<Point, double>, int, statusTreeComparator >::iterator it;

            auto range1 = statusTree.equal_range(key);
            // ɾ���ض����߶�
            for (it = range1.first; it != range1.second; ++it)
            {
                if (it->second == seg)
                {
                    break;
                }
            }

            if (it != statusTree.end())
            {
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
            int seg = event.segmentIndex;

            // �����߶ε� key ֵ
            std::pair<Point, double> key = { segments[seg].start, segments[seg].slope };

            // ɾ��ԭ���߶�
            // ʹ�� find ����ȷ���Ƿ���״̬���д�����ͬ key ��Ӧ�Ķ�
            auto range1 = statusTree.equal_range(key);
            // ɾ���ض����߶�
            for (auto it = range1.first; it != range1.second; ++it) {
                if (it->second == seg) {
                    it = statusTree.erase(it);
                    break;
                }
            }

            segments[seg].start = event.point;

            // �������ˣ�����������
            // �ѿ�ʼ����event
            if (segments[seg].start == segments[seg].end)
                events.push({ segments[seg].start, seg, ReStart });
        }
    }
}

void splitLineByIntersections(const Polygon& polygon,
    const Line& intersectionPoints,
    Polygon& splitLines)
{
    // ����ÿ������ε�ÿ����
    for (int polygonIndex = 0; polygonIndex < polygon.size(); ++polygonIndex)
    {
        const auto& line = polygon[polygonIndex];
        int n = line.size();
        splitLines.push_back({});

        for (int i = 0; i < n - 1; ++i)
        {
            const Point& start = line[i];
            const Point& end = line[i + 1];

            // �ҵ���ǰ���ϵĽ���
            Line pointsOnSegment;
            for (const Point& intersection : intersectionPoints)
            {
                if (pointOnSegment2(intersection, start, end))
                {
                    pointsOnSegment.push_back(intersection);
                }
            }

            splitLines.back().push_back(start); // ����ҿ�

            if (pointsOnSegment.empty())
            {
                continue;
            }

            // �������ľ���Խ�������
            std::sort(pointsOnSegment.begin(), pointsOnSegment.end(),
                [&start](const Point& p1, const Point& p2)
                {
                    return (start - p1).length() < (start - p2).length();
                });

            // ���������ĵ������߶�
            for (int j = 0; j < pointsOnSegment.size(); ++j)
            {
                splitLines.back().push_back(pointsOnSegment[j]); // �����Ƕε������յ�

                if (splitLines.back().size() == 2 && splitLines.back()[0] == splitLines.back()[1])
                {
                    splitLines.back().pop_back();
                }
                else
                {
                    // ����һ���߶β���¼����ͼ��
                    splitLines.push_back({ pointsOnSegment[j] });
                }
            }
        }
        splitLines.back().push_back(line.back()); // ��Ӷ�������һ����
    }
}


void filterSplitLinesCloseToPolyLines(const Polygon& splitLines,
    const Polygon& polyLines,
    const double dis,
    Polygon& filteredSplitLines)
{

    double dis2 = dis * dis - EPSILON_POINT * 2/GlobalScaleView; // ͼ���ϵ������ܱȽϴ�

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
                // �������ܴ�,�������б������ڱ�ȥ��
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
    // ��������¼ÿ������ĳ���ߺ������
    std::unordered_map<Point, std::vector<int>, PointHash2> table;

    // ��¼��Щ�߶��Ѿ���ʹ��
    std::vector<bool> used(splitLines.size(), false);

    // ������¼ÿ����ĳ���ߺ������
    for (int i = 0; i < splitLines.size(); i++)
    {
        const auto& line = splitLines[i];
        if (line.size() == 1 || (line.size() == 2 && line[0] == line[1]))
        {
            used[i] = true;
            continue;
        }
        table[line.front()].push_back(i); // ��� -> �����
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
    // �������ɻ���������
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
    // �����ͼ���ݵ� G1_draftLines
    qDebug() << L("��������ͼ��%1").arg(draftLines.size());
    G1_draftLines.clear();
    for (auto& line : draftLines)
    {
        G1_draftLines.push_back({});
        for (auto& point : line)
        {
            G1_draftLines.back().push_back({ point.x, point.y });
        }
    }

    // ���뽻�����ݵ� G1_intersections
    qDebug() << L("��������%1").arg(intersections.size());
    G1_intersections.clear();
    for (auto& point : intersections)
    {
        G1_intersections.push_back({ { point.x, point.y } });
    }

    // �����߶����ݵ� G1_splitLines
    qDebug() << L("�߶�����%1").arg(splitLines.size());
    G1_splitLines.clear();
    for (auto& line : splitLines)
    {
        G1_splitLines.push_back({});
        for (auto& point : line)
        {
            G1_splitLines.back().push_back({ point.x, point.y });
        }
    }

    // ������˺��߶����ݵ� G1_filterSplitLines
    qDebug() << L("���˺��߶�����%1").arg(filterSplitLines.size());
    G1_filterSplitLines.clear();
    for (auto& line : filterSplitLines)
    {
        G1_filterSplitLines.push_back({});
        for (auto& point : line)
        {
            G1_filterSplitLines.back().push_back({ point.x, point.y });
        }
    }

    // ����պ��������ݵ� G1_boundaryPointss
    qDebug() << L("�պ���������%1").arg(lines.size());
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