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
// �����߶��ϵĵ�
// ����Ľӿڣ� ͳһ����
// ==========================================================================

/**
* �ܴ��澡����ʧ�ܻ���
* ����ͳһ�Ľӿڣ������Ķ�������������
*/

/**
 * ���ݽڵ����ͺͿ��Ƶ�������ϵĵ�
 * @param nodeLineStyle �ߵ����ͣ��������ߡ�����������Բ���ȣ�
 * @param controlPoints ���Ƶ��б����ڼ����ߵ���״
 * @param linePoints ����������������õ����߶��ϵĵ�
 * @param steps ����������������ٸ��м��
 * @return ����ɹ������򷵻� true��ʧ���򷵻� false
 */
bool calculateLinePoints(NodeLineStyle nodeLineStyle, const QVector<QPointF>& controlPoints, QVector<QPointF>& linePoints, int steps = 20);

/**
 * ���ݽڵ����ͺͿ��Ƶ����պ����ϵĵ㣨������һ�±պϣ�
 * @param lineStyle �ߵ�����
 * @param controlPoints ���Ƶ��б�
 * @param linePoints ����������������õ��ıպ����ϵĵ�
 * @param steps ����������������ٸ��м��
 * @return ����ɹ������򷵻� true��ʧ���򷵻� false
 */
bool calculateCloseLinePoints(NodeLineStyle lineStyle, const QVector<QPointF>& controlPoints, QVector<QPointF>& linePoints, int steps = 20);

// �����һ�����Ľṹ��
struct Component {
    int len;
    NodeLineStyle nodeLineStyle;

    Component() : len(0), nodeLineStyle(NodeLineStyle::NoStyle) {}
    Component(int length, NodeLineStyle style) : len(length), nodeLineStyle(style) {}

