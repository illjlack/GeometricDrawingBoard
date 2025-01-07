#pragma once
#include <QPainter>
#include <QPointF>
#include <QColor>
#include <QVector>
#include <QKeyEvent>

#include "DrawSettings.h"

class Geo;
Geo* createGeo(DrawMode mode);

// ================================================================================================ Geo
class Geo {
public:
    virtual ~Geo() = default;
    virtual void draw(QPainter& painter) = 0; // ���Ʒ���
    virtual void completeDrawing();

    // �������ͼ����¼������л���
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    
    GeoType getGeoType();

    bool isStateDrawing();
    bool isStateComplete();
    bool isStateInvalid();
    bool isStateSelected();

    void setStateInvalid();
    void setStateComplete();
    void setStateSelected();
    void setStateNotSelected();
protected:
    void setGeoType(GeoType newType);   // ���캯����,ȷ������

private:
    int geoState = 0;
    GeoType geoType = GeoType::Undefined;
};

// ================================================================================================ ControlPoint

class ControlPoint : public QPointF
{
public:
    using QPointF::QPointF; // �̳� QPointF �����й��캯��
    ControlPoint(QPointF& point);
    void draw(QPainter& painter);
};


// ================================================================================================ Point
class Point : public Geo {
public:
    Point();
    ~Point();

    QPointF getPosition() const;
    void setPosition(const QPointF& newPos);

    QColor getColor() const;
    void setColor(const QColor& newColor);

    PointShape getShape() const;
    void setShape(PointShape newShape);

    void draw(QPainter& painter) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void completeDrawing() override;
private:
    QPointF position;                                                   // ���λ��
    QPointF tempPoint;
    QColor color = getSetting<QRgb>(Key_PointColor);                    // �����ɫ
    PointShape shape = getSetting<PointShape>(Key_PointShape);          // �����״
};

// ================================================================================================ BaseLine
class BaseLine : public Geo {
public:
    BaseLine();
    virtual ~BaseLine();

    // ������ɫ
    void setColor(QColor color);
    QColor getColor() const;

    // ��������
    void setLineStyle(LineStyle lineStyle);
    LineStyle getLineStyle() const;

    // ��������
    void setLineWidth(float width);
    float getLineWidth() const;

    // �������߶γ�
    void setDashPattern(float pattern);
    float getDashPattern() const;

    void pushPoint(QPointF& point);

    void mouseMoveEvent(QMouseEvent* event) override;

    void completeDrawing() override;

    virtual QVector<QPointF> getPoints();

protected:
    friend class Polygon;
    QVector<ControlPoint> controlPoints;                                // ���Ƶ�
    ControlPoint tempControlPoint;                                      // ��ʱ�Ŀ��Ƶ㣨׷������ƶ���

    float lineWidth = getSetting<float>(Key_LineWidth);                 // �߿�
    float dashPattern = getSetting<float>(Key_LineDashPattern);         // ���߶γ�
    QColor color = getSetting<QRgb>(Key_LineColor);                     // ��ɫ
    LineStyle lineStyle = getSetting<LineStyle>(Key_LineStyle);         // ��ʽ
};

// ================================================================================================ Polyline
class Polyline : public BaseLine {
public:
    Polyline();
    ~Polyline();
    void mousePressEvent(QMouseEvent* event) override;

protected:
    void draw(QPainter& painter) override;
};

// ================================================================================================ Spline
class Spline : public BaseLine {
public:
    Spline();
    ~Spline();
    void mousePressEvent(QMouseEvent* event) override;

    QVector<QPointF> getPoints() override;
protected:
    QVector<QPointF> curvePoints;
    void draw(QPainter& painter) override;
};

// ================================================================================================ Polygon
class Polygon : public Geo {
public:
    Polygon();
    ~Polygon();

    void setFillColor(const QColor& color);
    QColor getFillColor() const;

    void setBorderColor(const QColor& color);
    QColor getBorderColor() const;

    void setBorderStyle(LineStyle LineStyle);
    LineStyle getBorderStyle() const;

    void setBorderWidth(float width);
    float getBorderWidth() const;

    void draw(QPainter& painter) override;

    void mousePressEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

    void completeDrawing() override;

private:
    BaseLine* edges = nullptr;

    float lineWidth = getSetting<float>(Key_PgLineWidth);                  // �߿���
    QColor fillColor = getSetting<QRgb>(Key_PgFillColor);                  // ���������ɫ
    QColor lineColor = getSetting<QRgb>(Key_PgLineColor);                  // �߿���ɫ
    LineStyle lineStyle = getSetting<LineStyle>(Key_PgLineStyle);          // �߿�����
};