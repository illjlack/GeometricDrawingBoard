#pragma once

#include<Object.h>
#include<QOpenGLFunctions>
class testObject : public Object, public QObject
{
public:
    void initialize()
    {
        // ��鵱ǰ OpenGL �������Ƿ���Ч
        QOpenGLContext* currentContext = QOpenGLContext::currentContext();
        if (!currentContext) {
            qWarning() << "OpenGL context is not available!";
            return;
        }
        // ��ȡ OpenGL �����ӿ�
        QOpenGLFunctions* functions = currentContext->functions();
        if (!functions) {
            qWarning() << "Failed to get OpenGL functions!";
            return;
        }
        // ��� ShaderProgram �Ƿ��ѳ�ʼ��
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
        // ����λ������
        functions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);  // λ��
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

