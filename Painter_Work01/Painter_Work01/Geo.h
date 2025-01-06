#pragma once
#include <QPainter>
#include <QPointF>
#include <QColor>
#include <QVector>
#include <QKeyEvent>


class Geo {
public:
    enum class GeoType {
        Undefined = 0,         // δ����

        Point,                 // ��
        
        Polyline,              // ����
        Spline,                // ������
        ArcThreePoints,        // ����Բ��
        ArcTwoPoints,          // ����Բ��
        Streamline             // ����
    };

    enum class  GeoDrawState
    {
        Complete,   // ����״̬
        Drawing     // ���ڻ���
    };

    virtual ~Geo() = default;
    virtual void draw(QPainter& painter) = 0; // ���Ʒ���

    GeoType getType();
    GeoDrawState getGeoDrawState();
    bool getIsInvalid();
    
    // �������ͼ����¼������л���
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

protected:
    void setGeoType(Geo::GeoType newType);
    void setGeoDrawState(GeoDrawState newState);
    void setIsInvalid(bool flag);

private:
    GeoType geoType = GeoType::Undefined;
    GeoDrawState geoDrawState = GeoDrawState::Drawing;

    bool isInvalid = false; // ��Ч
};


class Point : public Geo {
public:
    enum class Shape {
        Square,
        Circle
    };

    Point();
    Point(const QPointF& position, QColor color = Qt::white, Shape shape = Shape::Square);
    ~Point();

    QPointF getPosition() const;
    void setPosition(const QPointF& newPos);

    QColor getColor() const;
    void setColor(const QColor& newColor);

    Shape getShape() const;
    void setShape(Shape newShape);

    void draw(QPainter& painter) override;

    void mousePressEvent(QMouseEvent* event) override;
private:
    QPointF position;  // ���λ��
    QColor color;      // �����ɫ
    Shape shape = Shape::Square;       // �����״
};


class BaseLine : public Geo {
public:
    enum class Style {
        Solid,     // ʵ��
        Dashed     // ����
    };
    BaseLine();
    BaseLine(const QVector<QPointF>& points,
        QColor color = Qt::black,
        Style style = Style::Solid,
        float width = 1.0f,
        float dashPattern = 1.0f);
    virtual ~BaseLine();

    // ���õ㼯
    void setPoints(const QVector<QPointF>& points);
    QVector<QPointF> getPoints() const;

    // ������ɫ
    void setColor(QColor color);
    QColor getColor() const;

    // ������ʽ
    void setStyle(Style style);
    Style getStyle() const;

    // �����߿�
    void setLineWidth(float width);
    float getLineWidth() const;

    // �������߶γ�
    void setDashPattern(float pattern);
    float getDashPattern() const;

    void pushPoint(QPointF& point);


protected:
    // ���� Polygon ��Ϊ��Ԫ��
    friend class Polygon;
    QVector<QPointF> points;    // �㼯
    QColor color = Qt::red;     // ��ɫ
    Style style = Style::Solid; // ��ʽ
    float lineWidth = 1.0f;       // �߿�
    float dashPattern = 1.0f;     // ���߶γ�
};


class Polyline : public BaseLine {
public:
    Polyline();
    Polyline(const QVector<QPointF>& points,
        QColor color = Qt::black,
        Style style = Style::Solid,
        float width = 1.0f);

    void mousePressEvent(QMouseEvent* event) override;

protected:
    void draw(QPainter& painter) override;
};

class Spline : public BaseLine {
public:
    Spline(const QVector<QPointF>& points,
        QColor color = Qt::black,
        Style style = Style::Solid,
        float width = 1.0f);
    Spline();
    void mousePressEvent(QMouseEvent* event) override;

protected:
    QVector<QPointF> curvePoints;
    void draw(QPainter& painter) override;
};



// ================================================ Polygon
class Polygon : public Geo {
public:
    Polygon(const QVector<QPointF>& points,
        const QColor& fillColor = Qt::white,
        const QColor& borderColor = Qt::black,
        BaseLine::Style borderStyle = BaseLine::Style::Solid,
        float borderWidth = 1.0f);
    Polygon();
    ~Polygon();

    void setFillColor(const QColor& color);
    QColor getFillColor() const;

    void setBorderColor(const QColor& color);
    QColor getBorderColor() const;

    void setBorderStyle(BaseLine::Style style);
    BaseLine::Style getBorderStyle() const;

    void setBorderWidth(float width);
    float getBorderWidth() const;

    void draw(QPainter& painter) override;

    void mousePressEvent(QMouseEvent* event) override;

private:
    BaseLine* edges = nullptr;
    QColor fillColor;               // ���������ɫ
    QColor borderColor;             // �߿���ɫ
    float borderWidth;              // �߿���
    BaseLine::Style borderStyle;    // �߿�����
};