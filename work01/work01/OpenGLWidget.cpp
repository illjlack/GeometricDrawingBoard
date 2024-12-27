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
}

void OpenGLWidget::pushObject(Object* obj)
{
    obj->setShader(shader);
    obj->initialize();
    vec.push_back(obj);
}

void OpenGLWidget::paintEvent(QPaintEvent* event)
{
    // ��ȡ��ǰʱ��ͼ���֡��ʱ��
    float currentFrame = static_cast<float>(QTime::currentTime().msecsSinceStartOfDay()) / 1000.0f;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // ��Ⱦ����
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // �����ɫ����Ȼ���

    // �����ӿ�
    glViewport(0, 0, width(), height());  // ��Ӧ���ڴ�С

    // ͶӰ�������ͼ�������ã�����������
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width() / (float)height(), 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix(); // ������һ�� Camera ����������ͼ����
    glm::mat4 model = glm::mat4(1.0f);  // �����ģ�;�������б任��

    // �����������壬����
    for (auto obj : vec)
    {
        obj->draw();  // ����Ļ��ƺ���
    }

    // ��� OpenGL ����
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        qDebug() << "OpenGL error:" << error;
    }
}


void OpenGLWidget::initializeGL()
{
    // �Զ����� OpenGL ����
    initializeOpenGLFunctions();


    // OpenGL ������ʼ��
    glEnable(GL_DEPTH_TEST);  // ������Ȳ���
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  // ���������ɫ

    shader = new QOpenGLShaderProgram;
    
    if (!shader->addShaderFromSourceFile(QOpenGLShader::Vertex, "vertex_shader.glsl")) {
        qDebug() << "Vertex shader error:" << shader->log();
    }
    if (!shader->addShaderFromSourceFile(QOpenGLShader::Fragment, "fragment_shader.glsl")) {
        qDebug() << "Fragment shader error:" << shader->log();
    }
    if (!shader->link()) {
        qDebug() << "Shader program linking error:" << shader->log();
    }

    qDebug() << "OpenGL initialized successfully!";
}


void OpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h); // �����ӿ�

    QMatrix4x4 projection;
    projection.perspective(45.0f, float(w) / float(h), 0.1f, 100.0f);  // ����͸��ͶӰ
    shader->setUniformValue("projection", projection);
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // �����ɫ�������Ȼ���
    // �ڴ˴�����OpenGL����
    // ���磺��Ⱦһ�������塢�������������ͼ��
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
