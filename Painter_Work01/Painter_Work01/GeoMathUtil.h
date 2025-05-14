#pragma once

#include <QPointF>
#include <QVector>
#include "Enums.h"
#include <QRectF>
#include <QMetaType>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

// ==========================================================================
// 计算线段上的点
// 对外的接口， 统一分配
// ==========================================================================

/**
* 能代替尽量不失败绘制
* 除了统一的接口，其他的都不清理传出参数
*/

/**
 * 根据节点类型和控制点计算线上的点
 * @param nodeLineStyle 线的类型（例如折线、样条、三点圆弧等）
 * @param controlPoints 控制点列表，用于计算线的形状
 * @param linePoints 输出参数，保存计算得到的线段上的点
 * @param steps 步数，决定计算多少个中间点
 * @return 如果成功计算则返回 true，失败则返回 false
 */
bool calculateLinePoints(NodeLineStyle nodeLineStyle, const QVector<QPointF>& controlPoints, QVector<QPointF>& linePoints, int steps = 20);

/**
 * 根据节点类型和控制点计算闭合线上的点（多做了一下闭合）
 * @param lineStyle 线的类型
 * @param controlPoints 控制点列表
 * @param linePoints 输出参数，保存计算得到的闭合线上的点
 * @param steps 步数，决定计算多少个中间点
 * @return 如果成功计算则返回 true，失败则返回 false
 */
bool calculateCloseLinePoints(NodeLineStyle lineStyle, const QVector<QPointF>& controlPoints, QVector<QPointF>& linePoints, int steps = 20);

// 多个线一起计算的结构体
struct Component {
    int len;
    NodeLineStyle nodeLineStyle;

    Component() : len(0), nodeLineStyle(NodeLineStyle::NoStyle) {}
    Component(int length, NodeLineStyle style) : len(length), nodeLineStyle(style) {}

    // 序列化和反序列化
    static inline QString nodeLineStyleToString(NodeLineStyle style) {
        switch (style) {
        case NodeLineStyle::NoStyle: return "N";
        case NodeLineStyle::StylePolyline: return "P";
        case NodeLineStyle::StyleSpline: return "S";
        case NodeLineStyle::StyleThreePointArc: return "T";
        case NodeLineStyle::StyleArc: return "A";
        case NodeLineStyle::StyleTwoPointCircle: return "C";
        case NodeLineStyle::StyleStreamline: return "L";
        default: return "U";
        }
    }

    static inline QString serializeComponent(const Component& component) {
        QJsonObject jsonObject;
        jsonObject["l"] = component.len;
        jsonObject["n"] = nodeLineStyleToString(component.nodeLineStyle);
        // 将 QJsonObject 转换为 JSON 字符串
        QJsonDocument doc(jsonObject);
        return doc.toJson(QJsonDocument::Compact);  // 使用紧凑格式的JSON字符串
    }

    static inline NodeLineStyle stringToNodeLineStyle(const QString& str) {
        if (str == "N") return NodeLineStyle::NoStyle;
        if (str == "P") return NodeLineStyle::StylePolyline;
        if (str == "S") return NodeLineStyle::StyleSpline;
        if (str == "T") return NodeLineStyle::StyleThreePointArc;
        if (str == "A") return NodeLineStyle::StyleArc;
        if (str == "C") return NodeLineStyle::StyleTwoPointCircle;
        if (str == "L") return NodeLineStyle::StyleStreamline;
        return NodeLineStyle::NoStyle;
    }

    static inline Component deserializeComponent(const QString& data) {
        Component component;
        // 将 JSON 字符串转换为 QJsonObject
        QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
        if (doc.isObject()) {
            QJsonObject jsonObject = doc.object();
            component.len = jsonObject["l"].toInt();
            component.nodeLineStyle = stringToNodeLineStyle(jsonObject["n"].toString());
        }
        return component;
    }

