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
        // ��ȡ��ǰ OpenGL ������
        functions = QOpenGLContext::currentContext()->functions();
    }

    ~CubeRenderer()
    {
        // ������Դ
        vbo.destroy();
        vao.destroy();
        delete texture;
    }

    // ��ʼ�����������ɫ��������
    void initialize()
    {
        // �����嶥�����ݣ������湲����ͬ����������
        GLfloat vertices[] = {
            // λ��               // ��������
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

        // ���� VAO �� VBO
        vao.create();
        vbo.create();

        // �� VAO �� VBO
        vao.bind();
        vbo.bind();
        vbo.allocate(vertices, sizeof(vertices));

        // λ������(��һ���������Ƕ�����ɫ�����location = 0��ʾ��ɫ������3������������������ float������Ҫ��һ��������ֵ��ƫ��)
        functions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        functions->glEnableVertexAttribArray(0);

        functions->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        functions->glEnableVertexAttribArray(1);


        vao.release();
        vbo.release();

        // ��ʼ������
        texture = new QOpenGLTexture(QImage("huiyuanai.png").mirrored());
    }

    void setShader(QOpenGLShaderProgram* shader) override
    {
        this->shader = shader;
    }

    void draw() override
    {
        // ȷ����ɫ���󶨳ɹ�
        if (!shader->bind()) {
            qDebug() << "Failed to bind shader!";
            return;
        }

        // ���������
        texture->bind(0);

        // ������� uniform ��λ���Ƿ���Ч
        int textureUniformLocation = shader->uniformLocation("texture");
        if (textureUniformLocation == -1) {
            qDebug() << "Uniform 'texture1' not found in shader!";
            return;
        }

        // ��������Ԫ
        shader->setUniformValue(textureUniformLocation, 0);

        // ��������ռ���ÿ���������λ��
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

        // ȷ�� VAO �󶨳ɹ�
        vao.bind();

        // ����ÿ�������壬����ģ�;�����Ⱦ
        for (unsigned int i = 0; i < 10; i++)
        {
            QMatrix4x4 model; // ����ģ�;���
            model.translate(cubePositions[i]); // ƽ�Ƶ�ָ��λ��
            float angle = 20.0f * i; // ÿ�����������ת�Ƕ�
            model.rotate(angle, 1.0f, 0.3f, 0.5f); // ��ת������

            // ���ģ�;��� uniform ��λ���Ƿ���Ч
            int modelUniformLocation = shader->uniformLocation("model");
            if (modelUniformLocation == -1) {
                qDebug() << "Uniform 'model' not found in shader!";
                continue; // ������������
            }

            // ��ģ�;��󴫵ݸ���ɫ��
            shader->setUniformValue(modelUniformLocation, model);

            // ���������壨���������飬36 �����㣩
            functions->glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // �ͷ� VAO
        vao.release();

        // �ͷ�����
        texture->release();

        // �ͷ���ɫ��
        shader->release();
    }


private:
    QOpenGLBuffer vbo;
    QOpenGLVertexArrayObject vao;
    QOpenGLFunctions* functions;

    QOpenGLTexture* texture;
};
