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
    // 设置定时器，每隔一段时间更新一次
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {update(); });
    timer->start(16);  // 大约60FPS

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
    // 自动加载OpenGL函数
    initializeOpenGLFunctions();

    // 初始化OpenGL环境
    glEnable(GL_DEPTH_TEST);  // 启用深度测试
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  // 设置清除颜色


    // 顶点着色器源代码
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

    // 片段着色器源代码
    const char* fragmentShaderSource = R"(
        #version 460 core

        in vec2 TexCoord;
        out vec4 FragColor;

        uniform sampler2D texture;

        void main() {
            FragColor = texture2D(texture, TexCoord);
        }
        )";

    // 编译和链接着色器
    shader->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    shader->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);

    //// 初始化着色器
    //if (!shader->addShaderFromSourceFile(QOpenGLShader::Vertex, "vertex_shader.glsl")) {
    //    qDebug() << "Vertex shader error:" << shader->log();
    //}
    //if (!shader->addShaderFromSourceFile(QOpenGLShader::Fragment, "fragment_shader.glsl")) {
    //    qDebug() << "Fragment shader error:" << shader->log();
    //}
    if (!shader->link()) {
        qDebug() << "Shader program linking error:" << shader->log();
        QApplication::exit(-1);  // 链接失败退出程序
    }
}



void OpenGLWidget::resizeGL(int w, int h)
{
    // 设置视口大小
    glViewport(0, 0, w, h);
}


void OpenGLWidget::paintGL()
{
    // 清除颜色缓冲和深度缓冲
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 计算帧间时间
    float currentFrame = static_cast<float>(frameTimer.elapsed()) / 1000.0f;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // 绑定着色器
    shader->bind();

    // 设置投影矩阵
    QMatrix4x4 projection;
    projection.perspective(45.0f, float(width()) / float(height()), 0.1f, 100.0f);
    shader->setUniformValue("projection", projection);

    // 设置视图矩阵
    QMatrix4x4 view = camera.GetViewMatrix();
    shader->setUniformValue("view", view);

    shader->release();

    // 渲染所有物体
    for (auto obj : vec)
    {
        obj->draw();
    }
}


void OpenGLWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        QApplication::quit(); // 按ESC退出应用
    }
    // 处理其他键盘事件
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    // 处理鼠标移动事件
}

void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    // 处理鼠标按下事件
}
