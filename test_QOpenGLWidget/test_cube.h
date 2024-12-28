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
#include <glm/gtc/type_ptr.hpp>

class CubeRenderer
{
public:
    CubeRenderer()
        : vbo(QOpenGLBuffer::VertexBuffer), vao()
    {
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
        // ��ȡ��ǰ OpenGL ������
        functions = QOpenGLContext::currentContext()->functions();

        // �����嶥�����ݣ������湲����ͬ����������
        GLfloat vertices[] = {
            // λ��               // ��������
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

             0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f
        };

        // ���� VAO �� VBO
        vao.create();
        vbo.create();

        // �� VAO �� VBO
        vao.bind();
        vbo.bind();
        vbo.allocate(vertices, sizeof(vertices));

        // ���ö�������
        shader->bind();
        shader->setAttributeBuffer(0, GL_FLOAT, 0, 3, 6 * sizeof(GLfloat)); // λ������
        shader->enableAttributeArray(0);
        shader->setAttributeBuffer(1, GL_FLOAT, 3, 3, 6 * sizeof(GLfloat)); // ������������
        shader->enableAttributeArray(1);
        shader->release();

        // ��ʼ������
        //texture = new QOpenGLTexture(QImage("huiyuanai.png").mirrored());
    }

    void setShader(QOpenGLShaderProgram* shader)
    {
        this->shader = shader;
    }

    void draw()
    {
        // ȷ����ɫ���󶨳ɹ�
        if (!shader->bind()) {
            qDebug() << "Failed to bind shader!";
            return;
        }

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
        //texture->release();

        // �ͷ���ɫ��
        shader->release();
    }


private:
    QOpenGLBuffer vbo;
    QOpenGLVertexArrayObject vao;
    QOpenGLFunctions* functions;

    QOpenGLTexture* texture;

    QOpenGLShaderProgram* shader;
};
