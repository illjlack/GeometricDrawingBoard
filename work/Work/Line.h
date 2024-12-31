#pragma once

#include "Object.h"

#include <QColor>
#include <QVector3D>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

namespace My {
    enum class LineType {
        Polyline,          // ����
        Spline,            // ������
        ArcThreePoints,    // ����Բ��
        ArcTwoPoints,      // ����Բ��
        Streamline         // ����
    };

    class BaseLine : public Object {
    public:
        enum class Style {
            Solid,             // ʵ��
            Dashed             // ����
        };

        BaseLine(const QVector<QVector3D>& points,
            QColor color = Qt::black,
            Style style = Style::Solid,
            float width = 1.0f,
            float dashPattern = 1.0f);
        virtual ~BaseLine();

        // �㼯
        void setPoints(const QVector<QVector3D>& points);
        QVector<QVector3D> getPoints() const;

        // ��ɫ
        void setColor(QColor color);
        QColor getColor() const;

        // ��ʽ
        void setStyle(Style style);
        Style getStyle() const;

        // �����߿�
        void setLineWidth(float width);
        float getLineWidth() const;

        // ���߶γ�
        void setDashPattern(float pattern);
        float getDashPattern() const;

    protected:
        QVector<QVector3D> points;  // �㼯
        QColor color;               // ����ɫ
        Style style;                // �ߵ���ʽ
        float lineWidth;            // �߿�
        float dashPattern;          // �����߳�

        QOpenGLBuffer vbo;
        QOpenGLVertexArrayObject vao;

        // ������
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
        // ʵ������Բ���Ļ����������߼�
        qDebug() << "Drawing buffer zone for ArcThreePoints";
    }
};

class ArcTwoPoints : public Line {
public:
    ArcTwoPoints(const QVector<QVector3D>& points, QColor color, float width, Style style)
        : Line(points, color, LineType::ArcTwoPoints, style, width) {}

    void drawBufferZone() override {
        // ʵ������Բ���Ļ����������߼�
        qDebug() << "Drawing buffer zone for ArcTwoPoints";
    }
};

class Streamline : public Line {
public:
    Streamline(const QVector<QVector3D>& points, QColor color, float width, Style style)
        : Line(points, color, LineType::Streamline, style, width) {}

    void drawBufferZone() override {
        // ʵ�����ߵĻ����������߼�
        qDebug() << "Drawing buffer zone for Streamline";
    }
};

*/