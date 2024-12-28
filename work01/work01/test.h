// CubeRenderer.h
#pragma once
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <glm/glm.hpp>
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
    }

    ~CubeRenderer()
    {
        // 清理资源
        vbo.destroy();
        vao.destroy();
        delete texture;
    }

    // 初始化立方体的着色器和数据
    void initialize()
    {
        // 立方体顶点数据，所有面共享相同的纹理坐标
        GLfloat vertices[] = {
            // 位置               // 纹理坐标
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

        // 位置属性(第一个参数就是顶点着色器里的location = 0表示颜色参数，3个分量，数据类型是 float，不需要归一化，步幅值，偏移)
        functions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        functions->glEnableVertexAttribArray(0);

        functions->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        functions->glEnableVertexAttribArray(1);


        vao.release();
        vbo.release();

        // 初始化纹理
        texture = new QOpenGLTexture(QImage("huiyuanai.png").mirrored());
    }

    void setShader(QOpenGLShaderProgram* shader) override
    {
        this->shader = shader;
    }

    void draw() override
    {
        // 确保着色器绑定成功
        if (!shader->bind()) {
            qDebug() << "Failed to bind shader!";
            return;
        }

        // 激活并绑定纹理
        texture->bind(0);

        // 检查纹理 uniform 的位置是否有效
        int textureUniformLocation = shader->uniformLocation("texture");
        if (textureUniformLocation == -1) {
            qDebug() << "Uniform 'texture1' not found in shader!";
            return;
        }

        // 设置纹理单元
        shader->setUniformValue(textureUniformLocation, 0);

        // 定义世界空间中每个立方体的位置
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

        // 确保 VAO 绑定成功
        vao.bind();

        // 遍历每个立方体，设置模型矩阵并渲染
        for (unsigned int i = 0; i < 10; i++)
        {
            QMatrix4x4 model; // 创建模型矩阵
            model.translate(cubePositions[i]); // 平移到指定位置
            float angle = 20.0f * i; // 每个立方体的旋转角度
            model.rotate(angle, 1.0f, 0.3f, 0.5f); // 旋转立方体

            // 检查模型矩阵 uniform 的位置是否有效
            int modelUniformLocation = shader->uniformLocation("model");
            if (modelUniformLocation == -1) {
                qDebug() << "Uniform 'model' not found in shader!";
                continue; // 跳过此立方体
            }

            // 将模型矩阵传递给着色器
            shader->setUniformValue(modelUniformLocation, model);

            // 绘制立方体（三角形数组，36 个顶点）
            functions->glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // 释放 VAO
        vao.release();

        // 释放纹理
        texture->release();

        // 释放着色器
        shader->release();
    }


private:
    QOpenGLBuffer vbo;
    QOpenGLVertexArrayObject vao;
    QOpenGLFunctions* functions;

    QOpenGLTexture* texture;
};
