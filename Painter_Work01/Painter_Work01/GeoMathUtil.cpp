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

using Line = QVector<QPointF>;                 // 表示一条线
using Polygon = QVector<Line>;                 // 表示一个面
using Polygons = QVector<Polygon>;             // 表示面集合

const double EPSILON = 1e-4;

// ==========================================================================
// MathTools
// ==========================================================================

// 比较器（给std容器用）
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

// 用于比较 QPointF 的哈希和相等
struct QPointFHash
{
    std::size_t operator()(const QPointF& point) const
    {
        auto roundedX = std::round(point.x() / EPSILON) * EPSILON;
        auto roundedY = std::round(point.y() / EPSILON) * EPSILON;
        return std::hash<double>()(roundedX) ^ (std::hash<double>()(roundedY) << 1);
    }
};

// 精度处理
int sgn(double x) 
{
    if (x < -EPSILON) return -1;
    else if (x > EPSILON) return 1;
    else return 0;
}

// 向量标准化
std::pair<double, double> normalize(double x, double y) 
{
    double dis = std::sqrt(x * x + y * y);
    return { x / dis, y / dis };
}

// 向量叉乘
// 可以用右手螺旋法则来判断方向， 除大拇指外， 四指朝1方向向2方向转， 此时大拇指方向即是叉乘结果方向， 向上为正;正的话，2在1的左边
inline double cross(double x1, double y1, double x2, double y2)
{
    return x1 * y2 - x2 * y1;
}

// 向量点乘
// 点乘的几何意义：表示两个向量的投影关系，反映它们的夹角方向
inline double dot(double x1, double y1, double x2, double y2)
{
    return x1 * x2 + y1 * y2;
}

// 计算向量长度
inline double length(double x, double y)
{
    return std::sqrt(x * x + y * y);
}

// QPointF版本叉乘
inline double cross(const QPointF& a, const QPointF& b)
{
    return cross(a.x(), a.y(), b.x(), b.y());
}

// QPointF版本点乘
inline double dot(const QPointF& a, const QPointF& b)
{
    return dot(a.x(), a.y(), b.x(), b.y());
}

// QPointF版本长度
inline double length(const QPointF& a)
{
    return length(a.x(), a.y());
}

// 判断点与向量的位置关系
int pointRelativeToVector(const QPointF& point, const QPointF& vectorStart, const QPointF& vectorEnd)
{
    // 计算向量 AB 和 AP 的叉积
    double crossProduct = cross(vectorEnd.x() - vectorStart.x(),
        vectorEnd.y() - vectorStart.y(),
        point.x() - vectorStart.x(),
        point.y() - vectorStart.y());

    // 根据叉积判断位置关系
    if (std::fabs(crossProduct) < EPSILON) {
        return 0; // 点在向量上
    }
    else if (crossProduct > 0) {
        return 1; // 点在向量左侧
    }
    else {
        return -1; // 点在向量右侧
    }
}

// 判断点 q 是否在线段 pr 上
bool pointOnSegment(const QPointF& q, const QPointF& p, const QPointF& r)
{
    // 不共线直接返回
    if(pointRelativeToVector(p, q, r))return false; 

    // 检查 q 是否在点 p 和点 r 的范围内
    return (q.x() <= std::max(p.x(), r.x()) && q.x() >= std::min(p.x(), r.x()) &&
        q.y() <= std::max(p.y(), r.y()) && q.y() >= std::min(p.y(), r.y()));
}

// 线段是否相交和计算坐标
bool segmentsIntersect(const QPointF& p1Start, const QPointF& p1End,
    const QPointF& p2Start, const QPointF& p2End,
    QPointF& intersection)
{
    // 计算方向
    int o1 = pointRelativeToVector(p1Start, p1End, p2Start);
    int o2 = pointRelativeToVector(p1Start, p1End, p2End);
    int o3 = pointRelativeToVector(p2Start, p2End, p1Start);
    int o4 = pointRelativeToVector(p2Start, p2End, p1End);

    // 通用相交条件（两线段在对方两侧）
    if (o1 != o2 && o3 != o4)
    {
        // 计算交点
        double a1 = p1End.y() - p1Start.y();
        double b1 = p1Start.x() - p1End.x();
        double c1 = a1 * p1Start.x() + b1 * p1Start.y();

        double a2 = p2End.y() - p2Start.y();
        double b2 = p2Start.x() - p2End.x();
        double c2 = a2 * p2Start.x() + b2 * p2Start.y();

        double determinant = a1 * b2 - a2 * b1;

        if (determinant == 0) 
        {
            return false;  // 平行线段
        }

        // 计算交点坐标
        intersection.setX((b2 * c1 - b1 * c2) / determinant);
        intersection.setY((a1 * c2 - a2 * c1) / determinant);
        return true;
    }

    // 特殊情况：线段共线且有重叠
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

    return false;  // 没有相交
}

// 点是否在面内
bool isPointInsidePolygon(const QPointF& point, const Polygon& polygon)
{
    for (const auto& ring : polygon) // 遍历每一个闭合环（外环或内环）
    {
        int windingNumber = 0; // 用于统计点的绕数，判断是否在多边形内

        for (int i = 0; i < ring.size(); ++i)
        {
            QPointF p1 = ring[i];
            QPointF p2 = ring[(i + 1) % ring.size()]; // 下一条边

            // 判断点在边的上下关系
            if (p1.y() <= point.y())
            {
                if (p2.y() > point.y() && cross(p1 - point, p2 - point) > 0)
                {
                    ++windingNumber; // 点从下向上穿过多边形边
                }
            }
            else
            {
                if (p2.y() <= point.y() && cross(p1 - point, p2 - point) < 0)
                {
                    --windingNumber; // 点从上向下穿过多边形边
                }
            }
        }

        // 如果绕数不为 0，说明点在多边形内
        if (windingNumber != 0)
        {
            return true;
        }
    }

    return false; // 点不在多边形内
}

double pointToSegmentDistance(const QPointF& point, const QPointF& start, const QPointF& end) {
    // 计算线段的向量
    QPointF segment = end - start;
    QPointF vectorToPoint = point - start;

    // 计算线段的长度
    double segmentLength = length(segment);

    // 如果线段的长度为零，返回点到起点的距离
    if (segmentLength == 0) {
        return length(vectorToPoint);
    }

    // 计算投影点的比例t
    double t = dot(vectorToPoint, segment) / (segmentLength * segmentLength);
    t = std::max(0.0, std::min(1.0, t));  // 限制t在0到1之间

    // 计算投影点的坐标
    QPointF projection = start + t * segment;

    // 计算并返回点到投影点的距离
    return length(point - projection);
}

// 判断正负缓冲区（有向多边形面积公式）(计算线的顺逆)
bool isClockwise(const Line& line) {
    double sum = 0.0;
    for (int i = 0; i < line.size() - 1; ++i) {
        const auto& p1 = line[i];
        const auto& p2 = line[i + 1];
        sum += (p2.x() - p1.x()) * (p2.y() + p1.y());
    }
    return sum > 0.0; // >0 表示顺时针，<0 表示逆时针
}


// ==========================================================================
// 计算线段上的点
// ==========================================================================

/**
 * 根据节点类型和控制点计算线上的点
 * @param nodeLineStyle 线的类型（例如折线、样条、三点圆弧等）
 * @param controlPoints 控制点列表，用于计算线的形状
 * @param linePoints 输出参数，保存计算得到的线段上的点
 * @param steps 步数，决定计算多少个中间点
 * @return 如果成功计算则返回 true，失败则返回 false
 */