    // 序列化 QVector<Component> 为 JSON 字符串
    static inline QString serializeComponentVector(const QVector<Component>& components) {
        QJsonArray jsonArray;
        for (const Component& component : components) {
            // 每个 component 序列化为 JSON 字符串
            jsonArray.append(serializeComponent(component));
        }
        // 将 QJsonArray 转换为 JSON 字符串
        QJsonDocument doc(jsonArray);
        return doc.toJson(QJsonDocument::Compact);  // 使用紧凑格式的JSON字符串
    }

    // 反序列化 JSON 字符串 为 QVector<Component>
    static inline QVector<Component> deserializeComponentVector(const QString& data) {
        QVector<Component> components;
        // 将 JSON 字符串转换为 QJsonArray
        QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
        if (doc.isArray()) {
            QJsonArray jsonArray = doc.array();
            for (const QJsonValue& value : jsonArray) {
                if (value.isString()) {
                    components.append(deserializeComponent(value.toString()));
                }
            }
        }
        return components;
    }
};

Q_DECLARE_METATYPE(Component)
Q_DECLARE_METATYPE(QVector<Component>)

/**
 * 根据多个线段的组件和控制点计算线上的点
 * @param component 组件列表
 * @param controlPoints 控制点列表
 * @param linePointss 输出参数，保存计算得到的线段上的点
 * @param steps 步数，决定计算多少个中间点
 * @return 返回成功绘制分图的个数
 */
int calculateLinePoints(const QVector<Component>& component, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps = 20);

/**
 * 根据多个线段的组件和控制点计算闭合线上的点
 * @param component 组件列表
 * @param controlPoints 控制点列表
 * @param linePointss 输出参数，保存计算得到的闭合线上的点
 * @param steps 步数，决定计算多少个中间点
 * @return 返回成功绘制分图的个数
 */
int calculateCloseLinePoints(const QVector<Component>& component, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps = 20);

/**
 * 根据多个线段的组件和控制点计算平行线（第二个分离一个点来控制距离）
 * @param component 组件列表
 * @param controlPoints 控制点列表
 * @param linePointss 输出参数，保存计算得到的闭合线上的点
 * @param steps 步数，决定计算多少个中间点
 * @return 返回成功绘制分图的个数
 */
int calculateParallelLinePoints(const QVector<Component>& components, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps = 20);

// 计算缓冲区
bool computeBufferBoundary(BufferCalculationMode mode, const QVector<QVector<QPointF>>& pointss, double r, QVector<QVector<QPointF>>& boundaryPointss);


// ==========================================================================
// 样条计算
// 贝塞尔曲线 和 B样条
// ==========================================================================

/**
 * 计算组合数（n 选 i），即计算 C(n, i)
 * @param n 总数
 * @param i 选择数
 * @return 计算出的组合数值
 */
int binomialCoefficient(int n, int i);

/**
 * 计算n次贝塞尔曲线的点（二维）
 * @param controlPoints 贝塞尔曲线的控制点列表
 * @param t 插值因子，通常在[0, 1]之间变化
 * @return 曲线上的一个点
 */
QPointF bezierN(const QVector<QPointF>& controlPoints, double t);

/**
 * 计算n次贝塞尔曲线上的多个点（二维）
 * @param controlPoints 贝塞尔曲线的控制点列表
 * @param numPoints 计算多少个曲线点
 * @return 包含所有计算得到的贝塞尔曲线点的列表
 */
QVector<QPointF> calculateBezierCurve(const QVector<QPointF>& controlPoints, int numPoints);

/**
 * Cox-de Boor 递归公式，用于计算 B 样条曲线上的点
 * @param knots 节点向量
 * @param i 当前节点的索引
 * @param p B 样条的次数
 * @param t 插值因子
 * @return 当前插值位置的权重
 */
double coxDeBoor(const QVector<double>& knots, int i, int p, double t);

/**
 * 生成 B 样条曲线的节点向量
 * @param n 控制点数
 * @param degree 曲线的次数
 * @return 生成的节点向量
 */
QVector<double> generateKnotVector(int n, int degree);

