#include "GeoMathUtil.h"
#include <QVector>
#include <QPointF>
#include <cmath>

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
 * 规范化角度，确保角度在[0, 2π)范围内
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
 * 根据圆心、半径、起始角度、角度差值和步数计算弧线上的点
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

// ==========================================================================
// 缓冲区计算
// ==========================================================================

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


            //points.append(QPointF(x1 + vx * dis, y1 + vy * dis));  // 左
            calculateArcPointsFromStartEndCenter(QPointF(x1 + vx * dis, y1 + vy * dis), QPointF(x1 - vx * dis, y1 - vy * dis), polyline[i], 20, points);
            //points.append(QPointF(x1 - vx * dis, y1 - vy * dis));  // 向右偏移
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
                continue;  // 如果共线，跳过该点
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
            continue;  // 如果共线，跳过该点
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