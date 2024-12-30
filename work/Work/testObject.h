#pragma once

#include<Object.h>
#include<QOpenGLFunctions>
class testObject : public Object, public QObject
{
public:
    void initialize()
    {
        // 检查当前 OpenGL 上下文是否有效
        QOpenGLContext* currentContext = QOpenGLContext::currentContext();
        if (!currentContext) {
            qWarning() << "OpenGL context is not available!";
            return;
        }
        // 获取 OpenGL 函数接口
        QOpenGLFunctions* functions = currentContext->functions();
        if (!functions) {
            qWarning() << "Failed to get OpenGL functions!";
            return;
        }
        // 检查 ShaderProgram 是否已初始化
        if (!shaderProgram || !shaderProgram->isLinked()) {
            qWarning() << "Shader program is not initialized or linked!";
            return;
        }

        shaderProgram->bind();
        vao.create();
        vbo.create();

        vao.bind();
        vbo.bind();
        GLfloat squareVertices[] = {
            -0.5f,  0.5f, 0.0f,
            0.5f,  0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,

            0.5f,  0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f
        };
        vbo.allocate(squareVertices, sizeof(squareVertices));
        // 设置位置属性
        functions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);  // 位置
        functions->glEnableVertexAttribArray(0);
        vao.release();
        vbo.release();
        shaderProgram->release();
        
    }

    void draw()
    {
        auto initialize = [&]()
        {
            
        };

        auto draw = [&]()
        {
            QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

            shaderProgram->bind();
            vao.bind();
            vbo.bind();
            shaderProgram->setUniformValue("color", QVector3D(1.0f, 1.0f, 1.0f));
            QMatrix4x4 modelMatrix;
            shaderProgram->setUniformValue("model", modelMatrix);
            functions->glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
            vao.release();
            vbo.release();
            shaderProgram->release();
        };

        if (!flag)
        {
            initialize();
            flag = true;
        }
        draw();
    }

    bool flag = false;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
};

