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
        // ��ȡ��ǰ OpenGL ������
        functions = QOpenGLContext::currentContext()->functions();
        initialize();
    }

    ~CubeRenderer()
    {
        // ������Դ
        vbo.destroy();
        vao.destroy();
    }

    // ��ʼ�����������ɫ��������
    void initialize()
    {
        // ������ɫ��
        shader.addShaderFromSourceFile(QOpenGLShader::Vertex, "vertex_shader.glsl");
        shader.addShaderFromSourceFile(QOpenGLShader::Fragment, "fragment_shader.glsl");
        shader.link();

        // �����嶥������
        GLfloat vertices[] = {
            // λ��                // ��������
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

        // ���ö�������
        shader.bind();
        shader.setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat)); // λ������
        shader.enableAttributeArray(0);
        shader.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat)); // ������������
        shader.enableAttributeArray(1);
        shader.release();

        // ��ʼ������
        texture1 = new QOpenGLTexture(QImage("muxiang.png").mirrored());
        texture2 = new QOpenGLTexture(QImage("huiyuanai.png").mirrored());
    }

    // ��Ⱦ������
    void draw() override
    {
        // ʹ����ɫ��
        shader.bind();

        shader.setUniformValue("texture1", 0);
        shader.setUniformValue("texture2", 1);


        // ���þ���
        // shader.setUniformValue("view", QMatrix4x4(glm::value_ptr(view)));
        // shader.setUniformValue("projection", QMatrix4x4(glm::value_ptr(projection)));

        // ��������
        texture1->bind(0);
        texture2->bind(1);

        // ����ռ���ÿ���������λ��
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

        for (unsigned int i = 0; i < 10; i++) // ��Ⱦ 10 ��������
        {
            // Ϊÿ�����������ģ�;��󲢴��ݸ���ɫ��
            QMatrix4x4 model; // ʹ�� Qt �� QMatrix4x4 ��ʼ��ģ�;���
            model.translate(cubePositions[i]); // ��������ƽ�Ƶ�ָ��λ��
            float angle = 20.0f * i; // ������ת�Ƕȣ�ÿ��������ĽǶȲ�ͬ
            model.rotate(angle, 1.0f, 0.3f, 0.5f); // ��ת������
            shader.setUniformValue("model", model); // ��ģ�;��󴫵ݸ���ɫ��
            
            // ���������壨���������飬36 �����㣩
            functions->glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        vao.release();

        // �ͷ���Դ
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
