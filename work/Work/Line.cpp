#include "Line.h"

#include "Line.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

Line::Line(const QVector<QVector3D>& points,
    QColor color,
    LineType type,
    Style style,
    float width,
    float dashPattern)
    : points(points), color(color), type(type), style(style), lineWidth(width), dashPattern(dashPattern) {}

Line::~Line() 
{
    vbo.destroy();
    vao.destroy();
}

void Line::setPoints(const QVector<QVector3D>& newPoints) 
{
    points = newPoints;
}

QVector<QVector3D> Line::getPoints() const 
{
    return points;
}

void Line::setColor(QColor newColor) 
{
    color = newColor;
}

QColor Line::getColor() const 
{
    return color;
}

void Line::setLineType(LineType newType) 
{
    type = newType;
}

Line::LineType Line::getLineType() const 
{
    return type;
}

void Line::setStyle(Style newStyle) {
    style = newStyle;
}

Line::Style Line::getStyle() const {
    return style;
}

void Line::setLineWidth(float width) {
    lineWidth = width;
}

float Line::getLineWidth() const {
    return lineWidth;
}

void Line::setDashPattern(float pattern) {
    dashPattern = pattern;
}

float Line::getDashPattern() const {
    return dashPattern;
}

void Line::initialize() {
    QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

    vao.create();
    vbo.create();

    vao.bind();
    vbo.bind();

    QVector<float> vertexData;
    for (const auto& point : points) {
        vertexData.append({ point.x(), point.y(), point.z() });
    }

    vbo.allocate(vertexData.data(), vertexData.size() * sizeof(float));

    functions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    functions->glEnableVertexAttribArray(0);

    vbo.release();
    vao.release();
}

void Line::draw() {
    QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

    shaderProgram->bind();
    vao.bind();
    vbo.bind();

    shaderProgram->setUniformValue("color", QVector4D(color.redF(), color.greenF(), color.blueF(), 1.0f));
    //shaderProgram->setUniformValue("lineWidth", lineWidth);

    if (style == Style::Dashed) {
        shaderProgram->setUniformValue("dashPattern", dashPattern);
    }

    if (type == LineType::Polyline) {
        functions->glLineWidth(20.0f);  // 设置线宽为2像素
        functions->glDrawArrays(GL_LINE_STRIP, 0, points.size());
    }
    else if (type == LineType::Spline) {
        // 实现样条线逻辑
    }
    else if (type == LineType::ArcThreePoints || type == LineType::ArcTwoPoints) {
        // 实现圆弧逻辑
    }

    vbo.release();
    vao.release();
    shaderProgram->release();
}
