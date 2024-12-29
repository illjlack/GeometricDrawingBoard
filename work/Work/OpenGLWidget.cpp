#include "OpenGLWidget.h"

#include <QTimer>

OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent), shaderProgram(nullptr)
{
    // 设置定时器，每隔一段时间更新一次
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {update(); });
    timer->start(16);  // 大约60FPS
}

OpenGLWidget::~OpenGLWidget()
{
    // 确保当前 OpenGL 上下文有效
    makeCurrent();  

    // 删除着色器程序
    if (shaderProgram) {
        delete shaderProgram;
    }

    doneCurrent();  // 释放当前上下文
}

void OpenGLWidget::pushObject(Object* obj)
{
    obj->setShader(shaderProgram);
    obj->initialize();
    vec.push_back(obj);
}

void OpenGLWidget::initializeGL()
{
    // 初始化 OpenGL 函数
    initializeOpenGLFunctions();

    // 初始化OpenGL环境
    glEnable(GL_DEPTH_TEST);  // 启用深度测试
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  // 设置清除颜色
    
    // 创建和初始化着色器程序
    shaderProgram = new QOpenGLShaderProgram();

    // 顶点着色器源代码
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
        })";

    // 片段着色器源代码
    const char* fragmentShaderSource = R"(
        #version 460 core
        out vec4 FragColor;

        uniform vec3 color;

        void main()
        {
            FragColor = vec4(color, 1.0);
        })";

    // 编译和链接着色器
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    shaderProgram->link();

    // 初始化 VAO 和 VBO
    vao = new  QOpenGLVertexArrayObject(this);
    vao->create();
    vertexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vertexBuffer->create();
}

void OpenGLWidget::resizeGL(int w, int h)
{
    // 更新视口大小
    glViewport(0, 0, w, h);
}

void OpenGLWidget::paintGL()
{
    // 清除颜色缓冲和深度缓冲
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram->bind();

    // 每一帧都画一次，因为没有初始化，第一帧的操作很大，但应该不会在第一帧能操作吧
    float currentFrame = static_cast<float>(frameTimer.elapsed()) / 1000.0f;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    camera.processInput(deltaTime);

    //camera.setProjectionMode(Camera::ProjectionMode::Orthographic);
    // test
    {

        vao->bind();
        vertexBuffer->bind();

        // 数轴的顶点数据，包含 X、Y、Z 轴的线段
        GLfloat axisVertices[] = {
            // X轴
            -10.0f,  0.0f,  0.0f,
             10.0f,  0.0f,  0.0f,

            // Y轴
            0.0f, -10.0f,  0.0f,
            0.0f,  10.0f,  0.0f,

            // Z轴
            0.0f,  0.0f, -10.0f,
            0.0f,  0.0f,  10.0f    
        };

        // 分配和传递数据到 VBO
        vertexBuffer->allocate(axisVertices, sizeof(axisVertices));

        // 设置位置属性
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);  // 位置
        glEnableVertexAttribArray(0);
    
        // 激活着色器程序
        shaderProgram->bind();

        updateCameraMovement();
        
        // 设置颜色（根据需要可以改变颜色）
        shaderProgram->setUniformValue("color", QVector3D(1.0f, 1.0f, 1.0f)); 
        
        // 计算模型矩阵（平移到 position）
        QMatrix4x4 modelMatrix;
        // 传递模型矩阵到着色器
        shaderProgram->setUniformValue("model", modelMatrix);

        // 绘制 XYZ 数轴
        glDrawArrays(GL_LINES, 0, 6);

        vao->release();
        vertexBuffer->release();
    }

    for (auto obj : vec)
    {
        obj->draw();
    }

    shaderProgram->release();
}
 
// 处理按下键盘事件
void OpenGLWidget::keyPressEvent(QKeyEvent* event)
{
    camera.keyPressEvent(event);
}

// 处理释放键盘事件
void OpenGLWidget::keyReleaseEvent(QKeyEvent* event)
{
    camera.keyReleaseEvent(event);
}

// 处理鼠标移动事件
void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    camera.mouseMoveEvent(event);
}

// 处理鼠标按下事件
void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    camera.mousePressEvent(event);
}

// 处理鼠标松开事件
void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    camera.mouseReleaseEvent(event);
}

// 处理滚轮事件
void OpenGLWidget::wheelEvent(QWheelEvent* event)
{
    camera.mouseScrollEvent(event); // 将滚轮事件传递给 Camera 来处理
}

// 更新相机运动
void OpenGLWidget::updateCameraMovement()
{
    QMatrix4x4 view = camera.GetViewMatrix();
    shaderProgram->setUniformValue("view", view);
    QMatrix4x4 projection = camera.GetProjectionMatrix(width(),height());
    shaderProgram->setUniformValue("projection", projection);
}