#include "Point.h"
#include <OpenGLWidget.h>


namespace My
{

    Point::Point(const QVector3D& position, QColor color, Shape shape)
        : position(position), color(color), shape(shape)
    {
    }

    Point::~Point() {
        // 清理资源
        vbo.destroy();
        vao.destroy();
    }

    QVector3D Point::getPosition() const
    {
        return position;
    }

    void Point::setPosition(const QVector3D& newPos)
    {
        position = newPos;
    }

    QColor Point::getColor() const
    {
        return color;
    }

    void Point::setColor(const QColor& newColor)
    {
        color = newColor;
    }

    Point::Shape Point::getShape() const
    {
        return shape;
    }

    void Point::setShape(Shape newShape)
    {
        shape = newShape;
    }

    void Point::initialize()
    {
        // 获取 OpenGL 函数
        QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

        shaderProgram->bind();
        // 创建 VAO 和 VBO
        vao.create();  // 创建 VAO
        vbo.create();  // 创建 VBO

        // 设置 VAO
        vao.bind();  // 绑定 VAO

        // 设置 VBO
        vbo.bind();  // 绑定 VBO
        if (shape == Shape::Square) {
            GLfloat squareVertices[] = {
                -0.01f,  0.01f, 0.0f,  // 左上
                 0.01f,  0.01f, 0.0f,  // 右上
                 0.01f, -0.01f, 0.0f,  // 右下
                 -0.01f, -0.01f, 0.0f,   // 左下
            };
            vbo.allocate(squareVertices, sizeof(squareVertices));
        }
        else if (shape == Shape::Circle) {
            const int segments = 100;
            GLfloat circleVertices[segments * 3];
            GLfloat angleStep = 2.0f * 3.1415926f / segments;

            for (int i = 0; i < segments; ++i) {
                GLfloat angle = i * angleStep;
                circleVertices[i * 3] = 0.01f * cos(angle);
                circleVertices[i * 3 + 1] = 0.01f * sin(angle);
                circleVertices[i * 3 + 2] = 0;
            }
            vbo.allocate(circleVertices, sizeof(circleVertices));
        }

        // 设置顶点属性指针
        functions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        functions->glEnableVertexAttribArray(0);

        // 解绑 VBO 和 VAO
        vbo.release();
        vao.release();

        shaderProgram->release();
    }


    void Point::draw()
    {
        shaderProgram->bind();
        QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

        // 设置颜色
        QVector4D vec4Color(color.redF(), color.greenF(), color.blueF(), 1.0f);
        shaderProgram->setUniformValue("color", vec4Color);

        // 计算模型矩阵（平移到 position）
        QMatrix4x4 modelMatrix;
        modelMatrix.translate(position);  // 将模型平移到指定位置
        // 传递模型矩阵到着色器
        shaderProgram->setUniformValue("model", modelMatrix);

        // 绑定 VAO 并绘制
        vao.bind();
        vbo.bind();
        if (shape == Shape::Square) {
            functions->glDrawArrays(GL_TRIANGLE_FAN, 0, 4);  // 绘制方形
        }
        else if (shape == Shape::Circle) {
            functions->glDrawArrays(GL_TRIANGLE_FAN, 0, 100);  // 绘制圆形
        }
        vao.release();
        vbo.release();
        shaderProgram->release();
    }


    QVector<QVector3D> Point::createBufferZone(float d, int segments) const {
        QVector<QVector3D> bufferVertices;
        bufferVertices.reserve(segments);

        float angleStep = 2.0f * 3.1415926f / segments;

        for (int i = 0; i < segments; ++i) {
            float angle = i * angleStep;
            float x = d * cos(angle);
            float y = d * sin(angle);
            float z = 0;
            bufferVertices.append(QVector3D(x, y, z));
        }

        return bufferVertices;
    }

    void Point::drawBufferZone()
    {
        const int segments = 100;
        if (!vboBufferZone.isCreated())
        {
            // 获取 OpenGL 函数
            QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

            shaderProgram->bind();
            vaoBufferZone.create();
            vboBufferZone.create();

            vaoBufferZone.bind();
            vboBufferZone.bind();

            QVector<QVector3D> bufferVertices = createBufferZone(0.1, segments);

            vboBufferZone.allocate(bufferVertices.data(), 3 * bufferVertices.size() * sizeof(float));


            functions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
            functions->glEnableVertexAttribArray(0);

            vboBufferZone.release();
            vaoBufferZone.release();
            shaderProgram->release();
        }


        shaderProgram->bind();
        QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

        // 启用混合模式，实现透明效果
        functions->glEnable(GL_BLEND);
        functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 设置透明蓝色
        QVector4D vec4Color(0.0f, 0.0f, 1.0f, 0.3f);  // 蓝色，透明度 50%
        shaderProgram->setUniformValue("color", vec4Color);

        // 计算模型矩阵（平移到 position）
        QMatrix4x4 modelMatrix;
        modelMatrix.translate(position);  // 将模型平移到指定位置
        shaderProgram->setUniformValue("model", modelMatrix);

        // 绑定 VAO 并绘制
        vaoBufferZone.bind();
        vboBufferZone.bind();

        // 使用透明蓝色绘制缓存区
        functions->glDrawArrays(GL_TRIANGLE_FAN, 0, segments);

        vaoBufferZone.release();
        vboBufferZone.release();
        shaderProgram->release();

        // 禁用混合模式
        functions->glDisable(GL_BLEND);
    }

}