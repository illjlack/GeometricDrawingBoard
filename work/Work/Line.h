#pragma once

#include "Object.h"

#include <QColor>
#include <QVector3D>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

namespace My {
    enum class LineType {
        Polyline,          // 折线
        Spline,            // 样条线
        ArcThreePoints,    // 三点圆弧
        ArcTwoPoints,      // 两点圆弧
        Streamline         // 流线
    };

    class BaseLine : public Object {
    public:
        enum class Style {
            Solid,             // 实线
            Dashed             // 虚线
        };

        BaseLine(const QVector<QVector3D>& points,
            QColor color = Qt::black,
            Style style = Style::Solid,
            float width = 1.0f,
            float dashPattern = 1.0f);
        virtual ~BaseLine();

        // 点集
        void setPoints(const QVector<QVector3D>& points);
        QVector<QVector3D> getPoints() const;

        // 颜色
        void setColor(QColor color);
        QColor getColor() const;

        // 样式
        void setStyle(Style style);
        Style getStyle() const;

        // 设置线宽
        void setLineWidth(float width);
        float getLineWidth() const;

        // 虚线段长
        void setDashPattern(float pattern);
        float getDashPattern() const;

    protected:
        QVector<QVector3D> points;  // 点集
        QColor color;               // 线颜色
        Style style;                // 线的样式
        float lineWidth;            // 线宽
        float dashPattern;          // 虚线线长

        QOpenGLBuffer vbo;
        QOpenGLVertexArrayObject vao;

        // 缓冲区
        QOpenGLBuffer vboBufferZone;
        QOpenGLVertexArrayObject vaoBufferZone;
    };


    class Polyline : public BaseLine {
    public:
        Polyline(const QVector<QVector3D>& points,
            QColor color = Qt::black,
            Style style = Style::Solid,
            float width = 1.0f)
            : BaseLine(points, color, style, width) {}

    protected:
        void initialize() override;
        void draw() override;
        virtual void drawBufferZone() override;
    };

    class Spline : public BaseLine {
    public:
        Spline(const QVector<QVector3D>& points,
            QColor color = Qt::black,
            Style style = Style::Solid,
            float width = 1.0f)
            : BaseLine(points, color, style, width) {}

    protected:
        void initialize() override;
        void draw() override;
        virtual void drawBufferZone() override;

        QVector<QVector3D> vertexData;
    };

}




/*


class ArcThreePoints : public Line {
public:
    ArcThreePoints(const QVector<QVector3D>& points, QColor color, float width, Style style)
        : Line(points, color, LineType::ArcThreePoints, style, width) {}

    void drawBufferZone() override {
        // 实现三点圆弧的缓冲区绘制逻辑
        qDebug() << "Drawing buffer zone for ArcThreePoints";
    }
};

class ArcTwoPoints : public Line {
public:
    ArcTwoPoints(const QVector<QVector3D>& points, QColor color, float width, Style style)
        : Line(points, color, LineType::ArcTwoPoints, style, width) {}

    void drawBufferZone() override {
        // 实现两点圆弧的缓冲区绘制逻辑
        qDebug() << "Drawing buffer zone for ArcTwoPoints";
    }
};

class Streamline : public Line {
public:
    Streamline(const QVector<QVector3D>& points, QColor color, float width, Style style)
        : Line(points, color, LineType::Streamline, style, width) {}

    void drawBufferZone() override {
        // 实现流线的缓冲区绘制逻辑
        qDebug() << "Drawing buffer zone for Streamline";
    }
};

*/