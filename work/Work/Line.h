#pragma once

#include "Object.h"

#include <QColor>
#include <QVector3D>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

class Line : public Object {
public:
    enum class LineType {
        Polyline,          // 折线
        Spline,            // 样条线
        ArcThreePoints,    // 三点圆弧
        ArcTwoPoints,      // 两点圆弧
        Streamline         // 流线
    };

    enum class Style {
        Solid,             // 实线
        Dashed             // 虚线
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
    QVector<QVector3D> points;  // 点集
    QColor color;               // 线颜色
    LineType type;              // 线的类型
    Style style;                // 线的样式
    float lineWidth;            // 线宽
    float dashPattern;          // 虚线模式

    QOpenGLBuffer vbo;
    QOpenGLVertexArrayObject vao;

    void initialize() override;
    void draw() override;
};