/**
 * 计算 B 样条曲线上的多个点（二维）
 * @param controlPoints 控制点列表
 * @param degree B 样条的次数
 * @param numCurvePoints 要计算的曲线点数
 * @param curvePoints 输出参数，保存计算得到的曲线点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateBSplineCurve(const QVector<QPointF>& controlPoints, int degree, int numCurvePoints, QVector<QPointF>& curvePoints);

// ==========================================================================
// 圆弧计算
// ==========================================================================

/**
 * 规范化角度，确保角度在[0, 2π)范围内
 * @param angle 输入角度（弧度制）
 * @return 规范化后的角度（弧度制）
 */
double normalizeAngle(double angle);

/**
 * 根据控制点计算三点圆弧的点
 * @param controlPoints 控制点列表
 * @param steps 步数，决定计算多少个点
 * @param arcPoints 输出参数，保存计算得到的弧线上的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateThreeArcPointsFromControlPoints(const QVector<QPointF>& controlPoints, int steps, QVector<QPointF>& arcPoints);

/**
 * 根据控制点计算圆弧的点
 * @param controlPoints 控制点列表
 * @param steps 步数，决定计算多少个点
 * @param arcPoints 输出参数，保存计算得到的弧线上的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateArcPointsFromControlPoints(const QVector<QPointF>& controlPoints, int steps, QVector<QPointF>& arcPoints);

/**
 * 根据三点计算弧线上的点
 * @param point1 圆上的第一个点
 * @param point2 圆上的第二个点
 * @param point3 圆上的第三个点
 * @param steps 步数，决定计算多少个点
 * @param arcPoints 输出参数，保存计算得到的弧线上的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateArcPointsFromThreePoints(const QPointF& point1, const QPointF& point2, const QPointF& point3, int steps, QVector<QPointF>& arcPoints);

/**
 * 根据两个点计算圆上的点
 * @param point1 圆上的第一个点
 * @param point2 圆上的第二个点
 * @param steps 步数，决定计算多少个点
 * @param arcPoints 输出参数，保存计算得到的圆上的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateCirclePointsFromControlPoints(const QPointF& point1, const QPointF& point2, int steps, QVector<QPointF>& arcPoints);

/**
 * 计算通过三点确定的圆的圆心和半径
 * @param p1 圆上的第一个点
 * @param p2 圆上的第二个点
 * @param p3 圆上的第三个点
 * @param center 输出参数，计算得到的圆心坐标
 * @param radius 输出参数，计算得到的圆的半径
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateCircle(const QPointF& p1, const QPointF& p2, const QPointF& p3, QPointF& center, double& radius);

/**
 * 计算通过两个点确定的圆的圆心和半径
 * @param p1 圆上的第一个点
 * @param p2 圆上的第二个点
 * @param center 输出参数，计算得到的圆心坐标
 * @param radius 输出参数，计算得到的圆的半径
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateCircle(const QPointF& p1, const QPointF& p2, QPointF& center, double& radius);

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
bool calculateArcPoints(const QPointF& center, double radius, double startAngle, double angleDiff, int steps, QVector<QPointF>& points);

// ==========================================================================
// 平行线计算
// ==========================================================================
// 参考：https://zhuanlan.zhihu.com/p/536948720

// 精度处理
int sgn(double x);
// 向量标准化
std::pair<double, double> normalize(double x, double y);

// 向量叉乘
double cross(double x1, double y1, double x2, double y2);

/**
 * 计算折线的双边平行线
 * @param polyline 输入折线的点列表
 * @param dis 平行线与折线的距离
 * @param leftPolyline 输出参数，保存平行线的左侧点
 * @param rightPolyline 输出参数，保存平行线的右侧点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateParallelLine(const QVector<QPointF>& polyline, double dis, QVector<QPointF>& leftPolyline, QVector<QPointF>& rightPolyline);

/**
 * 计算折线的单边平行线
 * @param polyline 输入折线的点列表
 * @param dis 平行线与折线的距离
 * @param parallelPolyline 输出参数，保存单边平行线的点
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateParallelLine(const QVector<QPointF>& polyline, double dis, QVector<QPointF>& parallelPolyline);


/**
 * 计算点到直线的垂直距离，并返回带方向的结果
 * @param point 点
 * @param lineStart 直线的起点
 * @param lineEnd 直线的终点
 * @param direction 返回值，用于表示点在直线的哪一侧，正值为左侧，负值为右侧，0为直线上
 * @return 点到直线的垂直距离
 */
