#pragma once
#include <QPainter>
#include <QPointF>
#include <QColor>
#include <QVector>
#include <QKeyEvent>

#include "DrawSettings.h"

// ================================================ Geo
class Geo {
public:
    virtual ~Geo() = default;
    virtual void draw(QPainter& painter) = 0; // ���Ʒ���

    // �������ͼ����¼������л���
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    GeoType getType();
    GeoDrawState getGeoDrawState();
    bool getIsInvalid();

protected:
    void setGeoType(GeoType newType);
    void setGeoDrawState(GeoDrawState newState);
    void setIsInvalid(bool flag);

private:
    GeoType geoType = GeoType::Undefined;
    GeoDrawState geoDrawState = GeoDrawState::Drawing;

    bool isInvalid = false;
    bool isSelected = false;
};

// ================================================ Point
class Point : public Geo {
public:
    Point();
    Point(const QPointF& position, QColor color, PointShape shape);
    ~Point();

    QPointF getPosition() const;
    void setPosition(const QPointF& newPos);

    QColor getColor() const;
    void setColor(const QColor& newColor);

    PointShape getShape() const;
    void setShape(PointShape newShape);

    void draw(QPainter& painter) override;

    void mousePressEvent(QMouseEvent* event) override;
private:
    QPointF position;                                                   // ���λ��
    QColor color = getSetting<QRgb>(Key_PointColor);                    // �����ɫ
    PointShape shape = getSetting<PointShape>(Key_PointShape);          // �����״
};

// ================================================ BaseLine
class BaseLine : public Geo {
public:
    BaseLine();
    virtual ~BaseLine();

    // ���õ㼯
    void setPoints(const QVector<QPointF>& points);
    QVector<QPointF> getPoints() const;

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


protected:
    // ���� Polygon ��Ϊ��Ԫ��(ͼ�ı�����)
    friend class Polygon;
    QVector<QPointF> controlPoint;                                      // ���Ƶ�

    float lineWidth = getSetting<float>(Key_LineWidth);                 // �߿�
    float dashPattern = getSetting<float>(Key_LineDashPattern);         // ���߶γ�
    QColor color = getSetting<QRgb>(Key_LineColor);                     // ��ɫ
    LineStyle lineStyle = getSetting<LineStyle>(Key_LineStyle);         // ��ʽ
};

// ================================================ Polyline
class Polyline : public BaseLine {
public:
    Polyline();
    ~Polyline();
    void mousePressEvent(QMouseEvent* event) override;

protected:
    void draw(QPainter& painter) override;
};

// ================================================ Spline
class Spline : public BaseLine {
public:
    Spline();
    ~Spline();
    void mousePressEvent(QMouseEvent* event) override;

protected:
    QVector<QPointF> curvePoints;
    void draw(QPainter& painter) override;
};

// ================================================ Polygon
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

private:
    BaseLine* edges = nullptr;
    QColor fillColor = getSetting<QRgb>(Key_PgFillColor);                  // ���������ɫ
    QColor lineColor = getSetting<QRgb>(Key_PgLineColor);                  // �߿���ɫ
    float lineWidth = getSetting<float>(Key_PgLineWidth);                  // �߿���
    LineStyle lineStyle = getSetting<LineStyle>(Key_PgLineStyle);          // �߿�����
};