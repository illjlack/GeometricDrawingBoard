#include "Line.h"

#include <QVector3D>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <myMath.h>

namespace My
{

    BaseLine::BaseLine(const QVector<QVector3D>& points,
        QColor color,
        Style style,
        float width,
        float dashPattern)
        : points(points), color(color), style(style), lineWidth(width), dashPattern(dashPattern) {}

    BaseLine::~BaseLine() 
    {
        // ������Դ������б�Ҫ�Ļ�
    }

    void BaseLine::setPoints(const QVector<QVector3D>& points) 
    {
        this->points = points;
    }

    QVector<QVector3D> BaseLine::getPoints() const 
    {
        return points;
    }

    void BaseLine::setColor(QColor color) 
    {
        this->color = color;
    }

    QColor BaseLine::getColor() const 
    {
        return color;
    }

    void BaseLine::setStyle(Style style) 
    {
        this->style = style;
    }

    BaseLine::Style BaseLine::getStyle() const 
    {
        return style;
    }

    void BaseLine::setLineWidth(float width) 
    {
        lineWidth = width;
    }

    float BaseLine::getLineWidth() const 
    {
        return lineWidth;
    }

    void BaseLine::setDashPattern(float pattern) 
    {
        dashPattern = pattern;
    }

    float BaseLine::getDashPattern() const 
    {
        return dashPattern;
    }



    void Polyline::initialize() 
    {
        QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

        vao.create();
        vbo.create();

        vao.bind();
        vbo.bind();

        QVector<float> vertexData;
        for (const auto& point : points) 
        {
            vertexData.append({ point.x(), point.y(), point.z() });
        }

        vbo.allocate(vertexData.data(), vertexData.size() * sizeof(float));

        functions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        functions->glEnableVertexAttribArray(0);

        vbo.release();
        vao.release();
    }

    void Polyline::draw() 
    {
        QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

        shaderProgram->bind();
        vao.bind();
        vbo.bind();

        shaderProgram->setUniformValue("color", QVector4D(color.redF(), color.greenF(), color.blueF(), 1.0f));
        //shaderProgram->setUniformValue("lineWidth", lineWidth);

        if (style == Style::Dashed) 
        {
            shaderProgram->setUniformValue("dashPattern", dashPattern);
        }


        functions->glLineWidth(20.0f);  // �����߿�Ϊ2����
        functions->glDrawArrays(GL_LINE_STRIP, 0, points.size());

        vbo.release();
        vao.release();
        shaderProgram->release();
    }

    void Polyline::drawBufferZone() {
        float d = 0.5;
        static QVector<QVector3D> bufferVertices;
        const int segments = points.size();
        if (!vboBufferZone.isCreated()) {
            QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();
            shaderProgram->bind();

            // ���ɿ��Ϊ d �Ļ���������
            QVector<QVector3D> bufferVertices;
            for (int i = 0; i < segments - 1; ++i) {
                QVector3D p0 = points[i];
                QVector3D p1 = points[i + 1];

                // ���㷽������
                QVector3D direction = p1 - p0;
                direction.normalize();  // ��һ����������

                // ���㷨����������Ϊ��ά�ռ�
                QVector3D normal = QVector3D(-direction.y(), direction.x(), 0);  // ��������ֱ�ڷ�������

                // ���ɻ��������㣬ƫ���߶�����
                bufferVertices.append(p0 + normal * d / 2);  // p0 ������ƫ��
                bufferVertices.append(p0 - normal * d / 2);  // p0 ����ƫ��

                bufferVertices.append(p1 + normal * d / 2);  // p1 ������ƫ��
                bufferVertices.append(p1 - normal * d / 2);  // p1 ����ƫ��
            }

            vboBufferZone.create();
            vaoBufferZone.create();
            vaoBufferZone.bind();
            vboBufferZone.bind();
            vboBufferZone.allocate(bufferVertices.data(), bufferVertices.size() * sizeof(QVector3D));
            functions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
            functions->glEnableVertexAttribArray(0);
            vboBufferZone.release();
            vaoBufferZone.release();
            shaderProgram->release();
        }

        // ���ƻ�����
        shaderProgram->bind();
        QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();
        functions->glEnable(GL_BLEND);
        functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        QVector4D vec4Color(color.redF(), color.greenF(), color.blueF(), 0.3f);  // ����͸����
        shaderProgram->setUniformValue("color", vec4Color);

        QMatrix4x4 modelMatrix;
        // modelMatrix.translate(position);  // ƽ�Ƶ�ָ��λ��
        shaderProgram->setUniformValue("model", modelMatrix);

        vaoBufferZone.bind();
        vboBufferZone.bind();
        functions->glDrawArrays(GL_TRIANGLE_FAN, 0, bufferVertices.size());
        vaoBufferZone.release();
        vboBufferZone.release();
        shaderProgram->release();

        functions->glDisable(GL_BLEND);
    }

    void Spline::initialize()
    {
        QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

        vao.create();
        vbo.create();

        vao.bind();
        vbo.bind();

        //vertexData = calculateBezierCurve(points, 100);
        vertexData = calculateBSpline(points, 4, 10000);

        vbo.allocate(vertexData.data(), 3*vertexData.size() * sizeof(float));

        functions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        functions->glEnableVertexAttribArray(0);

        vbo.release();
        vao.release();
    }

    void Spline::draw()
    {
        QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

        shaderProgram->bind();
        vao.bind();
        vbo.bind();

        shaderProgram->setUniformValue("color", QVector4D(color.redF(), color.greenF(), color.blueF(), 1.0f));
        //shaderProgram->setUniformValue("lineWidth", lineWidth);

        functions->glDrawArrays(GL_LINE_STRIP, 0, vertexData.size());

        vbo.release();
        vao.release();
        shaderProgram->release();
    }

    void Spline::drawBufferZone()
    {
    }

}
    
