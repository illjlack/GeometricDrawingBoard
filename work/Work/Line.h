#pragma once

#include "Object.h"

#include <QColor>
#include <QVector3D>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

class Line : public Object {
public:
    enum class LineType {
        Polyline,          // ����
        Spline,            // ������
        ArcThreePoints,    // ����Բ��
        ArcTwoPoints,      // ����Բ��
        Streamline         // ����
    };

    enum class Style {
        Solid,             // ʵ��
        Dashed             // ����
    };

    Line(const QVector<QVector3D>& points,
        QColor color = Qt::black,
        LineType type = LineType::Polyline,
        Style style = Style::Solid,
        float width = 1.0f,
        float dashPattern = 1.0f);
    ~Line();

    void setPoints(const QVector<QVector3D>& points);
    QVector<QVector3D> getPoints() const;

    void setColor(QColor color);
    QColor getColor() const;

    void setLineType(LineType type);
    LineType getLineType() const;

    void setStyle(Style style);
    Style getStyle() const;

    void setLineWidth(float width);
    float getLineWidth() const;

    void setDashPattern(float pattern);
    float getDashPattern() const;

    void drawBufferZone()
    {

    }

private:
    QVector<QVector3D> points;  // �㼯
    QColor color;               // ����ɫ
    LineType type;              // �ߵ�����
    Style style;                // �ߵ���ʽ
    float lineWidth;            // �߿�
    float dashPattern;          // ����ģʽ

    QOpenGLBuffer vbo;
    QOpenGLVertexArrayObject vao;

    void initialize() override;
    void draw() override;
};