bool calculateLinePoints(NodeLineStyle lineStyle, const Line& controlPoints, Line& linePoints, int steps)
{
    linePoints.clear();
    switch (lineStyle)
    {

    case NodeLineStyle::StyleStreamline: // 流线和折线一样直接使用控制点
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
 * 根据节点类型和控制点计算闭合线上的点（多做了一下闭合）
 * @param lineStyle 线的类型
 * @param controlPoints 控制点列表
 * @param linePoints 输出参数，保存计算得到的闭合线上的点
 * @param steps 步数，决定计算多少个中间点
 * @return 如果成功计算则返回 true，失败则返回 false
 */
bool calculateCloseLinePoints(NodeLineStyle lineStyle, const Line& controlPoints, Line& linePoints, int steps)
{
    linePoints.clear();
    Line newControlPoints;

    switch (lineStyle)
    {
    case NodeLineStyle::StyleStreamline:  // 流线和折线一样直接使用控制点
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

        if ((controlPoints.size() - 1) & 1)  // 少一个与第一个控制点一起画一个圆
        {
            newControlPoints = controlPoints;
            newControlPoints.push_back(controlPoints[0]);
            return calculateThreeArcPointsFromControlPoints(newControlPoints, steps, linePoints);
        }
        else
        {
            if (calculateThreeArcPointsFromControlPoints(controlPoints, steps, linePoints))
            {
                linePoints.push_back(linePoints[0]); // 直接连上第一个点
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
 * 根据多个线段的组件和控制点计算线上的点
 * @param component 组件列表
 * @param controlPoints 控制点列表
 * @param linePointss 输出参数，保存计算得到的线段上的点
 * @param steps 步数，决定计算多少个中间点
 * @return 返回成功绘制分图的个数
 */
int calculateLinePoints(const QVector<Component>& components, const Line& controlPoints, Polygon& linePointss, int steps)
{
    linePointss.clear();  // 清空结果数组
    int startIdx = 0;
    int result = 0; // 多少条线绘制完成

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
 * 根据多个线段的组件和控制点计算闭合线上的点
 * @param component 组件列表
 * @param controlPoints 控制点列表
 * @param linePointss 输出参数，保存计算得到的闭合线上的点
 * @param steps 步数，决定计算多少个中间点
 * @return 返回成功绘制分图的个数
 */
int calculateCloseLinePoints(const QVector<Component>& components, const Line& controlPoints, Polygon& linePointss, int steps)
{
    linePointss.clear();  // 清空结果数组
    int startIdx = 0;
    int result = 0; // 多少条线绘制完成

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
 * 根据多个线段的组件和控制点计算平行线（第二个分离一个点来控制距离）
 * @param component 组件列表
 * @param controlPoints 控制点列表
 * @param linePointss 输出参数，保存计算得到的闭合线上的点
 * @param steps 步数，决定计算多少个中间点
 * @return 返回成功绘制分图的个数
 */
int calculateParallelLinePoints(const QVector<Component>& components, const Line& controlPoints, Polygon& linePointss, int steps)
{
    linePointss.clear();  // 清空结果数组
    int startIdx = 0;
    int result = 0; // 多少条线绘制完成

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
    
    
        if (components.size() == 2 && components[1].len) // 绘制平行线
        {
            Line linePoints;
            result += calculateParallelLineThroughPoint(linePointss[0], controlPoints.back(),linePoints);
            linePointss.push_back(linePoints);
        }
    }
    return result;
}


// ==========================================================================
// 样条计算
// ==========================================================================

/**
 * 计算组合数（n 选 i），即计算 C(n, i)
 * @param n 总数
 * @param i 选择数
 * @return 计算出的组合数值
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
 * 计算n次贝塞尔曲线的点（二维）
 * @param controlPoints 贝塞尔曲线的控制点列表
 * @param t 插值因子，通常在[0, 1]之间变化
 * @return 曲线上的一个点
 */
QPointF bezierN(const Line& controlPoints, double t)
{
    int n = controlPoints.size() - 1;
    QPointF point(0.0, 0.0); // 用来存储计算结果

    // 贝塞尔曲线的求和公式
    for (int i = 0; i <= n; ++i) {
        int binom = binomialCoefficient(n, i);
        double weight = binom * pow(1 - t, n - i) * pow(t, i);
        point += weight * controlPoints[i];
    }

    return point;
}

/**
 * 计算n次贝塞尔曲线上的多个点（二维）
 * @param controlPoints 贝塞尔曲线的控制点列表
 * @param numPoints 计算多少个曲线点
 * @return 包含所有计算得到的贝塞尔曲线点的列表
 */
Line calculateBezierCurve(const Line& controlPoints, int numPoints)
{
    Line curvePoints;

    // 计算曲线上的多个点
    for (int i = 0; i <= numPoints; ++i) {
        double t = i / (double)numPoints; // t 值的变化
        QPointF point = bezierN(controlPoints, t); // 计算当前t值对应的贝塞尔点
        curvePoints.append(point);
    }

    return curvePoints;
}

/**
 * Cox-de Boor 递归公式，用于计算 B 样条曲线上的点
 * @param knots 节点向量
 * @param i 当前节点的索引
 * @param p B 样条的次数
 * @param t 插值因子
 * @return 当前插值位置的权重
 */
double coxDeBoor(const QVector<double>& knots, int i, int p, double t)
{
    if (p == 0) {
        // 基础情况
        return (knots[i] <= t && t < knots[i + 1]) ? 1.0 : 0.0;
    }

    double left = 0.0, right = 0.0;

    // 左递归部分
    if (knots[i + p] != knots[i]) {
        left = (t - knots[i]) / (knots[i + p] - knots[i]) * coxDeBoor(knots, i, p - 1, t);
    }

    // 右递归部分
    if (knots[i + p + 1] != knots[i + 1]) {
        right = (knots[i + p + 1] - t) / (knots[i + p + 1] - knots[i + 1]) * coxDeBoor(knots, i + 1, p - 1, t);
    }

    return left + right;
}

/**
 * 生成 B 样条曲线的节点向量
 * @param n 控制点数
 * @param degree 曲线的次数
 * @return 生成的节点向量
 */
QVector<double> generateKnotVector(int n, int degree)
{
    int m = n + degree + 2;  // 节点向量大小
    QVector<double> knots(m);

    for (int i = 0; i < m; ++i) {
        if (i <= degree) {
            knots[i] = 0.0;  // 前 degree+1 个值为 0
        }
        else if (i >= m - degree - 1) {
            knots[i] = 1.0;  // 后 degree+1 个值为 1
        }
        else {
            knots[i] = (i - degree) / static_cast<double>(n - degree + 1);  // 中间均匀分布
        }
    }

    return knots;
}

/**
 * 计算 B 样条曲线上的多个点（二维）
 * @param controlPoints 控制点列表
 * @param degree B 样条的次数
 * @param numCurvePoints 要计算的曲线点数
 * @param curvePoints 输出参数，保存计算得到的曲线点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateBSplineCurve(const Line& controlPoints, int degree, int numCurvePoints, Line& curvePoints)
{
    // 控制点数量小于degree时，无法计算B样条曲线
    int n = controlPoints.size() - 1;
    if (n < degree) {
        return false;  // 无法计算样条曲线
    }

    // 生成节点向量
    QVector<double> knots = generateKnotVector(n, degree);

    // 计算曲线上的点
    for (int i = 0; i < numCurvePoints; ++i) 
    {
        double t = i / static_cast<double>(numCurvePoints);  // t 范围 [0, 1]

        QPointF point(0.0, 0.0);

        // 按控制点计算加权值
        for (int j = 0; j <= n; ++j) 
        {
            double weight = coxDeBoor(knots, j, degree, t);
            point += weight * controlPoints[j];
        }

        curvePoints.append(point);  // 存储计算得到的曲线点
    }

    return true;  // 计算成功
}

// ==========================================================================
// 圆弧计算
// ==========================================================================

/**
 * 规范化角度，确保角度在[0, 2π)范围内，并支持顺时针为正
 * @param angle 输入角度（弧度制）
 * @return 规范化后的角度（弧度制）
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
 * 根据三个点计算圆心和半径
 * @param p1 圆上的第一个点
 * @param p2 圆上的第二个点
 * @param p3 圆上的第三个点
 * @param center 输出参数，计算得到的圆心坐标
 * @param radius 输出参数，计算得到的圆的半径
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateCircle(const QPointF& p1, const QPointF& p2, const QPointF& p3, QPointF& center, double& radius)
{
    if (p1 == p2 || p2 == p3 || p3 == p1)
    {
        return false;  // 点重合，无法计算圆
    }

    // 计算两条边的中垂线方程
    double x1 = p1.x(), y1 = p1.y();
    double x2 = p2.x(), y2 = p2.y();
    double x3 = p3.x(), y3 = p3.y();

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

    center = QPointF(center_x, center_y);
    radius = std::sqrt(std::pow(center.x() - x1, 2) + std::pow(center.y() - y1, 2));  // 计算半径
    return true;
}

/**
 * 根据两个点计算圆心和半径
 * @param p1 圆上的第一个点
 * @param p2 圆上的第二个点
 * @param center 输出参数，计算得到的圆心坐标
 * @param radius 输出参数，计算得到的圆的半径
 * @return 如果计算成功则返回 true，失败则返回 false
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
 * 根据圆心、半径、起始角度、角度差值和步数计算弧线上的点(正的逆时针)
 * @param center 圆心坐标
 * @param radius 圆的半径
 * @param startAngle 起始角度（弧度制）
 * @param angleDiff 角度差值（弧度制）
 * @param steps 步数，决定计算多少个点
 * @param points 输出参数，保存计算得到的弧线上的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateArcPoints(const QPointF& center, double radius, double startAngle, double angleDiff, int steps, Line& points)
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

        double x = center.x() + radius * std::cos(angle);
        double y = center.y() + radius * std::sin(angle);

        points.append(QPointF(x, y));
    }
    return true;
}

/**
 * 根据三个点计算弧线上的点
 * @param point1 圆上的第一个点
 * @param point2 圆上的第二个点
 * @param point3 圆上的第三个点
 * @param steps 步数，决定计算多少个点
 * @param arcPoints 输出参数，保存计算得到的弧线上的点
 * @return 如果计算成功则返回 true，失败则返回 false
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
 * 根据两个点计算圆上的点
 * @param point1 圆上的第一个点
 * @param point2 圆上的第二个点
 * @param steps 步数，决定计算多少个点
 * @param arcPoints 输出参数，保存计算得到的圆上的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateCirclePointsFromControlPoints(const QPointF& point1, const QPointF& point2, int steps, Line& arcPoints)
{
    QPointF center;
    double radius;

    // 追加
    // arcPoints.clear();

    if (!calculateCircle(point1, point2, center, radius)) {
        return false;
    }

    // 计算起始、结束、和中间点的角度，弧度制直接计算
    double startAngle = std::atan2(point1.y() - center.y(), point1.x() - center.x());

    return calculateArcPoints(center, radius, startAngle, 2 * M_PI, steps, arcPoints);
}

/**
 * 计算圆弧上的点（通过控制点）
 * @param controlPoints 控制点列表
 * @param steps 步数，决定计算多少个点
 * @param arcPoints 输出参数，保存计算得到的弧线上的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateArcPointsFromControlPoints(const Line& controlPoints, int steps, Line& arcPoints)
{
    if (!calculateArcPointsFromThreePoints(controlPoints[0], controlPoints[1], controlPoints[2], steps, arcPoints))
    {
        return false;
    }

    for (int i = 2; i + 1 < controlPoints.size(); i++)
    {
        // 绘制失败就直接返回
        if (!calculateArcPointsFromThreePoints(arcPoints[arcPoints.size() - 2], controlPoints[i], controlPoints[i + 1], steps, arcPoints))
        {
            return true;
        }
    }
    return true;
}

/**
 * 计算三点圆弧上的点（通过控制点）
 * @param controlPoints 控制点列表
 * @param steps 步数，决定计算多少个点
 * @param arcPoints 输出参数，保存计算得到的弧线上的点
 * @return 如果计算成功则返回 true，失败则返回 false
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
 * 调整圆弧的角度差值，确保在指定的角度范围内
 * @param angleDiff 角度差值
 * @param angleLimit 角度范围
 * @return 调整后的角度差值
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
// 平行线计算
// 直线方向统一从后面的点指向前面的点
// ==========================================================================
/**
 * 计算折线的双边平行线
 * @param polyline 输入折线的点列表
 * @param dis 平行线与折线的距离
 * @param leftPolyline 输出参数，保存平行线的左侧点
 * @param rightPolyline 输出参数，保存平行线的右侧点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateParallelLine(const Line& polyline, double dis, Line& leftPolyline, Line& rightPolyline)
{
    // 思路：平行线段处法向量的dis远的点（实际只用求起点和终点），夹角处相邻两平行线段相交，求角平分线处向量dis/sin(一半夹角)处的点
    // 
    // 判断折线点数是否有效，折线必须至少有两个点
    int plLen = polyline.size();
    if (plLen < 2)
    {
        return false; // 折线至少需要两个点
    }

    // 清空输出参数
    // leftPolyline.clear();
    // rightPolyline.clear();

    // 遍历折线的每个点，计算平行线
    for (int i = 0; i < plLen; ++i)
    {
        if (i == 0)
        {
            // 如果是折线的起点
            double x1 = polyline[i].x(), y1 = polyline[i].y();
            double x2 = polyline[i + 1].x(), y2 = polyline[i + 1].y();

            // 第一个点到第二个点的垂直方向向量（法向量）
            // (y1 - y2, x1 - x2) 垂直于 (y1 - y2, x2 - x1)
            auto [vx, vy] = normalize(y1 - y2, x2 - x1);

            leftPolyline.append(QPointF(x1 + vx * dis, y1 + vy * dis));  // 向左偏移
            rightPolyline.append(QPointF(x1 - vx * dis, y1 - vy * dis));  // 向右偏移
        }
        else if (i == plLen - 1)
        {
            // 如果是折线的终点
            double x1 = polyline[i - 1].x(), y1 = polyline[i - 1].y();
            double x2 = polyline[i].x(), y2 = polyline[i].y();

            auto [vx, vy] = normalize(y1 - y2, x2 - x1);

            leftPolyline.append(QPointF(x2 + vx * dis, y2 + vy * dis));  // 向左偏移
            rightPolyline.append(QPointF(x2 - vx * dis, y2 - vy * dis));  // 向右偏移
        }
        else
        {
            // 对于折线的中间点
            double x0 = polyline[i].x(), y0 = polyline[i].y();
            double x1 = polyline[i - 1].x(), y1 = polyline[i - 1].y();
            double x2 = polyline[i + 1].x(), y2 = polyline[i + 1].y();

            // 计算前后两段向量的单位向量
            auto [x01, y01] = normalize(x1 - x0, y1 - y0);
            auto [x02, y02] = normalize(x2 - x0, y2 - y0);

            if (sgn(cross(x01, y01, x02, y02)) == 0)
            {
                continue;  // 如果共线，跳过该点
            }

            // 计算角平分线的单位向量（两个向量的平均）
            auto [vx, vy] = normalize((x01 + x02) / 2, (y01 + y02) / 2);

            // 计算角平分线的长度，用于确定平行线的偏移量
            double sinX = std::fabs(cross(vx, vy, x02, y02));
            double disBisector = dis / sinX;  // 使用叉乘来确定夹角的大小，得出平行线的距离

            // 根据叉乘的符号确定左侧和右侧的平行线
            if (cross(x1 - x0, y1 - y0, x2 - x0, y2 - y0) > 0)
            {
                leftPolyline.append(QPointF(x0 - vx * disBisector, y0 - vy * disBisector));  // 向左偏移
                rightPolyline.append(QPointF(x0 + vx * disBisector, y0 + vy * disBisector));  // 向右偏移
            }
            else
            {
                leftPolyline.append(QPointF(x0 + vx * disBisector, y0 + vy * disBisector));  // 向左偏移
                rightPolyline.append(QPointF(x0 - vx * disBisector, y0 - vy * disBisector));  // 向右偏移
            }
        }
    }
    return true;
}

/**
 * 计算折线的单边平行线(ids正值为左侧，负值为右侧)
 * @param polyline 输入折线的点列表
 * @param dis 平行线与折线的距离
 * @param parallelPolyline 输出参数，保存单边平行线的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateParallelLine(const Line& polyline, double dis, Line& parallelPolyline)
{
    // 思路：平行线段处法向量的dis远的点（实际只用求起点和终点），夹角处相邻两平行线段相交，求角平分线处向量dis/sin(一半夹角)处的点

    int plLen = polyline.size();
    if (plLen < 2)
    {
        return false; // 折线至少需要两个点
    }

    // 遍历折线的每个点，计算单边平行线
    for (int i = 0; i < plLen; ++i)
    {
        if (i == 0)
        {
            // 如果是折线的起点
            double x1 = polyline[i].x(), y1 = polyline[i].y();
            double x2 = polyline[i + 1].x(), y2 = polyline[i + 1].y();

            auto [vx, vy] = normalize(y1 - y2, x2 - x1);// （dy, -dx） = p1 - p2 方向是指向p1

            parallelPolyline.append(QPointF(x1 +  vx * dis, y1 +  vy * dis));
        }
        else if (i == plLen - 1)
        {
            // 如果是折线的终点
            double x1 = polyline[i - 1].x(), y1 = polyline[i - 1].y();
            double x2 = polyline[i].x(), y2 = polyline[i].y();

            auto [vx, vy] = normalize(y1 - y2, x2 - x1);

            parallelPolyline.append(QPointF(x2 +  vx * dis, y2 +  vy * dis)); 
        }
        else
        {
            // 对于折线的中间点
            double x0 = polyline[i].x(), y0 = polyline[i].y();
            double x1 = polyline[i - 1].x(), y1 = polyline[i - 1].y();
            double x2 = polyline[i + 1].x(), y2 = polyline[i + 1].y();

            // 计算前后两段向量的单位向量
            auto [x01, y01] = normalize(x1 - x0, y1 - y0);
            auto [x02, y02] = normalize(x2 - x0, y2 - y0);

            if (sgn(cross(x01, y01, x02, y02)) == 0)
            {
                continue;  // 如果共线，跳过该点
            }

            // 计算角平分线的单位向量（两个向量的平均）
            auto [vx, vy] = normalize((x01 + x02) / 2, (y01 + y02) / 2);

            // 计算角平分线的长度，用于确定平行线的偏移量
            double sinX = std::fabs(cross(vx, vy, x02, y02));
            double disBisector = dis / sinX;  // 使用叉乘来确定夹角的大小，得出平行线的距离

            // 选择左侧或右侧的平行线
            if (cross(x1 - x0, y1 - y0, x2 - x0, y2 - y0) > 0)
            {
                parallelPolyline.append(QPointF(x0 - vx * disBisector, y0 - vy * disBisector));   // 向左偏移(如果dis是负数就向右偏移了)
            }
            else
            {
                parallelPolyline.append(QPointF(x0 + vx * disBisector, y0 + vy * disBisector));   // 向左偏移
            }
        }
    }
    return true;
}

 /**
  * 计算点到直线的垂直距离，并返回带方向的结果
  * @param point 点
  * @param lineStart 直线的起点
  * @param lineEnd 直线的终点
  * @param direction 返回值，用于表示点在直线的哪一侧，正值为左侧，负值为右侧，0为直线上
  * @return 点到直线的垂直距离
  */
double pointToLineDistanceWithDirection(const QPointF& point, const QPointF& lineStart, const QPointF& lineEnd)
{
    // 计算直线的方程 Ax + By + C = 0 的系数 A, B, C
    double A = lineEnd.y() - lineStart.y();  // y2 - y1
    double B = lineStart.x() - lineEnd.x();  // x1 - x2
    double C = lineEnd.x() * lineStart.y() - lineStart.x() * lineEnd.y();  // x2 * y1 - x1 * y2

    // 计算点到直线的垂直距离
    double distance = std::fabs(A * point.x() + B * point.y() + C) / std::sqrt(A * A + B * B);

    // 通过叉积判断点在直线的哪一侧
    // 使用点到直线起点的向量和直线的方向向量计算叉积
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
 * 计算平行折线，且平行线的第一条线段经过指定的点
 * @param polyline 输入折线的点列表
 * @param targetPoint 指定目标点，平行线将通过该点
 * @param parallelPolyline 输出参数，保存经过目标点的平行线
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateParallelLineThroughPoint(const Line& polyline, const QPointF& targetPoint, Line& parallelPolyline)
{
    // 原理：就是计算点到最后一条线段的距离,然后按这个距离画单边平行线
    int plLen = polyline.size();
    if (plLen < 2)
    {
        return false; // 折线至少需要两个点
    }
    // 计算点到第一条线段的垂直距离
    // 直线方向统一从后面的点指向前面的点
    double distance = pointToLineDistanceWithDirection(targetPoint,polyline[plLen - 1],polyline[plLen - 2]);

    // 按照计算的距离生成平行线
    return calculateParallelLine(polyline, distance, parallelPolyline);
}

// ==========================================================================================
// 缓冲区计算(基于矢量的缓冲区分析算法：平行线、角平分线、凸圆弧角,融合path)
// ==========================================================================================

/**
 * 根据起点、终点和圆心计算特点方向的圆弧上的点(顺时针方向，从左往右)
 * @param startPoint 圆弧的起点
 * @param endPoint 圆弧的终点
 * @param center 圆心
 * @param steps 步数，决定计算多少个点
 * @param clockwise 是否顺时针方向绘制圆弧，true 表示顺时针，false 表示逆时针
 * @param arcPoints 输出参数，保存计算得到的弧线上的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateArcPointsFromStartEndCenter(const QPointF& startPoint, const QPointF& endPoint, const QPointF& center,
    int steps, Line& arcPoints)
{
    // 计算起点和终点的角度
    double startAngle = std::atan2(startPoint.y() - center.y(), startPoint.x() - center.x());
    double endAngle = std::atan2(endPoint.y() - center.y(), endPoint.x() - center.x());

    startAngle = normalizeAngle(startAngle);
    endAngle = normalizeAngle(endAngle);

    // 计算角度差（弧度差）
    double angleDiff = normalizeAngle(endAngle - startAngle);


        
    // 计算圆弧上的点
    return calculateArcPoints(center, std::sqrt((center.x() - startPoint.x()) * (center.x() - startPoint.x())
        + (center.y() - startPoint.y()) * (center.y() - startPoint.y())), startAngle, angleDiff, steps, arcPoints);
}

// 计算线段长度是否满足条件，如果不满足则返回true表示需要打断
bool isLengthEnough(double x1, double y1, double x2, double y2, double r) {
    
    auto computeHalfAngleTan = [](double x1, double y1, double x2, double y2) {
        // 计算向量长度
        double len1 = length(x1, y1);
        double len2 = length(x2, y2);

        // 计算点积和叉积
        double dotProduct = dot(x1, y1, x2, y2);
        double crossProduct = std::fabs(cross(x1, y1, x2, y2));

        // 计算 sin 和 cos
        double cosTheta = dotProduct / (len1 * len2);
        double sinTheta = crossProduct / (len1 * len2);

        // 返回 tan(θ / 2)
        return sinTheta / (1 + cosTheta);
    };
    double needLen = r / computeHalfAngleTan(x1, y1, x2, y2);
    // 计算线段长度
    double len1 = length(x1, y1);
    double len2 = length(x2, y2);
    return len1 >= needLen && len2 >= needLen;
}

/**
 * 计算折线的缓存区
 * @param polyline 输入折线的点列表
 * @param dis 平行线与折线的距离
 * @param points 输出参数
 * @return 如果计算成功则返回 true，失败则返回 false
 */
// 参考：https://zhuanlan.zhihu.com/p/539904045
bool calculateLineBuffer(const Line& polyline, double dis, Line& points)
{
    // 思路：做折线的平行线，大于PI的角做圆弧处理，先从左往右画

    int plLen = polyline.size();
    if (plLen < 2)
    {
        return false; // 折线至少需要两个点
    }

    // 遍历折线的每个点
    for (int i = 0; i < plLen; ++i)
    {
        if (i == 0)
        {
            // 如果是折线的起点
            double x1 = polyline[i].x(), y1 = polyline[i].y();
            double x2 = polyline[i + 1].x(), y2 = polyline[i + 1].y();

            auto [vx, vy] = normalize(y1 - y2, x2 - x1);// （dy, -dx） = p1 - p2 方向是指向p1

            calculateArcPointsFromStartEndCenter(QPointF(x1 + vx * dis, y1 + vy * dis), QPointF(x1 - vx * dis, y1 - vy * dis), polyline[i], 20, points);
        }
        else if (i == plLen - 1)
        {
            // 如果是折线的终点
            double x1 = polyline[i - 1].x(), y1 = polyline[i - 1].y();
            double x2 = polyline[i].x(), y2 = polyline[i].y();

            auto [vx, vy] = normalize(y1 - y2, x2 - x1);

            calculateArcPointsFromStartEndCenter( QPointF(x2 - vx * dis, y2 - vy * dis), QPointF(x2 + vx * dis, y2 + vy * dis), polyline[i], 20, points);
        }
        else
        {
            // 对于折线的中间点
            double x0 = polyline[i].x(), y0 = polyline[i].y();
            double x1 = polyline[i - 1].x(), y1 = polyline[i - 1].y();
            double x2 = polyline[i + 1].x(), y2 = polyline[i + 1].y();

            // 计算前后两段向量的单位向量
            auto [x01, y01] = normalize(x1 - x0, y1 - y0);
            auto [x02, y02] = normalize(x2 - x0, y2 - y0);

            if (sgn(cross(x01, y01, x02, y02)) == 0)
            {
                // 如果前后线段的方向向量共线,直接用法向量加入点
                auto [vx, vy] = normalize(y01, -x01);// （dy, -dx） = p1 - p2 方向是指向p1
                points.append(QPointF(x0 - vx * dis, y0 - vy * dis));
                continue;
            }

            // 计算角平分线的单位向量（两个向量的平均）
            auto [vx, vy] = normalize((x01 + x02) / 2, (y01 + y02) / 2);

            // 计算角平分线的长度，用于确定平行线的偏移量
            double sinX = std::fabs(cross(vx, vy, x02, y02));
            double disBisector = dis / sinX;  // 使用叉乘来确定夹角的大小，得出平行线的距离

            // 选择左侧或右侧的平行线
            if (cross(x1 - x0, y1 - y0, x2 - x0, y2 - y0) > 0) // p2在左侧，直线方向是指向p1的，在画右边（角度大于PI）
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

    // 遍历折线的每个点(反方向)
    for (int i = plLen - 2; i > 0; --i)
    {
        // 对于折线的中间点
        double x0 = polyline[i].x(), y0 = polyline[i].y();
        double x1 = polyline[i + 1].x(), y1 = polyline[i + 1].y();
        double x2 = polyline[i - 1].x(), y2 = polyline[i - 1].y();

        // 计算前后两段向量的单位向量
        auto [x01, y01] = normalize(x1 - x0, y1 - y0);
        auto [x02, y02] = normalize(x2 - x0, y2 - y0);

        if (sgn(cross(x01, y01, x02, y02)) == 0)
        {
            // 如果前后线段的方向向量共线,直接用法向量加入点
            auto [vx, vy] = normalize(y01, -x01);// （dy, -dx） = p1 - p2 方向是指向p1
            continue;
        }

        // 计算角平分线的单位向量（两个向量的平均）
        auto [vx, vy] = normalize((x01 + x02) / 2, (y01 + y02) / 2);

        // 计算角平分线的长度，用于确定平行线的偏移量
        double sinX = std::fabs(cross(vx, vy, x02, y02));
        double disBisector = dis / sinX;  // 使用叉乘来确定夹角的大小，得出平行线的距离

        // 选择左侧或右侧的平行线
        if (cross(x1 - x0, y1 - y0, x2 - x0, y2 - y0) > 0) // p2在左侧，直线方向是指向p1的，在画右边（角度大于PI）
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

// 折线的缓存区,闭合版本
bool calculateClosedLineBuffer(const Line& polyline, double dis, Polygon& lines)
{
    // 思路：处理闭合折线，生成缓冲区，包括角平分线和圆弧的计算
    int plLen = polyline.size() - 1;
    if (plLen < 3)
    {
        return false; // 闭合折线至少需要三个点
    }

    Line currentLine;

    // 遍历折线的每个点（闭合处理，首尾相连）
    for (int i = 0; i < plLen; ++i)
    {
        // 获取当前点、前一点、后一点
        double x0 = polyline[i].x(), y0 = polyline[i].y();
        double x1 = polyline[(i - 1 + plLen) % plLen].x(), y1 = polyline[(i - 1 + plLen) % plLen].y();
        double x2 = polyline[(i + 1) % plLen].x(), y2 = polyline[(i + 1) % plLen].y();


        // 计算前后两段向量的单位向量
        auto [x01, y01] = normalize(x1 - x0, y1 - y0);
        auto [x02, y02] = normalize(x2 - x0, y2 - y0);

        if (sgn(cross(x01, y01, x02, y02)) == 0)
        {
            // 如果前后线段的方向向量共线,直接用法向量加入点
            auto [vx, vy] = normalize(y01, -x01);// （dy, -dx） = p1 - p2 方向是指向p1
            currentLine.append(QPointF(x0 - vx * dis, y0 - vy * dis));
            continue;
        }

        // 计算角平分线的单位向量（两个向量的平均）
        auto [vx, vy] = normalize((x01 + x02) / 2, (y01 + y02) / 2);

        // 计算角平分线的长度，用于确定平行线的偏移量
        double sinX = std::fabs(cross(vx, vy, x02, y02));
        double disBisector = dis / sinX;  // 使用叉乘来确定夹角的大小，得出平行线的距离

        // 选择左侧或右侧的平行线
        if (cross(x1 - x0, y1 - y0, x2 - x0, y2 - y0) > 0) // p2在左侧，直线方向是指向p1的，在画右边（角度大于PI）
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
    // 闭合当前线条，并添加到结果中
    if (!currentLine.isEmpty())
    {
        currentLine.append(currentLine[0]);
        lines.append(currentLine);
    }

    currentLine.clear();
    // 遍历折线的每个点(反方向)
    for (int i = plLen -1; i >= 0; --i)
    {
        // 获取当前点、前一点、后一点
        double x0 = polyline[i].x(), y0 = polyline[i].y();
        double x1 = polyline[(i + 1) % plLen].x(), y1 = polyline[(i + 1) % plLen].y();
        double x2 = polyline[(i - 1 + plLen) % plLen].x(), y2 = polyline[(i - 1 + plLen) % plLen].y();

        // 计算前后两段向量的单位向量
        auto [x01, y01] = normalize(x1 - x0, y1 - y0);
        auto [x02, y02] = normalize(x2 - x0, y2 - y0);

        if (sgn(cross(x01, y01, x02, y02)) == 0)
        {
            // 如果前后线段的方向向量共线,直接用法向量加入点
            auto [vx, vy] = normalize(y01, -x01);// （dy, -dx） = p1 - p2 方向是指向p1
            continue;
        }

        // 计算角平分线的单位向量（两个向量的平均）
        auto [vx, vy] = normalize((x01 + x02) / 2, (y01 + y02) / 2);

        // 计算角平分线的长度，用于确定平行线的偏移量
        double sinX = std::fabs(cross(vx, vy, x02, y02));
        double disBisector = dis / sinX;  // 使用叉乘来确定夹角的大小，得出平行线的距离

        // 选择左侧或右侧的平行线
        if (cross(x1 - x0, y1 - y0, x2 - x0, y2 - y0) > 0) // p2在左侧，直线方向是指向p1的，在画右边（角度大于PI）
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
    // 闭合当前线条，并添加到结果中
    if (!currentLine.isEmpty())
    {
        currentLine.append(currentLine[0]);
        lines.append(currentLine);
    }


    return true;
}


// 计算夹角处缓冲区
void computeAngleBufferIntersection(const QVector2D& v1, const QVector2D& v2, double r, QPointF o, Line& joinedPath)
{
    Line p1, p2;
    // 计算单位向量和法向量
    QVector2D u1 = v1.normalized();
    QVector2D u2 = v2.normalized();
    QVector2D normal1(-u1.y(), u1.x());
    QVector2D normal2(-u2.y(), u2.x());
    // 计算 p1 的圆弧和起终点
    QVector2D arcStart1 = v1 - normal1 * r;
    QVector2D arcEnd1 = v1 + normal1 * r;
    QVector2D end1 = normal1 * r;
    calculateArcPointsFromStartEndCenter(arcStart1.toPointF(), arcEnd1.toPointF(), v1.toPointF(), 20, p1);
    p1.push_back(end1.toPointF());
    // 计算 p2 的圆弧和起终点
    QVector2D arcStart2 = v2 + normal2 * r;
    QVector2D arcEnd2 = v2 - normal2 * r;
    QVector2D end2 = -normal2 * r;
    p2.push_back(end2.toPointF());
    calculateArcPointsFromStartEndCenter(arcEnd2.toPointF(), arcStart2.toPointF(), v2.toPointF(), 20, p2);
    // 寻找交点
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

    // 如果找到交点，截断并连接路径
    if (p1IntersectIndex != -1 && p2IntersectIndex != -1)
    {
        // 将 p1 截断到交点
        for (int i = 0; i <= p1IntersectIndex; ++i)
        {
            joinedPath.push_back(p1[i] + o);
        }
        joinedPath.push_back(intersection + o);

        // 将 p2 从交点开始添加
        for (int j = p2IntersectIndex + 1; j < p2.size(); ++j)
        {
            joinedPath.push_back(p2[j] + o);
        }
    }
}


// 计算夹角小的两段线缓存区
bool calculateLittleLineBuffer(const Line& polyline, double dis, Line& points)
{
    // 思路：做折线的平行线，大于PI的角做圆弧处理，先从左往右画

    int plLen = polyline.size();
    if (plLen != 3)
    {
        return false;
    }

    // 共线,交给其他函数
    if (pointRelativeToVector(polyline[0], polyline[1], polyline[2]) == 0)
    {
        calculateLineBuffer(polyline, dis, points);
        return true;
    }

    auto draw = [&](QPointF point1, QPointF point2, QPointF point3)
    {
        // 对于折线的中间点
        float x0 = point2.x(), y0 = point2.y();
        float x1 = point1.x(), y1 = point1.y();
        float x2 = point3.x(), y2 = point3.y();

        // 计算前后两段向量的单位向量
        auto [x01, y01] = normalize(x1 - x0, y1 - y0);
        auto [x02, y02] = normalize(x2 - x0, y2 - y0);

        // 选择左侧或右侧的平行线
        if (cross(x1 - x0, y1 - y0, x2 - x0, y2 - y0) > 0) // p2在左侧，直线方向是指向p1的，在画右边（角度大于PI）
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

// 线段结构体
struct Segment
{
    QPointF start, end;

    // 确保线段的起点小于终点（按 x 排序）
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

    double slope; // 更新起点，不重复计算
};


void sweepLineFindIntersections(const Polygon& pointss, Line& intersections, bool isArea = true) {

    // 事件类型
    enum EventType { Start, End, Intersection };

    struct Event {
        QPointF point;                          // 当前事件的坐标
        int segmentIndex;                       // 所属线段的索引
        int intersecIndex;                      // 相交线段的索引
        EventType type;                         // 事件类型

        // 重载比较运算符（用于优先队列）
        bool operator>(const Event& other) const {
            if (point.x() == other.point.x())
                return point.y() > other.point.y();
            return point.x() > other.point.x();
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
        for (int i = 0; i < points.size() - 1; ++i) {
            // 创建线段
            Segment segment(points[i], points[i + 1]);
            segments.push_back(segment);
            cnt++;


            // 生成起点和终点事件
            Event startEvent{
                segment.start,                                  // 起点坐标
                cnt - 1,                                        // 线段索引
                -1,                                             // 交点索引（起点没有交点）
                Start                                           // 事件类型
            };

            Event endEvent{
                segment.end,                                    // 终点坐标
                cnt - 1,                                        // 线段索引
                -1,                                             // 交点索引（终点没有交点）
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
    struct CustomComparator {
        bool operator()(const std::pair<QPointF, double>& a, const std::pair<QPointF, double>& b) const {
            // 确定比较的 x 值（较大值）
            double x_target = std::max(a.first.x(), b.first.x());

            // 计算 a 和 b 在 x = x_target 处的 y 值
            double y_a = a.first.y() + a.second * (x_target - a.first.x()); // y = y0 + k * (x - x0)
            double y_b = b.first.y() + b.second * (x_target - b.first.x());

            // 比较 y 值
            if (y_a != y_b) {
                return y_a < y_b;
            }

            // 如果相同，按斜率进行比较
            return a.second < b.second;
        }
    };
    std::multimap<std::pair<QPointF, double>, int, CustomComparator >statusTree;

    auto isAdjacentSegments = [&](int segIndex1, int segIndex2) {

        if (segIndex2 == segIndex1)return true;
        if (segments[segIndex2].start == segments[segIndex1].start)return true;



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

    // 一个交点算一次就行，太容易死锁了
    std::unordered_set<QPointF, QPointFHash, QPointFEqual> recordedIntersections;

    auto addIntersectionEvent = [&](int seg1, int seg2)
    {
        if (isAdjacentSegments(seg1, seg2))
        {
            return; // 如果两段相邻，忽略计算交点
        }
        QPointF intersecPoint;
        if (segmentsIntersect(segments[seg1].start, segments[seg1].end, segments[seg2].start, segments[seg2].end, intersecPoint)) 
        {
            // 如果已经有交点
            if (recordedIntersections.count(intersecPoint))return;
            else recordedIntersections.insert(intersecPoint);

            // 如果通过所有判断，记录交点和事件
            intersections.push_back(intersecPoint);
            events.push({ intersecPoint, seg1, seg2, Intersection });
        }
    };

    while (!events.empty()) {
        Event event = events.top();
        events.pop();

        int segmentIdx = event.segmentIndex;

        if (event.type == Start) {
            // 计算线段的 key 值
            std::pair<QPointF, double> key = { segments[segmentIdx].start, segments[segmentIdx].slope };

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
            std::pair<QPointF, double> key = { segments[segmentIdx].start, segments[segmentIdx].slope };

            // 查找要移除的线段
            auto it = statusTree.find(key);
            if (it != statusTree.end()) {
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
            int seg1 = event.segmentIndex;
            int seg2 = event.intersecIndex;

            // 计算线段的 key 值
            std::pair<QPointF, double> key1 = { segments[seg1].start, segments[seg1].slope };
            std::pair<QPointF, double> key2 = { segments[seg2].start, segments[seg2].slope };

            // 删除原有线段
            // 使用 find 方法确认是否在状态树中存在相同 key 对应的段
            auto range1 = statusTree.equal_range(key1);
            // 删除特定的线段
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

            // 到交点了，继续插入检查
            // 把开始加入event
            events.push({
                segments[seg1].start,                                   // 起点坐标
                seg1,                                                   // 线段索引
                -1,                                                     // 交点索引（起点没有交点）
                Start                                                   // 事件类型
                });
            events.push({
                segments[seg2].start,                                   // 起点坐标
                seg2,                                                   // 线段索引
                -1,                                                     // 交点索引（起点没有交点）
                Start                                                   // 事件类型
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
    // 遍历每个多边形的每条边
    for (int polygonIndex = 0; polygonIndex < polygons.size(); ++polygonIndex)
    {
        const auto& polygon = polygons[polygonIndex];
        int n = polygon.size();
        splitLines.push_back({});
        belong.push_back(polygonIndex); // 初始线段属于当前多边形

        for (int i = 0; i < n - 1; ++i)
        {
            const QPointF& start = polygon[i];
            const QPointF& end = polygon[i + 1];

            // 找到当前边上的交点
            Line pointsOnSegment;
            for (const QPointF& intersection : intersectionPoints)
            {
                if (pointOnSegment(intersection, start, end))
                {
                    pointsOnSegment.append(intersection);
                }
            }

            splitLines.last().append(start); // 左闭右开

            if (pointsOnSegment.isEmpty())
            {
                continue;
            }

            // 按离起点的距离对交点排序
            std::sort(pointsOnSegment.begin(), pointsOnSegment.end(),
                [&start](const QPointF& p1, const QPointF& p2)
                {
                    return QLineF(start, p1).length() < QLineF(start, p2).length();
                });

            // 根据排序后的点生成线段
            for (int j = 0; j < pointsOnSegment.size(); ++j)
            {
                splitLines.last().append(pointsOnSegment[j]); // 交点是段的起点和终点

                // 新增一条线段并记录所属图形
                splitLines.push_back({ pointsOnSegment[j] });
                belong.push_back(polygonIndex); // 新增线段也属于当前多边形
            }
        }
        splitLines.last().append(polygon.last()); // 添加多边形最后一个点
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

    // 调用实际的分割函数进行处理
    QVector<int> tempBelong;
    splitLineByIntersections(polygons, intersectionPoints, splitLines, tempBelong);

    // 根据 tempBelong 中的索引和 polygonIndices，填充最终的 belong
    for (int i = 0; i < tempBelong.size(); ++i)
    {
        belong.push_back(belongPolygon[tempBelong[i]]);
    }
}

void reconstructPolygons(const Polygon& splitLines,
    Polygon& mergedPolygons)
{
    // 建立表：每个交点的出向边和入向边
    std::unordered_map<QPointF, std::pair<std::vector<int>, std::vector<int>>, QPointFHash, QPointFEqual> table;

    // 填充表：记录每个点的出向边和入向边
    for (int i = 0; i < splitLines.size(); i++)
    {
        const auto& line = splitLines[i];
        table[line.first()].first.push_back(i); // 起点 -> 出向边
        table[line.last()].second.push_back(i); // 终点 -> 入向边
    }

    // 记录哪些线段已经被使用
    std::vector<bool> used(splitLines.size(), false);

    // 遍历所有线段并重建多边形
    for (int i = 0; i < splitLines.size(); i++)
    {
        if (used[i])
            continue; // 跳过已经使用的线段

        Line polygon;
        int currentLineIndex = i;

        // 构建多边形
        while (true)
        {
            // 获取当前线段
            const auto& line = splitLines[currentLineIndex];

            for (auto& point : line) polygon.append(point);

            used[currentLineIndex] = true; // 标记当前线段已使用

            // 查找当前线段终点的出向边
            const QPointF& endPoint = line.last();
            auto& outEdges = table[endPoint].first;

            // 找到未被使用的出向边
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

            currentLineIndex = nextLineIndex; // 移动到下一条线段
        }

        // 添加到结果中
        if (polygon.size() > 2) // 至少三个点才能构成多边形
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
        int polygonIndex = belong[i]; // 获取当前线段所属多边形的索引

        QPointF midPoint;
        // 计算线上上的一点
        if (line.size() > 2)
        {
            midPoint = line[line.size() / 2];
        }
        else midPoint = (line[0] + line[1]) * 0.5;

        bool isInsideOtherPolygon = false;

        // 检查中点是否在其他闭合多边形的内部
        for (int j = 0; j < polygons.size(); ++j)
        {
            if (j == polygonIndex)
            {
                continue; // 跳过当前线段所属的多边形
            }

            if (isPointInsidePolygon(midPoint, polygons[j]))
            {
                isInsideOtherPolygon = true;
                break;
            }
        }

        // 如果中点不在其他多边形内部，保留该线段
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
    Line tempSegment;           // 临时段

    auto computeHalfAngleTan = [](double x1, double y1, double x2, double y2) {
        // 计算向量模长
        double len1 = std::sqrt(x1 * x1 + y1 * y1);
        double len2 = std::sqrt(x2 * x2 + y2 * y2);

        // 计算点积和叉积
        double dotProduct = x1 * x2 + y1 * y2;          // 点积
        double crossProduct = x1 * y2 - y1 * x2;        // 叉积

        // 计算 sin 和 cos
        double cosTheta = dotProduct / (len1 * len2);
        double sinTheta = std::fabs(crossProduct) / (len1 * len2);

        // 计算 tan(θ / 2)
        return sinTheta / (1 + cosTheta);
    };

    for (int i = 1; i < points.size() - 1; ++i) {
        double x0 = points[i].x(), y0 = points[i].y();
        double x1 = points[i + 1].x(), y1 = points[i + 1].y();
        double x2 = points[i - 1].x(), y2 = points[i - 1].y();

        // 计算前后两段向量
        double x01 = x1 - x0, y01 = y1 - y0;
        double x02 = x2 - x0, y02 = y2 - y0;

        double needLen = r / computeHalfAngleTan(x01, y01, x02, y02);

        // 判断是否需要打断
        if (std::sqrt(x01 * x01 + y01 * y01) < needLen || std::sqrt(x02 * x02 + y02 * y02) < needLen) 
        {
            vis[i] = true; // 需要打断
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
    // 遍历每个多边形的每条边
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

            // 找到当前边上的交点
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

            splitLines.last().append(start); // 左闭右开

            if (pointsOnSegment.isEmpty())
            {
                continue;
            }

            // 按离起点的距离对交点排序
            std::sort(pointsOnSegment.begin(), pointsOnSegment.end(),
                [&start](const QPointF& p1, const QPointF& p2)
                {
                    return QLineF(start, p1).length() < QLineF(start, p2).length();
                });

            // 根据排序后的点生成线段
            for (int j = 0; j < pointsOnSegment.size(); ++j)
            {
                QPointF midPoint = (pointsOnSegment[j] + splitLines.last().last()) / 2;
                splitLines.last().append(midPoint);

                if (QPointFEqual()(midPoint, pointsOnSegment[j]))continue;

                splitLines.push_back({ midPoint, pointsOnSegment[j] });
            }
        }

        splitLines.last().append(polygon.last()); // 添加多边形最后一个点
    }
}


bool computeBufferBoundaryWithVector(const Polygon& pointss, double r, Polygon& boundaryPointss)
{
    // 自相交在前面在生成缓冲区前判断
    // Step 0: 处理自相交线,夹角小的线的打断分割

    // 三维,点组成线,多线组成面（正负缓冲区），多面
    Polygons polygons;

    for (auto& points : pointss)
    {
        Polygon lines;

        //因为长度不够而打断
        breakLineOnLen(points, r, lines);

        // 使用扫描线算法找到交点
        Line intersectionPoints; // 存储所有交点
        sweepLineFindIntersections(lines, intersectionPoints, false); // 自定义扫描线算法函数

        Polygon splitLines;

        breakLineOnIntersections(lines, intersectionPoints, splitLines); // 分割线段


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


    // 自己的计算很多错误，用path
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
                boundaryPointss.push_back({}); // 开始新的点集
            }
            boundaryPointss.last().append(QPointF(combinedPath.elementAt(i).x, combinedPath.elementAt(i).y));
        }
        return true;
    }


    // Step 1: 使用扫描线算法找到交点
    Line intersectionPoints; // 存储所有交点
    sweepLineFindIntersections(polygons, intersectionPoints); // 自定义扫描线算法函数

    qDebug() << L("交点数：%1").arg(intersectionPoints.size());

    // Step 2: 根据交点分割线段
    Polygon splitLines;
    QVector<int> belong;
    splitLineByIntersections(polygons, intersectionPoints, splitLines, belong); // 分割线段  

    qDebug() << L("线段数：%1").arg(splitLines.size());

    // Step 3: 过滤位于多边形内部的线段
    Polygon filteredSplitLines;
    filterSplitLinesInsidePolygons(splitLines, belong, polygons, filteredSplitLines); // 点集 pointss 表示原始多边形

    //boundaryPointss = filteredSplitLines;
    //return true;

    qDebug() << L("过滤后线段数：%1").arg(filteredSplitLines.size());

    // Step 4: 重建拓扑结构并合并多边形
    reconstructPolygons(filteredSplitLines, boundaryPointss);

    qDebug() << L("闭合曲线数：%1").arg(boundaryPointss.size());

    for (int i = 0; i < boundaryPointss.size(); ++i)
    {
        qDebug() << boundaryPointss[i];
    }

    return true;
}


// ==========================================================================================
// 缓冲区计算(基于栅格的缓冲区分析算法)
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
        // 遍历所有点，更新边界
        for (const QPointF& point : points)
        {
            if (point.x() < minX) minX = point.x();
            if (point.x() > maxX) maxX = point.x();
            if (point.y() < minY) minY = point.y();
            if (point.y() > maxY) maxY = point.y();
        }
    }
    // 返回上下左右边界
    return QRectF(QPointF(minX, minY), QPointF(maxX, maxY));
}

void getGridMap(const Polygon& pointss, double r, GridMap& gridMap)
{
    // 计算点集合的上下左右边界
    QRectF bounds = calculateBounds(pointss);

    // 考虑时间复杂度，搜完格子要控制在 1s 级别，网格数1e8；
    // 计算缩放比例，确保网格点总数控制在 1e6 以内
    double area = (bounds.width() + 2 * r) * (bounds.height() + 2 * r);

    int pointNum = 0;
    for (auto& points : pointss)pointNum += points.size();

    gridMap.scale = std::sqrt(area*pointNum / 10000000.0);  // 缩放比例

    // 算法复杂度：area*pointNum*scale（控制在秒级别）

    int k = std::round(r / gridMap.scale);

    // 偏移量，用于让网格坐标从 (0, 0) 开始
    gridMap.offset = QPointF(bounds.left() - (k + 5) * gridMap.scale, bounds.top() - (k + 5) * gridMap.scale);

    // 计算网格尺寸，增加一定缓冲区域
    gridMap.sizeX = static_cast<int>(bounds.width() / gridMap.scale + 2 * k + 10);
    gridMap.sizeY = static_cast<int>(bounds.height() / gridMap.scale + 2 * k + 10);
}

void restoreFromGrid(const QVector<QVector<QPoint>>& gridPointss, const GridMap& gridMap, Polygon& pointss)
{
    // 遍历二维网格点集合
    for (const QVector<QPoint>& gridPoints : gridPointss)
    {
        pointss.push_back(Line());
        Line& points = pointss.last();
        for (const QPoint& gridPoint : gridPoints)
        {
            double x = gridPoint.x() * gridMap.scale + gridMap.offset.x();
            double y = gridPoint.y() * gridMap.scale + gridMap.offset.y();
            points.append(QPointF(x, y)); // 还原原始坐标点
        }
    }
}

// 判断点是否在所有折线的距离小于给定值
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
             
        // 遍历当前折线的每个线段
        for (int i = 0; i < polyline.size() - 1; ++i)
        {
            const QPointF& start = polyline[i];
            const QPointF& end = polyline[i + 1];

            // 计算当前点到该线段的垂直距离
            double d = pointToSegmentDistance(point, start, end);

             // 如果当前最小距离已经小于阈值，提前返回
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
    // 获取网格尺寸和初始点集合
    int sizeX = gridMap.sizeX, sizeY = gridMap.sizeY;


    // 初始化标记网格
    std::vector<std::vector<int>> mark(sizeX, std::vector<int>(sizeY, 0));

    int k = std::round(r / gridMap.scale);

    // 暴力枚举二维网格中的每个点
    for (int i = 0; i < sizeX; i++) {
        for (int j = 0; j < sizeY; j++) {
            // 判断是否在任何初始点的距离范围内
            double x = i * gridMap.scale + gridMap.offset.x();
            double y = j * gridMap.scale + gridMap.offset.y();
            if (isPointCloseToAnyPolyline({ x,y }, pointss, r))
            {
                mark[i][j] = 1;
            }
        }
    }

    QVector<QPoint> boundaryPoints;
    // 找到边界点
    const int dx[8] = { 0, 0, 1, -1 , 1, 1, -1, -1 };
    const int dy[8] = { -1, 1, 0, 0 , 1, -1, 1, -1 };

    for (int i = 0; i < sizeX; i++) {
        for (int j = 0; j < sizeY; j++) {
            if (mark[i][j] == 1) { // 如果点被标记为 1
                bool isBoundary = false;
                for (int z = 0; z < 8; z++)
                {
                    int x = i + dx[z], y = j + dy[z];
                    // 检查周围是否存在未标记的点
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

    // 清空mark,标记边界
    for (auto& row : mark)
    {
        std::fill(row.begin(), row.end(), 0);
    }
    for (auto& point : boundaryPoints)
    {
        mark[point.x()][point.y()] = true;
    }

    // =========================================================== dfs搜索，排序点
    // 因为都是闭合路径，且没有交点，dfs可以搜出
    // 初始化结果网格
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

            // 遍历 8 个方向
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
    if (points.size() < 3) return;  // 点数少于3个，无法简化

    // 获取起点和终点
    QPointF start = points.first();
    QPointF end = points.last();

    // 找到最大距离的点
    double maxDist = 0.0;
    int index = 0;
    for (int i = 1; i < points.size() - 1; ++i) {
        double dist = pointToSegmentDistance(points[i], start, end);
        if (dist > maxDist) {
            maxDist = dist;
            index = i;
        }
    }
    // 如果最大距离大于阈值epsilon，保留该点并递归简化两部分折线
    if (maxDist > epsilon) {
        // 递归左侧
        Line left(points.begin(), points.begin() + index + 1);
        douglasPeucker(left, epsilon);

        // 递归右侧
        Line right(points.begin() + index, points.end());
        douglasPeucker(right, epsilon);

        // 合并左右部分，去掉重复的端点
        points.clear();
        points.append(left);
        points.append(right.mid(1)); // 去掉右侧第一个点，因为它已经在左侧结束时存在
    }
    else {
        // 如果最大距离小于阈值epsilon，直接保留起点和终点，认为该部分已经简化到足够接近
        points.clear();
        points.append(start);  // 保留起点
        points.append(end);    // 保留终点
    }
}

void simpleLine(Line& points)
{
    int midIndex = points.size() / 2;

    // 将边界点集拆分为前后两半
    Line firstHalf(points.begin(), points.begin() + midIndex + 1);
    Line secondHalf(points.mid(midIndex));

    // 使用Douglas-Peucker简化两半点集
    douglasPeucker(firstHalf, 1);
    douglasPeucker(secondHalf, 1);

    // 清空当前集合，并将简化后的前后两半合并
    points.clear();
    points.append(firstHalf);
    points.append(secondHalf.mid(1));
}


// 计算缓冲区边界，使用栅格化算法
bool computeBufferBoundaryWithGrid(const Polygon& pointss, double r, Polygon& boundaryPointss)
{
    boundaryPointss.clear();

    // 如果半径非法，返回false
    if (r < 0)
        return false;

    GridMap gridMap;

    // 将二维点集合映射到网格
    getGridMap(pointss, r, gridMap);

    QVector<QVector<QPoint>> gridBoundaryPointss;

    // 使用暴力算法计算分界点
    markBoundaryPointsBruteForce(pointss, gridMap, r, gridBoundaryPointss);

    // 从网格恢复二维边界点集合
    restoreFromGrid(gridBoundaryPointss, gridMap, boundaryPointss);

    // 对每个边界点集合进行Douglas-Peucker简化，并应用B样条曲线

    // 误差在scale

    for (auto& points : boundaryPointss)
    {
        simpleLine(points);

        // 使用B样条曲线平滑简化后的边界点集
        Line smoothedPoints;
        if (points.size())
            points.push_back(points[0]); // 保证闭合

        calculateBSplineCurve(points, 3, points.size() * 100, smoothedPoints);

        //solveCubicSpline(points, smoothedPoints);

        // 替换原始点集为平滑后的点集
        points.swap(smoothedPoints);

        // 保证闭合
        if (points.size())
            points.push_back(points[0]);
    }

    return true;
}

bool computeBufferBoundary(BufferCalculationMode mode, const Polygon& pointss, double r, Polygon& boundaryPointss)
{
    boundaryPointss.clear();
    // 单独处理点
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
