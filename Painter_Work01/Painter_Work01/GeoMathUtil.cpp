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

// 比较器（给std容器用）
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

// 用于比较 QPointF 的哈希和相等
struct QPointFHash
{
    std::size_t operator()(const QPointF& point) const
    {
        return std::hash<double>()(point.x()) ^ std::hash<double>()(point.y());
    }
};

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
bool calculateLinePoints(NodeLineStyle lineStyle, const QVector<QPointF>& controlPoints, QVector<QPointF>& linePoints, int steps)
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
bool calculateCloseLinePoints(NodeLineStyle lineStyle, const QVector<QPointF>& controlPoints, QVector<QPointF>& linePoints, int steps)
{
    linePoints.clear();
    QVector<QPointF> newControlPoints;

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
int calculateLinePoints(const QVector<Component>& components, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps)
{
    linePointss.clear();  // 清空结果数组
    int startIdx = 0;
    int result = 0; // 多少条线绘制完成

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
 * 根据多个线段的组件和控制点计算闭合线上的点
 * @param component 组件列表
 * @param controlPoints 控制点列表
 * @param linePointss 输出参数，保存计算得到的闭合线上的点
 * @param steps 步数，决定计算多少个中间点
 * @return 返回成功绘制分图的个数
 */
int calculateCloseLinePoints(const QVector<Component>& components, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps)
{
    linePointss.clear();  // 清空结果数组
    int startIdx = 0;
    int result = 0; // 多少条线绘制完成

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
 * 根据多个线段的组件和控制点计算平行线（第二个分离一个点来控制距离）
 * @param component 组件列表
 * @param controlPoints 控制点列表
 * @param linePointss 输出参数，保存计算得到的闭合线上的点
 * @param steps 步数，决定计算多少个中间点
 * @return 返回成功绘制分图的个数
 */
int calculateParallelLinePoints(const QVector<Component>& components, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps)
{
    linePointss.clear();  // 清空结果数组
    int startIdx = 0;
    int result = 0; // 多少条线绘制完成

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
    
    
        if (components.size() == 2 && components[1].len) // 绘制平行线
        {
            QVector<QPointF> linePoints;
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
QPointF bezierN(const QVector<QPointF>& controlPoints, double t)
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
QVector<QPointF> calculateBezierCurve(const QVector<QPointF>& controlPoints, int numPoints)
{
    QVector<QPointF> curvePoints;

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
bool calculateBSplineCurve(const QVector<QPointF>& controlPoints, int degree, int numCurvePoints, QVector<QPointF>& curvePoints)
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
bool calculateArcPoints(const QPointF& center, double radius, double startAngle, double angleDiff, int steps, QVector<QPointF>& points)
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
 * 根据两个点计算圆上的点
 * @param point1 圆上的第一个点
 * @param point2 圆上的第二个点
 * @param steps 步数，决定计算多少个点
 * @param arcPoints 输出参数，保存计算得到的圆上的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateCirclePointsFromControlPoints(const QPointF& point1, const QPointF& point2, int steps, QVector<QPointF>& arcPoints)
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
bool calculateArcPointsFromControlPoints(const QVector<QPointF>& controlPoints, int steps, QVector<QPointF>& arcPoints)
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

// 精度处理
int sgn(double x) {
    if (x < -EPS) return -1;
    else if (x > EPS) return 1;
    else return 0;
}

// 向量标准化
std::pair<double, double> normalize(double x, double y) {
    double dis = std::sqrt(x * x + y * y);
    return { x / dis, y / dis };
}

// 向量叉乘
// 可以用右手螺旋法则来判断方向， 除大拇指外， 四指朝1方向向2方向转， 此时大拇指方向即是叉乘结果方向， 向上为正;正的话，2在1的左边
double cross(double x1, double y1, double x2, double y2) {
    return x1 * y2 - x2 * y1;
}

/**
 * 计算折线的双边平行线
 * @param polyline 输入折线的点列表
 * @param dis 平行线与折线的距离
 * @param leftPolyline 输出参数，保存平行线的左侧点
 * @param rightPolyline 输出参数，保存平行线的右侧点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateParallelLine(const QVector<QPointF>& polyline, double dis, QVector<QPointF>& leftPolyline, QVector<QPointF>& rightPolyline)
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
bool calculateParallelLine(const QVector<QPointF>& polyline, double dis, QVector<QPointF>& parallelPolyline)
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
bool calculateParallelLineThroughPoint(const QVector<QPointF>& polyline, const QPointF& targetPoint, QVector<QPointF>& parallelPolyline)
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
    int steps, QVector<QPointF>& arcPoints)
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


/**
 * 判断点与向量的位置关系
 * @param point 点
 * @param vectorStart 向量的起点
 * @param vectorEnd 向量的终点
 * @return 返回：

 */
int pointPositionRelativeToVector(const QPointF& point, const QPointF& vectorStart, const QPointF& vectorEnd)
{
    // 计算向量 AB 和 AP 的叉积
    // x1*y2 - y1*x2
    //可以用右手螺旋法则来判断方向， 除大拇指外， 四指朝1方向向2方向转， 此时大拇指方向即是叉乘结果方向， 向上为正
    double crossProduct = (point.x() - vectorStart.x()) * (vectorEnd.y() - vectorStart.y()) -
        (point.y() - vectorStart.y()) * (vectorEnd.x() - vectorStart.x());

    if (crossProduct > 0)
    {
        return 1;  // 点在向量的左侧
    }
    else if (crossProduct < 0)
    {
        return -1; // 点在向量的右侧
    }

    return 0;  // 点在向量的同一直线上
}

// 计算线段长度是否满足条件，如果不满足则返回true表示需要打断
bool isLengthEnough(double x1, double y1, double x2, double y2, double r) {
    
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
    double needLen = r / computeHalfAngleTan(x1, y1, x2, y2);
    // 计算线段长度
    double len1 = std::sqrt(x1 * x1 + y1 * y1);
    double len2 = std::sqrt(x2 * x2 + y2 * y2);
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
bool calculateLineBuffer(const QVector<QPointF>& polyline, double dis, QVector<QPointF>& points)
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
bool calculateClosedLineBuffer(const QVector<QPointF>& polyline, double dis, QVector<QVector<QPointF>>& lines)
{
    // 思路：处理闭合折线，生成缓冲区，包括角平分线和圆弧的计算
    int plLen = polyline.size() - 1;
    if (plLen < 3)
    {
        return false; // 闭合折线至少需要三个点
    }

    QVector<QPointF> currentLine;

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

void simpleLine(QVector<QPointF>& points)
{
    int midIndex = points.size() / 2;

    // 将边界点集拆分为前后两半
    QVector<QPointF> firstHalf(points.begin(), points.begin() + midIndex + 1);
    QVector<QPointF> secondHalf(points.mid(midIndex));

    // 使用Douglas-Peucker简化两半点集
    douglasPeucker(firstHalf, 1);
    douglasPeucker(secondHalf, 1);

    // 清空当前集合，并将简化后的前后两半合并
    points.clear();
    points.append(firstHalf);
    points.append(secondHalf.mid(1));
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

// 检查两条线段是否相交
bool doIntersect(const Segment& s1, const Segment& s2)
{
    auto orientation = [](const QPointF& p, const QPointF& q, const QPointF& r)
    {
        double val = (q.y() - p.y()) * (r.x() - q.x()) - (q.x() - p.x()) * (r.y() - q.y());
        if (std::abs(val) < EPSILON) return 0; // 共线（考虑误差）
        return (val > 0.0) ? 1 : 2; // 顺时针（右侧） or 逆时针（左侧）
    };

    int o1 = orientation(s1.start, s1.end, s2.start);
    int o2 = orientation(s1.start, s1.end, s2.end);
    int o3 = orientation(s2.start, s2.end, s1.start);
    int o4 = orientation(s2.start, s2.end, s1.end);

    // 检查通用相交条件（互相在线段两侧）
    if (o1 != o2 && o3 != o4)
    {
        return true;
    }

    // 检查共线情况（使用误差处理）
    auto onSegment = [](const QPointF& p, const QPointF& q, const QPointF& r)
    {
        return std::min(p.x(), r.x()) - EPSILON <= q.x() && q.x() <= std::max(p.x(), r.x()) + EPSILON &&
            std::min(p.y(), r.y()) - EPSILON <= q.y() && q.y() <= std::max(p.y(), r.y()) + EPSILON;
    };

    // 如果线段共线，检查它们是否重叠
    if (o1 == 0 && onSegment(s1.start, s2.start, s1.end)) return true;
    if (o2 == 0 && onSegment(s1.start, s2.end, s1.end)) return true;
    if (o3 == 0 && onSegment(s2.start, s1.start, s2.end)) return true;
    if (o4 == 0 && onSegment(s2.start, s1.end, s2.end)) return true;

    return false; // 没有相交
}

// 计算两条线段的交点
bool calculateIntersection(const Segment& s1, const Segment& s2, QPointF& point) {
    // 线段起点向量
    const QPointF& A = s1.start;
    const QPointF& B = s1.end;
    const QPointF& C = s2.start;
    const QPointF& D = s2.end;

    // 计算向量 AB 和 CD
    QPointF AB(B.x() - A.x(), B.y() - A.y());
    QPointF CD(D.x() - C.x(), D.y() - C.y());

    // 计算叉乘
    double cross1 = AB.x() * CD.y() - AB.y() * CD.x(); // AB 与 CD 的叉积

    // 精度控制，避免浮点数误差导致问题
    const double epsilon = 1e-10; // 自定义精度阈值
    if (std::fabs(cross1) < epsilon) {
        return false;
    }

    // 计算 AC 和 CD 的叉积
    QPointF AC(C.x() - A.x(), C.y() - A.y());
    double t = (AC.x() * CD.y() - AC.y() * CD.x()) / cross1;

    // 根据参数 t 计算交点
    double x = A.x() + t * AB.x();
    double y = A.y() + t * AB.y();

    point = { x,y };
    return true;
}


void sweepLineFindIntersections(const QVector<QVector<QPointF>>& pointss, QVector<QPointF>& intersections, bool isArea = true) {

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
        if (doIntersect(segments[seg1], segments[seg2])) {
            QPointF intersecPoint;
            if (!calculateIntersection(segments[seg1], segments[seg2], intersecPoint))return;

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

// 判断一个点是否在线段上
bool isPointOnSegment(const QPointF& p, const QPointF& start, const QPointF& end, double epsilon = 1e-6)
{
    // 检查点是否在线段的范围内
    if (p.x() < std::min(start.x(), end.x()) - epsilon || p.x() > std::max(start.x(), end.x()) + epsilon ||
        p.y() < std::min(start.y(), end.y()) - epsilon || p.y() > std::max(start.y(), end.y()) + epsilon)
    {
        return false;
    }

    // 检查是否共线
    QLineF line1(start, p);
    QLineF line2(p, end);
    return std::fabs(line1.length() + line2.length() - QLineF(start, end).length()) < epsilon;
}

void splitLineByIntersections(const QVector<QVector<QPointF>>& polygons,
    const QVector<QPointF>& intersectionPoints,
    QVector<QVector<QPointF>>& splitLines,
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
            QVector<QPointF> pointsOnSegment;
            for (const QPointF& intersection : intersectionPoints)
            {
                if (isPointOnSegment(intersection, start, end))
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

    // 调用实际的分割函数进行处理
    QVector<int> tempBelong;
    splitLineByIntersections(polygons, intersectionPoints, splitLines, tempBelong);

    // 根据 tempBelong 中的索引和 polygonIndices，填充最终的 belong
    for (int i = 0; i < tempBelong.size(); ++i)
    {
        belong.push_back(belongPolygon[tempBelong[i]]);
    }
}

void reconstructPolygons(const QVector<QVector<QPointF>>& splitLines,
    QVector<QVector<QPointF>>& mergedPolygons)
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

        QVector<QPointF> polygon;
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

bool isPointInsidePolygonDirection(const QPointF& point, const QVector<QPointF>& polygon, bool horizontal)
{
    int intersections = 0;
    int n = polygon.size();

    if (n < 3)
    {
        return false; // 非法多边形（少于3个点）
    }

    for (int i = 0; i < n; ++i)
    {
        const QPointF& p1 = polygon[i];
        const QPointF& p2 = polygon[(i + 1) % n];

        if (horizontal)
        {
            // 水平射线检测
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
            // 垂直射线检测
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

    // 总投票结果 > 0 表示内部，<= 0 表示外部
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

// 判断正负缓冲区（有向多边形面积公式）
bool isClockwise(const QVector<QPointF>& polygon) {
    double sum = 0.0;
    for (int i = 0; i < polygon.size() - 1; ++i) {
        const auto& p1 = polygon[i];
        const auto& p2 = polygon[i + 1];
        sum += (p2.x() - p1.x()) * (p2.y() + p1.y());
    }
    return sum > 0.0; // >0 表示顺时针，<0 表示逆时针
}

void breakLineOnLen(const QVector<QPointF>& points, double r, QVector<QVector<QPointF>>& segments)
{
    if (points.size() < 2) {
        return;
    }

    QVector<QPointF> tempSegment;
    tempSegment.push_back(points[0]);

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
        if (std::sqrt(x01 * x01 + y01 * y01) < needLen || std::sqrt(x02 * x02 + y02 * y02) < needLen) {
            // 添加当前段到结果集
            tempSegment.push_back(points[i]);
            segments.push_back(tempSegment);
            tempSegment.clear();
        }
        // 始终添加当前点到临时段
        tempSegment.push_back(points[i]);
    }

    // 添加最后一段
    tempSegment.push_back(points.last());
    if (!tempSegment.isEmpty()) {
        segments.push_back(tempSegment);
    }

    return;
}

bool computeBufferBoundaryWithVector(const QVector<QVector<QPointF>>& pointss, double r, QVector<QVector<QPointF>>& boundaryPointss)
{
    // 自相交在前面在生成缓冲区前判断
    // Step 0: 处理自相交线,夹角小的线的打断分割

    // 三维,点组成线,多线组成面（正负缓冲区），多面
    QVector<QVector<QVector<QPointF>>> polygons;

    for (auto& points : pointss)
    {
        QVector<QVector<QPointF>> lines;

        //因为长度不够而打断
        breakLineOnLen(points, r, lines);

        // 使用扫描线算法找到交点
        QVector<QPointF> intersectionPoints; // 存储所有交点
        sweepLineFindIntersections(lines, intersectionPoints, false); // 自定义扫描线算法函数

        QVector<QVector<QPointF>> splitLines;
        QVector<int> belong;

        splitLineByIntersections(lines, intersectionPoints, splitLines, belong); // 分割线段

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


    // 自己的计算很多错误，用path
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
    QVector<QPointF> intersectionPoints; // 存储所有交点
    sweepLineFindIntersections(polygons, intersectionPoints); // 自定义扫描线算法函数

    qDebug() << L("交点个数：%1").arg(intersectionPoints.size());

    // Step 2: 根据交点分割线段
    QVector<QVector<QPointF>> splitLines;
    QVector<int> belong;
    splitLineByIntersections(polygons, intersectionPoints, splitLines, belong); // 分割线段  

    qDebug() << L("线段个数：%1").arg(splitLines.size());

    //boundaryPointss = splitLines;
    //return true;

    // Step 3: 过滤位于多边形内部的线段
    QVector<QVector<QPointF>> filteredSplitLines;
    filterSplitLinesInsidePolygons(splitLines, belong, polygons, filteredSplitLines); // 点集 pointss 表示原始多边形

    //boundaryPointss = filteredSplitLines;
    //return true;

    // Step 4: 重建拓扑结构并合并多边形
    reconstructPolygons(filteredSplitLines, boundaryPointss);

    return true;
}






// ==========================================================================================
// 缓冲区计算(基于栅格的缓冲区分析算法)
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

void getGridMap(const QVector<QVector<QPointF>>& pointss, double r, GridMap& gridMap)
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

void restoreFromGrid(const QVector<QVector<QPoint>>& gridPointss, const GridMap& gridMap, QVector<QVector<QPointF>>& pointss)
{
    // 遍历二维网格点集合
    for (const QVector<QPoint>& gridPoints : gridPointss)
    {
        pointss.push_back(QVector<QPointF>());
        QVector<QPointF>& points = pointss.last();
        for (const QPoint& gridPoint : gridPoints)
        {
            double x = gridPoint.x() * gridMap.scale + gridMap.offset.x();
            double y = gridPoint.y() * gridMap.scale + gridMap.offset.y();
            points.append(QPointF(x, y)); // 还原原始坐标点
        }
    }
}

// n3超级暴力
// 计算欧几里得距离
int euclideanDistance2(int x1, int y1, int x2, int y2)
{
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

// 计算点到线段的垂直距离
double pointToSegmentDistance(const QPointF& point, const QPointF& start, const QPointF& end) {
    double x1 = start.x(), y1 = start.y();
    double x2 = end.x(), y2 = end.y();
    double x0 = point.x(), y0 = point.y();

    // 计算线段的长度
    double dx = x2 - x1;
    double dy = y2 - y1;
    double segmentLength = std::sqrt(dx * dx + dy * dy);

    // 如果线段的长度为零，返回点到起点的距离
    if (segmentLength == 0) {
        return std::sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));
    }

    // 计算投影点的比例t
    double t = ((x0 - x1) * dx + (y0 - y1) * dy) / (segmentLength * segmentLength);
    t = std::max(0.0, std::min(1.0, t));  // 限制t在0到1之间

    // 计算投影点的坐标
    double projection_x = x1 + t * dx;
    double projection_y = y1 + t * dy;

    // 计算并返回点到投影点的距离
    return std::sqrt((x0 - projection_x) * (x0 - projection_x) + (y0 - projection_y) * (y0 - projection_y));
}

// 判断点是否在所有折线的距离小于给定值
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

void markBoundaryPointsBruteForce(const QVector<QVector<QPointF>>& pointss,const GridMap& gridMap, double r, QVector<QVector<QPoint>>& boundaryPointss)
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

void douglasPeucker(QVector<QPointF>& points, double epsilon) {
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
        QVector<QPointF> left(points.begin(), points.begin() + index + 1);
        douglasPeucker(left, epsilon);

        // 递归右侧
        QVector<QPointF> right(points.begin() + index, points.end());
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

// 计算缓冲区边界，使用栅格化算法
bool computeBufferBoundaryWithGrid(const QVector<QVector<QPointF>>& pointss, double r, QVector<QVector<QPointF>>& boundaryPointss)
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
        int midIndex = points.size() / 2;

        // 将边界点集拆分为前后两半
        QVector<QPointF> firstHalf(points.begin(), points.begin() + midIndex + 1);
        QVector<QPointF> secondHalf(points.mid(midIndex));

        // 使用Douglas-Peucker简化两半点集
        douglasPeucker(firstHalf, gridMap.scale*2);
        douglasPeucker(secondHalf, gridMap.scale * 2);

        // 清空当前集合，并将简化后的前后两半合并
        points.clear();
        points.append(firstHalf);
        points.append(secondHalf.mid(1));

        // 使用B样条曲线平滑简化后的边界点集
        QVector<QPointF> smoothedPoints;
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

bool computeBufferBoundary(BufferCalculationMode mode, const QVector<QVector<QPointF>>& pointss, double r, QVector<QVector<QPointF>>& boundaryPointss)
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
