#pragma once
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <glm/glm.hpp>
#include <stb_image.h>
#include <string>
#include <vector>
#include "Object.h"
#include <glm/gtc/type_ptr.hpp>

class CubeRenderer
   : public Object
{
public:
    CubeRenderer()
        : vbo(QOpenGLBuffer::VertexBuffer), vao()
    {
        // 获取当前 OpenGL 上下文
        functions = QOpenGLContext::currentContext()->functions();
        initialize();
    }

    ~CubeRenderer()
    {
        // 清理资源
        vbo.destroy();
        vao.destroy();
    }

    // 初始化立方体的着色器和数据
    void initialize()
    {
        // 加载着色器
        shader.addShaderFromSourceFile(QOpenGLShader::Vertex, "vertex_shader.glsl");
        shader.addShaderFromSourceFile(QOpenGLShader::Fragment, "fragment_shader.glsl");
        shader.link();

        // 立方体顶点数据
        GLfloat vertices[] = {
            // 位置                // 纹理坐标
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };

        // 创建 VAO 和 VBO
        vao.create();
        vbo.create();

        // 绑定 VAO 和 VBO
        vao.bind();
        vbo.bind();
        vbo.allocate(vertices, sizeof(vertices));

        // 设置顶点属性
        shader.bind();
        shader.setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat)); // 位置属性
        shader.enableAttributeArray(0);
        shader.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat)); // 纹理坐标属性
        shader.enableAttributeArray(1);
        shader.release();

        // 初始化纹理
        texture1 = new QOpenGLTexture(QImage("muxiang.png").mirrored());
        texture2 = new QOpenGLTexture(QImage("huiyuanai.png").mirrored());
    }

    // 渲染立方体
    void draw() override
    {
        // 使用着色器
        shader.bind();

        shader.setUniformValue("texture1", 0);
        shader.setUniformValue("texture2", 1);


        // 设置矩阵
        // shader.setUniformValue("view", QMatrix4x4(glm::value_ptr(view)));
        // shader.setUniformValue("projection", QMatrix4x4(glm::value_ptr(projection)));

        // 激活纹理
        texture1->bind(0);
        texture2->bind(1);

        // 世界空间中每个立方体的位置
        QVector3D cubePositions[] = {
            QVector3D(0.0f,  0.0f,  0.0f),
            QVector3D(2.0f,  5.0f, -15.0f),
            QVector3D(-1.5f, -2.2f, -2.5f),
            QVector3D(-3.8f, -2.0f, -12.3f),
            QVector3D(2.4f, -0.4f, -3.5f),
            QVector3D(-1.7f,  3.0f, -7.5f),
            QVector3D(1.3f, -2.0f, -2.5f),
            QVector3D(1.5f,  2.0f, -2.5f),
            QVector3D(1.5f,  0.2f, -1.5f),
            QVector3D(-1.3f,  1.0f, -1.5f)
        };

        vao.bind();

        for (unsigned int i = 0; i < 10; i++) // 渲染 10 个立方体
        {
            // 为每个立方体计算模型矩阵并传递给着色器
            QMatrix4x4 model; // 使用 Qt 的 QMatrix4x4 初始化模型矩阵
            model.translate(cubePositions[i]); // 将立方体平移到指定位置
            float angle = 20.0f * i; // 计算旋转角度，每个立方体的角度不同
            model.rotate(angle, 1.0f, 0.3f, 0.5f); // 旋转立方体
            shader.setUniformValue("model", model); // 将模型矩阵传递给着色器
            
            // 绘制立方体（三角形数组，36 个顶点）
            functions->glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        vao.release();

        // 释放资源
        shader.release();
    }

private:
    QOpenGLShaderProgram shader;
    QOpenGLBuffer vbo;
    QOpenGLVertexArrayObject vao;
    QOpenGLFunctions* functions;

    QOpenGLTexture* texture1;
    QOpenGLTexture* texture2;
};