double pointToLineDistanceWithDirection(const QPointF& point, const QPointF& lineStart, const QPointF& lineEnd);

/**
 * 计算平行折线，且平行线经过指定的点
 * @param polyline 输入折线的点列表
 * @param targetPoint 指定目标点，平行线将通过该点
 * @param parallelPolyline 输出参数，保存经过目标点的平行线
 * @return 如果计算成功则返回 true，失败则返回 false
 */
bool calculateParallelLineThroughPoint(const QVector<QPointF>& polyline, const QPointF& targetPoint, QVector<QPointF>& parallelPolyline);

// ==========================================================================================
// 基于栅格的缓冲区分析算法（暴力枚举所有点，深搜排序）
// ==========================================================================================

// 简化线
void simpleLine(QVector<QPointF>& points);

// GridMap 数据结构，用于存储网格映射信息
struct GridMap {
    double scale;     // 缩放比例
    QPointF offset;   // 偏移量
    int sizeX;        // 网格横向尺寸
    int sizeY;        // 网格纵向尺寸
};

/**
 * 计算点集的边界
 * @param pointss 输入点集
 * @return 点集的边界矩形
 */
QRectF calculateBounds(const QVector<QVector<QPointF>>& pointss);

/**
 * 将点集映射到网格
 * @param pointss 输入点集
 * @param r 网格大小
 * @param gridMap 输出网格映射
 */
void getGridMap(const QVector<QVector<QPointF>>& pointss, double r, GridMap& gridMap);

/**
 * 从网格恢复原始点集
 * @param gridPointss 网格点集
 * @param gridMap 网格映射
 * @param pointss 输出恢复后的点集
 */
void restoreFromGrid(const QVector<QVector<QPoint>>& gridPointss, const GridMap& gridMap, QVector<QVector<QPointF>>& pointss);


/**
 * 计算点到线段的垂直距离
 * @param point 点
 * @param start 线段起点
 * @param end 线段终点
 * @return 点到线段的垂直距离
 */
double pointToSegmentDistance(const QPointF& point, const QPointF& start, const QPointF& end);

/**
 * 判断点是否在任意折线的距离小于给定值
 * @param point 点
 * @param boundaryPointss 折线集合
 * @param distance 给定的距离阈值
 * @return 如果点距离某个折线小于阈值，返回true
 */
bool isPointCloseToAnyPolyline(const QPointF& point, const QVector<QVector<QPointF>>& boundaryPointss, double distance);

/**
 * 使用暴力算法标记边界点
 * @param pointss 输入点集
 * @param gridMap 网格映射
 * @param r 网格大小
 * @param boundaryPointss 输出边界点集合
 */
void markBoundaryPointsBruteForce(const QVector<QVector<QPointF>>& pointss, const GridMap& gridMap, double r, QVector<QVector<QPoint>>& boundaryPointss);

/**
 * Douglas-Peucker算法进行线简化
 * @param points 点集
 * @param epsilon 简化精度
 */
void douglasPeucker(QVector<QPointF>& points, double epsilon = 4);

/**
 * 计算缓冲区边界，使用栅格化算法
 * @param pointss 输入点集
 * @param r 缓冲区半径
 * @param boundaryPointss 输出缓冲区边界点集合
 * @return 如果成功，返回true
 */
bool computeBufferBoundaryWithGrid(const QVector<QVector<QPointF>>& pointss, double r, QVector<QVector<QPointF>>& boundaryPointss);





bool computeBufferBoundaryWithVector(const QVector<QVector<QPointF>>& pointss, double r, QVector<QVector<QPointF>>& boundaryPointss);
