#include "OpenGLWidget.h"
#include <QtWidgets/QApplication>
#include <QTimer>
#include <QTime>
#include <qDebug>

OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent),
    lastFrame(0.0f),
    deltaTime(0.0f)
{
    // ���ö�ʱ����ÿ��һ��ʱ�����һ��
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {update(); });
    timer->start(16);  // ��Լ60FPS

    shader = new QOpenGLShaderProgram();
}

OpenGLWidget::~OpenGLWidget()
{
    delete shader;

    for (auto obj : vec)
    {
       // delete obj;
    }
}

void OpenGLWidget::pushObject(Object* obj)
{
    obj->setShader(shader);
    obj->initialize();
    vec.push_back(obj);
}


void OpenGLWidget::initializeGL()
{
    // �Զ�����OpenGL����
    initializeOpenGLFunctions();

    // ��ʼ��OpenGL����
    glEnable(GL_DEPTH_TEST);  // ������Ȳ���
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  // ���������ɫ


    // ������ɫ��Դ����
    const char* vertexShaderSource = R"(
        #version 460 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;

        out vec2 TexCoord;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
	        gl_Position = projection * view * model * vec4(aPos, 1.0f);
	        TexCoord = vec2(aTexCoord.x, aTexCoord.y);
        })";

    // Ƭ����ɫ��Դ����
    const char* fragmentShaderSource = R"(
        #version 460 core

        in vec2 TexCoord;
        out vec4 FragColor;

        uniform sampler2D texture;

        void main() {
            FragColor = texture2D(texture, TexCoord);
        }
        )";

    // �����������ɫ��
    shader->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    shader->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);

    //// ��ʼ����ɫ��
    //if (!shader->addShaderFromSourceFile(QOpenGLShader::Vertex, "vertex_shader.glsl")) {
    //    qDebug() << "Vertex shader error:" << shader->log();
    //}
    //if (!shader->addShaderFromSourceFile(QOpenGLShader::Fragment, "fragment_shader.glsl")) {
    //    qDebug() << "Fragment shader error:" << shader->log();
    //}
    if (!shader->link()) {
        qDebug() << "Shader program linking error:" << shader->log();
        QApplication::exit(-1);  // ����ʧ���˳�����
    }
}



void OpenGLWidget::resizeGL(int w, int h)
{
    // �����ӿڴ�С
    glViewport(0, 0, w, h);
}


void OpenGLWidget::paintGL()
{
    // �����ɫ�������Ȼ���
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ����֡��ʱ��
    float currentFrame = static_cast<float>(frameTimer.elapsed()) / 1000.0f;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // ����ɫ��
    shader->bind();

    // ����ͶӰ����
    QMatrix4x4 projection;
    projection.perspective(45.0f, float(width()) / float(height()), 0.1f, 100.0f);
    shader->setUniformValue("projection", projection);

    // ������ͼ����
    QMatrix4x4 view = camera.GetViewMatrix();
    shader->setUniformValue("view", view);

    shader->release();

    // ��Ⱦ��������
    for (auto obj : vec)
    {
        obj->draw();
    }
}


void OpenGLWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        QApplication::quit(); // ��ESC�˳�Ӧ��
    }
    // �������������¼�
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    // ��������ƶ��¼�
}

void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    // ������갴���¼�
}
