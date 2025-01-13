#pragma once

#include <QPainter>
#include <QPointF>
#include <QColor>
#include <QVector>
#include <QKeyEvent>
#include <QPainterPath>
#include "GeoMathUtil.h"
#include "comm.h"

class Geo;
Geo* createGeo(DrawMode mode);

// ===================================================================== Geo
class Geo 
{
public:
    virtual ~Geo() = default;

    // ��ȡ��������
    GeoType getGeoType();

    // �������ͼ����¼����л��ƻ��޸ģ������ά��״̬��
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event, const QPointF& pos);
    virtual void mousePressEvent(QMouseEvent* event, const QPointF& pos);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    // �������
    virtual bool hitTesting(const QPointF& point);

    void dragGeo(const QPointF& point);

    // ��ɹ���
    virtual void completeDrawing();
    // ���Ʒ���
    virtual void draw(QPainter& painter) = 0;

    // ״̬����
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
    virtual void initialize();                          // ��ʼ�����ڵ�һ�ε��ʱ��������
    virtual void drawControlPoints(QPainter& painter);  // ���ƿ��Ƶ�
    virtual void drawBuffer(QPainter& painter);         // ���ƻ�����

    void setGeoType(GeoType newType);   // ���캯������������

    QVector<QPointF> controlPoints;     // ���Ƶ�
    QPointF tempControlPoints;          // ��ʱ���Ƶ�
    GeoParameters geoParameters;        // ���β���

    virtual void updateTempPoint(const QPointF& pos);   // ������ʱ��
    virtual void endSegmentDrawing();   // �����λ���
    virtual void pushControlPoint(const QPointF& pos);  // ��ӿ��Ƶ�

    bool isMouseLeftButtonPressed();

    void setStateInitialized();
    void setStateInvalid();
    void setStateComplete();

    void markControlPointsChanged();            // ��ǿ��Ƶ�ı�
    bool isControlPointsChanged() const;        // �����Ƶ��Ƿ�ı�
    void resetControlPointsChanged();           // ���ñ��

    void markBufferChanged();            // ��ǻ������ı�
    bool isBufferChanged() const;        // ��黺�����Ƿ�ı�
    void resetBufferChanged();           // ���ñ��

    QPointF* currentSelectedPoint = nullptr; // ��ǰѡ�п��Ƶ�
    QPointF hitPoint;                       // ���λ�ã�������¼�ƶ����룩

private:
    
    int geoState = 0;                       // ����״̬
    bool mouseLeftButtonPressed = false;    // ���������±��
    GeoType geoType = GeoType::Undefined;   // ��������
    bool controlPointsChanged = false;      // ���Ƶ�仯���
    bool bufferChanged = true;              // �������仯���
};

// ===================================================================== Point
class Point : public Geo 
{
public:
    Point();

    void mousePressEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseMoveEvent(QMouseEvent* event, const QPointF& pos) override;
    void completeDrawing() override;

    void draw(QPainter& painter) override;
    void drawBuffer(QPainter& painter) override;

private:
    QPainterPath pathBuffer;                    // ·��������
    QVector<QVector<QPointF>> buffers;          // ������
    QPainterPath bufferPath;                    // ·��
};

// ===================================================================== SimpleLine
class SimpleLine : public Geo 
{
public:
    SimpleLine();

    void mousePressEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseMoveEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    bool hitTesting(const QPointF& point) override;

    void draw(QPainter& painter) override;
    void drawBuffer(QPainter& painter) override;

protected:
    QVector<QPointF> points;              // �㼯
    QPainterPath path;                    // ·��
    QVector<QVector<QPointF>> buffers;    // ������
    QPainterPath bufferPath;              // ·��
};

// ===================================================================== DoubleLine
class DoubleLine : public Geo 
{
public:
    DoubleLine();
    void initialize() override;

    void mousePressEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseMoveEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void pushControlPoint(const QPointF& pos) override;
    void endSegmentDrawing() override;

    bool hitTesting(const QPointF& point) override;

    void draw(QPainter& painter) override;
    void drawBuffer(QPainter& painter) override;

protected:
    bool isDrawing;                       // �Ƿ��ڻ���
    QVector<Component> component;         // ���Ƶ�����
    QVector<QVector<QPointF>> pointss;    // ��ά�㼯
    QPainterPath path;                    // ·��

    QVector<QVector<QPointF>> buffers;    // ������
    QPainterPath bufferPath;              // ·��
};

// ===================================================================== ParallelLine
class ParallelLine : public Geo 
{
public:
    ParallelLine();
    void initialize() override;

    void mousePressEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseMoveEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    bool hitTesting(const QPointF& point) override;

    void endSegmentDrawing() override;
    void pushControlPoint(const QPointF& pos) override;

    void draw(QPainter& painter) override;
    void drawBuffer(QPainter& painter) override;

protected:
    QVector<QVector<QPointF>> pointss;    // ��ά�㼯
    QVector<Component> component;
    QPainterPath path;                    // ·��

    QVector<QVector<QPointF>> buffers;    // ������
    QPainterPath bufferPath;              // ·��
};

// ===================================================================== TwoPointCircle
class TwoPointCircle : public Geo 
{
public:
    TwoPointCircle();

    void mousePressEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseMoveEvent(QMouseEvent* event, const QPointF& pos) override;
    void completeDrawing() override;

    void pushControlPoint(const QPointF& pos) override;

    bool hitTesting(const QPointF& point) override;

    void draw(QPainter& painter) override;
    void drawBuffer(QPainter& painter) override;

protected:
    QVector<QPointF> points;             // �㼯
    QPainterPath path;                   // ·��

    QVector<QVector<QPointF>> buffers;  // ������
    QPainterPath bufferPath;            // ·��
};

// ===================================================================== SimpleArea
class SimpleArea : public Geo 
{
public:
    SimpleArea();

    void mousePressEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseMoveEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    bool hitTesting(const QPointF& point) override;

    void draw(QPainter& painter) override;
    void drawBuffer(QPainter& painter) override;

protected:
    QVector<QPointF> points;             // �㼯
    QPainterPath path;                   // ·��

    QVector<QVector<QPointF>> buffers;  // ������
    QPainterPath bufferPath;            // ·��
};

// ===================================================================== ComplexArea 
class ComplexArea : public Geo 
{
public:
    ComplexArea();
    void initialize() override;

    void mousePressEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseMoveEvent(QMouseEvent* event, const QPointF& pos) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void completeDrawing() override;

    void endSegmentDrawing() override;
    void pushControlPoint(const QPointF& pos) override;

    bool hitTesting(const QPointF& point) override;

    void draw(QPainter& painter) override;
    void drawBuffer(QPainter& painter) override;

protected:
    bool isDrawing;                       // �Ƿ��ڻ���
    QVector<QVector<QPointF>> pointss;    // ��ά�㼯
    QPainterPath path;                    // ·��
    QVector<Component> component;

    QVector<QVector<QPointF>> buffers;    // ������
    QPainterPath bufferPath;              // ·��
};