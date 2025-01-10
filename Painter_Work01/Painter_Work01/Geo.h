#pragma once
#include <QPainter>
#include <QPointF>
#include <QColor>
#include <QVector>
#include <QKeyEvent>
#include "GeoMathUtil.h"
#include "comm.h"

class Geo;
Geo* createGeo(DrawMode mode);

// ===================================================================== Geo
class Geo {
public:
    virtual ~Geo() = default;

    // ��������
    GeoType getGeoType(); 

    // �������ͼ����¼������л��ƻ��޸�(����Geo��������¼��������幦�ܣ�ֻά��״̬)
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    // �������
    virtual bool hitTesting(QPointF);
    // ��ɹ���
    virtual void completeDrawing();  
    // ���Ʒ���
    virtual void draw(QPainter& painter) = 0; 

    // ״̬��һЩ�޸�
    bool isStateDrawing();
    bool isStateComplete();
    bool isStateInvalid();
    bool isStateSelected();
    bool isStateInitialized();

    void setStateSelected();
    void setStateNotSelected();
              
    GeoParameters getGeoParameters();                        // ��ȡ��ǰ�� GeoParameters
    void setGeoParameters(const GeoParameters& params);      // ���� GeoParameters

protected:
    virtual void initialize();                          // ��ʼ�����ڵ�һ�ε����ȷ����һ�����Ƶ㣩��ʱ���Զ����ã�����������
    
    virtual void drawControlPoints(QPainter& painter);  // ���ƿ��Ƶ�
    virtual void drawBuffer(QPainter& painter);         // ���ƻ�����
    
    void setGeoType(GeoType newType);   // ���캯����,ȷ������
    QVector<QPointF> controlPoints;     // ���Ƶ� (ÿ��������Լ��ӻ�����Ϣ)
    QPointF tempControlPoints;          // ��ʱ���Ƶ�, �ڻ�����ʹ��
    GeoParameters geoParameters;        // ���εĲ���

    // ���������Ĭ��ʵ�֣�
    virtual void updateTempPoint(const QPoint& pos);   // ������ʱ��
    virtual void endSegmentDrawing();   // �����λ���
    virtual void pushControlPoint(const QPoint& pos); // ��ӿ��Ƶ�

    bool isMouseLeftButtonPressed();

    void setStateInitialized();
    void setStateInvalid();
    void setStateComplete();

    void markControlPointsChanged();            // ��ǿ��Ƶ��Ѹı�
    bool isControlPointsChanged() const;        // �����Ƶ��Ƿ��Ѹı�
    void resetControlPointsChanged();           // ���ñ��

    void markBufferChanged();            // ��ǿ��Ƶ��Ѹı�
    bool isBufferChanged() const;        // �����Ƶ��Ƿ��Ѹı�
    void resetBufferChanged();           // ���ñ��

    QPointF* currentSelectedPoint = nullptr; // ��ǰѡ�п��Ƶ�

private:

    int geoState = 0;                       // ״̬
    bool mouseLeftButtonPressed = false;    // ����Ƿ����϶�
    GeoType geoType = GeoType::Undefined;   // ���伸�ε�����
    bool controlPointsChanged = false;      // ���Ƶ�仯��Ҫ����
    bool bufferChanged = true;             // �������仯��Ҫ����
};

// ===================================================================== Point
class Point : public Geo {
public:
    Point();

    // ͼ�ι����¼�
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void draw(QPainter& painter) override;

private:
};

// ===================================================================== SimpleLine
class SimpleLine : public Geo {
public:
    SimpleLine();

    // ͼ�ι����¼�
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void draw(QPainter& painter) override;
protected:
    QVector<QPointF> points;        // �㼯
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
    void drawBuffer(QPainter& painter) override;
protected:

    bool isDrawing;                       // �ڻ���
    QVector<QVector<QPointF>> pointss;    // ��ά�㼯

    QVector<QVector<QPointF>> buffers;    // ������

    QVector<Component> component;

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
};

// ===================================================================== TwoPointCircle
class TwoPointCircle : public Geo {
public:
    TwoPointCircle();

    // ͼ�ι����¼�
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void pushControlPoint(const QPoint& pos) override;

    void draw(QPainter& painter) override;
protected:

    QVector<QPointF> points;       // �㼯
};

// ===================================================================== SimpleArea
class SimpleArea : public Geo {
public:
    SimpleArea();

    // ͼ�ι����¼�
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void draw(QPainter& painter) override;
protected:

    QVector<QPointF> points;    // �㼯
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
};