#include "test_QOpenGLWidget.h"

test_QOpenGLWidget::test_QOpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent), vbo(QOpenGLBuffer::VertexBuffer), vao(this), shaderProgram(nullptr)
{
}

test_QOpenGLWidget::~test_QOpenGLWidget()
{
    makeCurrent();  // ȷ����ǰ OpenGL ��������Ч

    // ɾ�� VBO �� VAO
    vbo.destroy();
    vao.destroy();

    // ɾ����ɫ������
    if (shaderProgram) {
        delete shaderProgram;
    }

    doneCurrent();  // �ͷŵ�ǰ������
}

void test_QOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();  // ��ʼ�� OpenGL ����

    // �����ͳ�ʼ����ɫ������
    shaderProgram = new QOpenGLShaderProgram();

    // ������ɫ��Դ����
    const char* vertexShaderSource = R"(
        #version 460 core
        layout(location = 0) in vec3 position;
        layout(location = 1) in vec3 color;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec3 vertexColor;
        void main()
        {
            gl_Position = projection * view * model * vec4(position, 1.0);
            vertexColor = color;
        })";

    // Ƭ����ɫ��Դ����
    const char* fragmentShaderSource = R"(
        #version 460 core
        in vec3 vertexColor;
        out vec4 FragColor;
        void main()
        {
            FragColor = vec4(vertexColor, 1.0);
        })";

    // �����������ɫ��
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    shaderProgram->link();

    cube.setShader(shaderProgram);
    cube.initialize();
}

void test_QOpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);  // �����ӿڴ�С
}

void test_QOpenGLWidget::paintGL()
{


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // ���ñ�����ɫ
    glClear(GL_COLOR_BUFFER_BIT);  // �����ɫ����


    shaderProgram->bind();

    // ������ͼ����ʹ�� QMatrix4x4 �� lookAt ����
    QMatrix4x4 view;
    view.lookAt(
        QVector3D(0.0f, 0.0f, 5.0f),    // �����λ��
        QVector3D(0.0f, 0.0f, 0.0f),    // Ŀ��λ��
        QVector3D(0.0f, 1.0f, 0.0f)     // ���Ϸ���
    );
    // ����ͼ���󴫵ݸ���ɫ��
    shaderProgram->setUniformValue("view", view);

    QMatrix4x4 projection;
    projection.perspective(45.0f, 1.0f, 0.1f, 100.0f);
    shaderProgram->setUniformValue("projection", projection);

    shaderProgram->release();

    cube.draw();
}


