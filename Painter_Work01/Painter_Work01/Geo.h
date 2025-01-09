#pragma once
#include <QPainter>
#include <QPointF>
#include <QColor>
#include <QVector>
#include <QKeyEvent>
#include "GeoMathUtil.h"

class Geo;
Geo* createGeo(DrawMode mode);

// ===================================================================== Geo
class Geo {
public:
    virtual ~Geo() = default;
    virtual void initialize() = 0;                      // ��ʼ�����ڵ�һ�ε����ȷ����һ�����Ƶ㣩��ʱ���Զ����ã�����������
    virtual void draw(QPainter& painter) = 0;           // ���Ʒ���
    virtual void drawControlPoints(QPainter& painter);  // ���ƿ��Ƶ�
    //virtual void drawBuffer(QPainter& painter) = 0;   // ���ƻ�����
    virtual void hitTesting(QPointF);                   // �������
    virtual void completeDrawing(); // ��ɹ��죨���ǰ�����¼������л��ƣ�

    GeoType getGeoType(); // ��������


    // �������ͼ����¼������л��ƻ��޸�(����Geo��������¼��������幦�ܣ�ֻά��״̬)
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    // ���������Ĭ��ʵ�֣�
    virtual void updateTempPoint(const QPoint& pos);   // ������ʱ��
    virtual void endSegmentDrawing();   // �����λ���
    virtual void pushControlPoint(const QPoint& pos); // ��ӿ��Ƶ�

    // ״̬��һЩ�޸�
    bool isStateDrawing();
    bool isStateComplete();
    bool isStateInvalid();
    bool isStateSelected();
    bool isStateInitialized();
    bool isMouseLeftButtonPressed();

    void setStateInitialized();
    void setStateInvalid();
    void setStateComplete();
    void setStateSelected();
    void setStateNotSelected();

    void markControlPointsChanged();   // ��ǿ��Ƶ��Ѹı�
    bool isControlPointsChanged() const; // �����Ƶ��Ƿ��Ѹı�
    void resetControlPointsChanged(); // ���ñ��
protected:


    void setGeoType(GeoType newType);   // ���캯����,ȷ������
    QVector<QPointF> controlPoints;    // ���Ƶ� (ÿ��������Լ��ӻ�����Ϣ)
    QPointF tempControlPoints;          // ��ʱ���Ƶ�, �ڻ�����ʹ��
private:
    int geoState = 0;
    bool mouseLeftButtonPressed = false; // ����Ƿ����϶�
    GeoType geoType = GeoType::Undefined;

    bool controlPointsChanged = false; // ���Ƶ�仯��Ҫ����
};

// ===================================================================== Point
class Point : public Geo {
public:
    Point();
    void initialize() override;

    // ͼ�ι����¼�
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void draw(QPainter& painter) override;

private:
    QColor color;                    // �����ɫ
    PointShape shape;                // �����״
};

// ===================================================================== SimpleLine
class SimpleLine : public Geo {
public:
    SimpleLine();
    void initialize() override;

    // ͼ�ι����¼�
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void draw(QPainter& painter) override;
protected:

    QVector<QPointF> points;        // �㼯
    NodeLineStyle nodeLineStyle;    // �ڵ�����
    
    float lineWidth;                    // �߿�
    float dashPattern;                  // ���߶γ�
    QColor color;                       // ��ɫ
    LineStyle lineStyle;                // ��ʽ��ʵ�ߡ����ߣ�
};

// ===================================================================== DoubleLine
class DoubleLine : public Geo {
public:
    DoubleLine();
    void initialize() override;

    // ͼ�ι����¼�
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void pushControlPoint(const QPoint& pos) override;
    void endSegmentDrawing() override;

    void draw(QPainter& painter) override;
protected:

    bool isDrawing;                       // �ڻ���
    QVector<QVector<QPointF>> pointss;    // ��ά�㼯
    QVector<Component> component;

    NodeLineStyle nodeLineStyle;   // �ڵ�����
    float lineWidth;               // �߿���
    QColor fillColor;              // ���������ɫ
    QColor lineColor;              // �߿���ɫ
    LineStyle lineStyle;           // �߿�����
    float lineDashPattern;         // ���߶γ�
};

// ===================================================================== ParallelLine
class ParallelLine : public Geo {
public:
    ParallelLine();
    void initialize() override;

    // ͼ�ι����¼�
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void endSegmentDrawing() override;
    void pushControlPoint(const QPoint& pos) override;

    void draw(QPainter& painter) override;
protected:

    QVector<QVector<QPointF>> pointss;    // ��ά�㼯
    QVector<Component> component;

    NodeLineStyle nodeLineStyle;   // �ڵ�����
    float lineWidth;               // �߿���
    QColor fillColor;              // ���������ɫ
    QColor lineColor;              // �߿���ɫ
    LineStyle lineStyle;           // �߿�����
    float lineDashPattern;         // ���߶γ�
};

// ===================================================================== TwoPointCircle
class TwoPointCircle : public Geo {
public:
    TwoPointCircle();
    void initialize() override;

    // ͼ�ι����¼�
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void pushControlPoint(const QPoint& pos) override;

    void draw(QPainter& painter) override;
protected:

    QVector<QPointF> points;       // �㼯
    float lineWidth;               // �߿���
    QColor fillColor;              // ���������ɫ
    QColor lineColor;              // �߿���ɫ
    LineStyle lineStyle;           // �߿�����
    float lineDashPattern;         // ���߶γ�
};

// ===================================================================== SimpleArea
class SimpleArea : public Geo {
public:
    SimpleArea();
    void initialize() override;

    // ͼ�ι����¼�
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void draw(QPainter& painter) override;
protected:

    QVector<QPointF> points;    // �㼯
    NodeLineStyle nodeLineStyle;    // �ڵ�����

    float lineWidth;               // �߿���
    QColor fillColor;              // ���������ɫ
    QColor lineColor;              // �߿���ɫ
    LineStyle lineStyle;           // �߿�����
    float lineDashPattern;         // ���߶γ�
};

// ===================================================================== ComplexArea 
class ComplexArea : public Geo {
public:
    ComplexArea();
    void initialize() override;

    // ͼ�ι����¼�
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    virtual void endSegmentDrawing();   // �����λ���
    virtual void pushControlPoint(const QPoint& pos); // ��ӿ��Ƶ�

    void draw(QPainter& painter) override;
protected:

    bool isDrawing;                       // �ڻ���
    QVector<QVector<QPointF>> pointss;    // ��ά�㼯
    QVector<Component> component;

    NodeLineStyle nodeLineStyle;   // �ڵ�����
    float lineWidth;               // �߿���
    QColor fillColor;              // ���������ɫ
    QColor lineColor;              // �߿���ɫ
    LineStyle lineStyle;           // �߿�����
    float lineDashPattern;         // ���߶γ�
};