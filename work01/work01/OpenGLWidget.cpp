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
}

void OpenGLWidget::pushObject(Object* obj)
{
    obj->setShader(shader);
    obj->initialize();
    vec.push_back(obj);
}

void OpenGLWidget::paintEvent(QPaintEvent* event)
{
    // 获取当前时间和计算帧间时间
    float currentFrame = static_cast<float>(QTime::currentTime().msecsSinceStartOfDay()) / 1000.0f;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // 渲染过程
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除颜色和深度缓冲

    // 设置视口
    glViewport(0, 0, width(), height());  // 适应窗口大小

    // 投影矩阵和视图矩阵设置（如果有相机）
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width() / (float)height(), 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix(); // 假设有一个 Camera 类来管理视图矩阵
    glm::mat4 model = glm::mat4(1.0f);  // 物体的模型矩阵（如果有变换）

    // 遍历所有物体，绘制
    for (auto obj : vec)
    {
        obj->draw();  // 物体的绘制函数
    }

    // 检查 OpenGL 错误
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        qDebug() << "OpenGL error:" << error;
    }
}


void OpenGLWidget::initializeGL()
{
    // 自动加载 OpenGL 函数
    initializeOpenGLFunctions();


    // OpenGL 环境初始化
    glEnable(GL_DEPTH_TEST);  // 启用深度测试
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  // 设置清除颜色

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
    glViewport(0, 0, w, h); // 设置视口

    QMatrix4x4 projection;
    projection.perspective(45.0f, float(w) / float(h), 0.1f, 100.0f);  // 设置透视投影
    shader->setUniformValue("projection", projection);
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除颜色缓冲和深度缓冲
    // 在此处绘制OpenGL场景
    // 例如：渲染一个立方体、纹理或其他几何图形
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