    // ���л��ͷ����л�
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
        // �� QJsonObject ת��Ϊ JSON �ַ���
        QJsonDocument doc(jsonObject);
        return doc.toJson(QJsonDocument::Compact);  // ʹ�ý��ո�ʽ��JSON�ַ���
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
        // �� JSON �ַ���ת��Ϊ QJsonObject
        QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
        if (doc.isObject()) {
            QJsonObject jsonObject = doc.object();
            component.len = jsonObject["l"].toInt();
            component.nodeLineStyle = stringToNodeLineStyle(jsonObject["n"].toString());
        }
        return component;
    }

    // ���л� QVector<Component> Ϊ JSON �ַ���
    static inline QString serializeComponentVector(const QVector<Component>& components) {
        QJsonArray jsonArray;
        for (const Component& component : components) {
            // ÿ�� component ���л�Ϊ JSON �ַ���
            jsonArray.append(serializeComponent(component));
        }
        // �� QJsonArray ת��Ϊ JSON �ַ���
        QJsonDocument doc(jsonArray);
        return doc.toJson(QJsonDocument::Compact);  // ʹ�ý��ո�ʽ��JSON�ַ���
    }

    // �����л� JSON �ַ��� Ϊ QVector<Component>
    static inline QVector<Component> deserializeComponentVector(const QString& data) {
        QVector<Component> components;
        // �� JSON �ַ���ת��Ϊ QJsonArray
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
 * ���ݶ���߶ε�����Ϳ��Ƶ�������ϵĵ�
 * @param component ����б�
 * @param controlPoints ���Ƶ��б�
 * @param linePointss ����������������õ����߶��ϵĵ�
 * @param steps ����������������ٸ��м��
 * @return ���سɹ����Ʒ�ͼ�ĸ���
 */
int calculateLinePoints(const QVector<Component>& component, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps = 20);

/**
 * ���ݶ���߶ε�����Ϳ��Ƶ����պ����ϵĵ�
 * @param component ����б�
 * @param controlPoints ���Ƶ��б�
 * @param linePointss ����������������õ��ıպ����ϵĵ�
 * @param steps ����������������ٸ��м��
 * @return ���سɹ����Ʒ�ͼ�ĸ���
 */
int calculateCloseLinePoints(const QVector<Component>& component, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps = 20);

/**
 * ���ݶ���߶ε�����Ϳ��Ƶ����ƽ���ߣ��ڶ�������һ���������ƾ��룩
 * @param component ����б�
 * @param controlPoints ���Ƶ��б�
 * @param linePointss ����������������õ��ıպ����ϵĵ�
 * @param steps ����������������ٸ��м��
 * @return ���سɹ����Ʒ�ͼ�ĸ���
 */
int calculateParallelLinePoints(const QVector<Component>& components, const QVector<QPointF>& controlPoints, QVector<QVector<QPointF>>& linePointss, int steps = 20);

// ���㻺����
bool computeBufferBoundary(BufferCalculationMode mode, const QVector<QVector<QPointF>>& pointss, double r, QVector<QVector<QPointF>>& boundaryPointss);


// ==========================================================================
// ��������
// ���������� �� B����
// ==========================================================================

/**
 * �����������n ѡ i���������� C(n, i)
 * @param n ����
 * @param i ѡ����
 * @return ������������ֵ
 */
int binomialCoefficient(int n, int i);

/**
 * ����n�α��������ߵĵ㣨��ά��
 * @param controlPoints ���������ߵĿ��Ƶ��б�
 * @param t ��ֵ���ӣ�ͨ����[0, 1]֮��仯
 * @return �����ϵ�һ����
 */
QPointF bezierN(const QVector<QPointF>& controlPoints, double t);

/**
 * ����n�α����������ϵĶ���㣨��ά��
 * @param controlPoints ���������ߵĿ��Ƶ��б�
 * @param numPoints ������ٸ����ߵ�
 * @return �������м���õ��ı��������ߵ���б�
 */
QVector<QPointF> calculateBezierCurve(const QVector<QPointF>& controlPoints, int numPoints);

/**
 * Cox-de Boor �ݹ鹫ʽ�����ڼ��� B ���������ϵĵ�
 * @param knots �ڵ�����
 * @param i ��ǰ�ڵ������
 * @param p B �����Ĵ���
 * @param t ��ֵ����
 * @return ��ǰ��ֵλ�õ�Ȩ��
 */
double coxDeBoor(const QVector<double>& knots, int i, int p, double t);

/**
 * ���� B �������ߵĽڵ�����
 * @param n ���Ƶ���
 * @param degree ���ߵĴ���
 * @return ���ɵĽڵ�����
 */
QVector<double> generateKnotVector(int n, int degree);

/**
 * ���� B ���������ϵĶ���㣨��ά��
 * @param controlPoints ���Ƶ��б�
 * @param degree B �����Ĵ���
 * @param numCurvePoints Ҫ��������ߵ���
 * @param curvePoints ����������������õ������ߵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateBSplineCurve(const QVector<QPointF>& controlPoints, int degree, int numCurvePoints, QVector<QPointF>& curvePoints);

// ==========================================================================
// Բ������
// ==========================================================================

/**
 * �淶���Ƕȣ�ȷ���Ƕ���[0, 2��)��Χ��
 * @param angle ����Ƕȣ������ƣ�
 * @return �淶����ĽǶȣ������ƣ�
 */
double normalizeAngle(double angle);

/**
 * ���ݿ��Ƶ��������Բ���ĵ�
 * @param controlPoints ���Ƶ��б�
 * @param steps ����������������ٸ���
 * @param arcPoints ����������������õ��Ļ����ϵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateThreeArcPointsFromControlPoints(const QVector<QPointF>& controlPoints, int steps, QVector<QPointF>& arcPoints);

/**
 * ���ݿ��Ƶ����Բ���ĵ�
 * @param controlPoints ���Ƶ��б�
 * @param steps ����������������ٸ���
 * @param arcPoints ����������������õ��Ļ����ϵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateArcPointsFromControlPoints(const QVector<QPointF>& controlPoints, int steps, QVector<QPointF>& arcPoints);

/**
 * ����������㻡���ϵĵ�
 * @param point1 Բ�ϵĵ�һ����
 * @param point2 Բ�ϵĵڶ�����
 * @param point3 Բ�ϵĵ�������
 * @param steps ����������������ٸ���
 * @param arcPoints ����������������õ��Ļ����ϵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateArcPointsFromThreePoints(const QPointF& point1, const QPointF& point2, const QPointF& point3, int steps, QVector<QPointF>& arcPoints);

/**
 * �������������Բ�ϵĵ�
 * @param point1 Բ�ϵĵ�һ����
 * @param point2 Բ�ϵĵڶ�����
 * @param steps ����������������ٸ���
 * @param arcPoints ����������������õ���Բ�ϵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateCirclePointsFromControlPoints(const QPointF& point1, const QPointF& point2, int steps, QVector<QPointF>& arcPoints);

/**
 * ����ͨ������ȷ����Բ��Բ�ĺͰ뾶
 * @param p1 Բ�ϵĵ�һ����
 * @param p2 Բ�ϵĵڶ�����
 * @param p3 Բ�ϵĵ�������
 * @param center �������������õ���Բ������
 * @param radius �������������õ���Բ�İ뾶
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateCircle(const QPointF& p1, const QPointF& p2, const QPointF& p3, QPointF& center, double& radius);

/**
 * ����ͨ��������ȷ����Բ��Բ�ĺͰ뾶
 * @param p1 Բ�ϵĵ�һ����
 * @param p2 Բ�ϵĵڶ�����
 * @param center �������������õ���Բ������
 * @param radius �������������õ���Բ�İ뾶
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateCircle(const QPointF& p1, const QPointF& p2, QPointF& center, double& radius);

/**
 * ����Բ�ġ��뾶����ʼ�Ƕȡ��ǶȲ�ֵ�Ͳ������㻡���ϵĵ�
 * @param center Բ������
 * @param radius Բ�İ뾶
 * @param startAngle ��ʼ�Ƕȣ������ƣ�
 * @param angleDiff �ǶȲ�ֵ�������ƣ�
 * @param steps ����������������ٸ���
 * @param points ����������������õ��Ļ����ϵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateArcPoints(const QPointF& center, double radius, double startAngle, double angleDiff, int steps, QVector<QPointF>& points);

// ==========================================================================
// ƽ���߼���
// ==========================================================================
// �ο���https://zhuanlan.zhihu.com/p/536948720

// ���ȴ���
int sgn(double x);
// ������׼��
std::pair<double, double> normalize(double x, double y);

// �������
double cross(double x1, double y1, double x2, double y2);

/**
 * �������ߵ�˫��ƽ����
 * @param polyline �������ߵĵ��б�
 * @param dis ƽ���������ߵľ���
 * @param leftPolyline �������������ƽ���ߵ�����
 * @param rightPolyline �������������ƽ���ߵ��Ҳ��
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateParallelLine(const QVector<QPointF>& polyline, double dis, QVector<QPointF>& leftPolyline, QVector<QPointF>& rightPolyline);

/**
 * �������ߵĵ���ƽ����
 * @param polyline �������ߵĵ��б�
 * @param dis ƽ���������ߵľ���
 * @param parallelPolyline ������������浥��ƽ���ߵĵ�
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateParallelLine(const QVector<QPointF>& polyline, double dis, QVector<QPointF>& parallelPolyline);


/**
 * ����㵽ֱ�ߵĴ�ֱ���룬�����ش�����Ľ��
 * @param point ��
 * @param lineStart ֱ�ߵ����
 * @param lineEnd ֱ�ߵ��յ�
 * @param direction ����ֵ�����ڱ�ʾ����ֱ�ߵ���һ�࣬��ֵΪ��࣬��ֵΪ�Ҳ࣬0Ϊֱ����
 * @return �㵽ֱ�ߵĴ�ֱ����
 */
double pointToLineDistanceWithDirection(const QPointF& point, const QPointF& lineStart, const QPointF& lineEnd);

/**
 * ����ƽ�����ߣ���ƽ���߾���ָ���ĵ�
 * @param polyline �������ߵĵ��б�
 * @param targetPoint ָ��Ŀ��㣬ƽ���߽�ͨ���õ�
 * @param parallelPolyline ������������澭��Ŀ����ƽ����
 * @return �������ɹ��򷵻� true��ʧ���򷵻� false
 */
bool calculateParallelLineThroughPoint(const QVector<QPointF>& polyline, const QPointF& targetPoint, QVector<QPointF>& parallelPolyline);

// ==========================================================================================
// ����դ��Ļ����������㷨������ö�����е㣬��������
// ==========================================================================================

// ����
void simpleLine(QVector<QPointF>& points);

// GridMap ���ݽṹ�����ڴ洢����ӳ����Ϣ
struct GridMap {
    double scale;     // ���ű���
    QPointF offset;   // ƫ����
    int sizeX;        // �������ߴ�
    int sizeY;        // ��������ߴ�
};

/**
 * ����㼯�ı߽�
 * @param pointss ����㼯
 * @return �㼯�ı߽����
 */
QRectF calculateBounds(const QVector<QVector<QPointF>>& pointss);

/**
 * ���㼯ӳ�䵽����
 * @param pointss ����㼯
 * @param r �����С
 * @param gridMap �������ӳ��
 */
void getGridMap(const QVector<QVector<QPointF>>& pointss, double r, GridMap& gridMap);

/**
 * ������ָ�ԭʼ�㼯
 * @param gridPointss ����㼯
 * @param gridMap ����ӳ��
 * @param pointss ����ָ���ĵ㼯
 */
void restoreFromGrid(const QVector<QVector<QPoint>>& gridPointss, const GridMap& gridMap, QVector<QVector<QPointF>>& pointss);


/**
 * ����㵽�߶εĴ�ֱ����
 * @param point ��
 * @param start �߶����
 * @param end �߶��յ�
 * @return �㵽�߶εĴ�ֱ����
 */
double pointToSegmentDistance(const QPointF& point, const QPointF& start, const QPointF& end);

/**
 * �жϵ��Ƿ����������ߵľ���С�ڸ���ֵ
 * @param point ��
 * @param boundaryPointss ���߼���
 * @param distance �����ľ�����ֵ
 * @return ��������ĳ������С����ֵ������true
 */
bool isPointCloseToAnyPolyline(const QPointF& point, const QVector<QVector<QPointF>>& boundaryPointss, double distance);

/**
 * ʹ�ñ����㷨��Ǳ߽��
 * @param pointss ����㼯
 * @param gridMap ����ӳ��
 * @param r �����С
 * @param boundaryPointss ����߽�㼯��
 */
void markBoundaryPointsBruteForce(const QVector<QVector<QPointF>>& pointss, const GridMap& gridMap, double r, QVector<QVector<QPoint>>& boundaryPointss);

/**
 * Douglas-Peucker�㷨�����߼�
 * @param points �㼯
 * @param epsilon �򻯾���
 */
void douglasPeucker(QVector<QPointF>& points, double epsilon = 4);

/**
 * ���㻺�����߽磬ʹ��դ���㷨
 * @param pointss ����㼯
 * @param r �������뾶
 * @param boundaryPointss ����������߽�㼯��
 * @return ����ɹ�������true
 */
bool computeBufferBoundaryWithGrid(const QVector<QVector<QPointF>>& pointss, double r, QVector<QVector<QPointF>>& boundaryPointss);





bool computeBufferBoundaryWithVector(const QVector<QVector<QPointF>>& pointss, double r, QVector<QVector<QPointF>>& boundaryPointss);
