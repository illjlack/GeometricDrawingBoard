#include "test_QOpenGLWidget.h"

test_QOpenGLWidget::test_QOpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent), vbo(QOpenGLBuffer::VertexBuffer), vao(this), shaderProgram(nullptr)
{
}

test_QOpenGLWidget::~test_QOpenGLWidget()
{
    makeCurrent();  // 确保当前 OpenGL 上下文有效

    // 删除 VBO 和 VAO
    vbo.destroy();
    vao.destroy();

    // 删除着色器程序
    if (shaderProgram) {
        delete shaderProgram;
    }

    doneCurrent();  // 释放当前上下文
}

void test_QOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();  // 初始化 OpenGL 函数

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
            vertexColor = color;
        })";

    // 片段着色器源代码
    const char* fragmentShaderSource = R"(
        #version 460 core
        in vec3 vertexColor;
        out vec4 FragColor;
        void main()
        {
            FragColor = vec4(vertexColor, 1.0);
        })";

    // 编译和链接着色器
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    shaderProgram->link();

    cube.setShader(shaderProgram);
    cube.initialize();
}

void test_QOpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);  // 更新视口大小
}

void test_QOpenGLWidget::paintGL()
{


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 设置背景颜色
    glClear(GL_COLOR_BUFFER_BIT);  // 清空颜色缓存


    shaderProgram->bind();

    // 创建视图矩阵，使用 QMatrix4x4 的 lookAt 方法
    QMatrix4x4 view;
    view.lookAt(
        QVector3D(0.0f, 0.0f, 5.0f),    // 摄像机位置
        QVector3D(0.0f, 0.0f, 0.0f),    // 目标位置
        QVector3D(0.0f, 1.0f, 0.0f)     // 向上方向
    );
    // 将视图矩阵传递给着色器
    shaderProgram->setUniformValue("view", view);

    QMatrix4x4 projection;
    projection.perspective(45.0f, 1.0f, 0.1f, 100.0f);
    shaderProgram->setUniformValue("projection", projection);

    shaderProgram->release();

    cube.draw();
}


