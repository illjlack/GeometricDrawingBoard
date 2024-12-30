#include "OpenGLWidget.h"

#include <QTimer>
#include <Point.h>
#include <testObject.h>

#include <QThread>

OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent), shaderProgram(nullptr)
{
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {update(); });
    timer->start(16);  // ��Լ60FPS
}

OpenGLWidget::~OpenGLWidget()
{
    makeCurrent();  

    if (shaderProgram) {
        delete shaderProgram;
    }
    doneCurrent();
}

void OpenGLWidget::pushObject(Object* obj)
{
    makeCurrent();
    obj->setShader(shaderProgram);
    obj->initialize();
    vec.push_back(obj);
    doneCurrent();
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);  // ������Ȳ���
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  // ���������ɫ
    
    shaderProgram = new QOpenGLShaderProgram();

    const char* vertexShaderSource = R"(
        #version 460 core
        layout(location = 0) in vec3 position;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec3 vertexColor;
        void main()
        {
            gl_Position = projection * view * model * vec4(position, 1.0);
        })";

    const char* fragmentShaderSource = R"(
        #version 460 core
        out vec4 FragColor;

        uniform vec3 color;

        void main()
        {
            FragColor = vec4(color, 1.0);
        })";

    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    shaderProgram->link();
}

void OpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ��������
    processInput();
    // ���¾���
    updateCameraMovement();

    test();

    for (auto obj : vec)
    {        
        obj->draw();
    }
}
 
void OpenGLWidget::test()
{
    
    static QOpenGLVertexArrayObject vao;
    static QOpenGLBuffer vbo;
    auto initialize = [&]()
    {
        QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

        vao.create();
        vbo.create();

        shaderProgram->bind();
        vao.bind();
        vbo.bind();
        // ����Ķ������ݣ����� X��Y��Z ����߶�
        GLfloat axisVertices[] = {
            // X��
            -10.0f,  0.0f,  0.0f,
             10.0f,  0.0f,  0.0f,

             // Y��
             0.0f, -10.0f,  0.0f,
             0.0f,  10.0f,  0.0f,

             // Z��
             0.0f,  0.0f, -10.0f,
             0.0f,  0.0f,  10.0f
        };

        // ����ʹ������ݵ� VBO
        vbo.allocate(axisVertices, sizeof(axisVertices));

        // ����λ������
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);  // λ��
        glEnableVertexAttribArray(0);
        vao.release();
        vbo.release();
        shaderProgram->release();
    };

    auto draw = [&]()
    {
        QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();
        
        shaderProgram->bind();
        vao.bind();
        vbo.bind();
        shaderProgram->setUniformValue("color", QVector3D(1.0f, 1.0f, 1.0f));
        QMatrix4x4 modelMatrix;
        shaderProgram->setUniformValue("model", modelMatrix);
        functions->glDrawArrays(GL_LINES, 0, 6);
        vao.release();
        vbo.release();
        shaderProgram->release();
    };

    static bool flag = false;
    if (!flag)
    {
        initialize();
        flag = true;
    }
    draw();
}

// ��������˶�
void OpenGLWidget::updateCameraMovement()
{
    // ������ɫ������
    shaderProgram->bind();
    QMatrix4x4 view = camera.GetViewMatrix();
    shaderProgram->setUniformValue("view", view);
    QMatrix4x4 projection = camera.GetProjectionMatrix(width(), height());
    shaderProgram->setUniformValue("projection", projection);
    shaderProgram->release();
}

void OpenGLWidget::processInput()
{
    // ÿһ֡����һ�Σ���Ϊû�г�ʼ������һ֡�Ĳ����ܴ󣬵�Ӧ�ò����ڵ�һ֡�ܲ�����
    float currentFrame = static_cast<float>(frameTimer.elapsed()) / 1000.0f;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    camera.processInput(deltaTime);
}


// �����¼����¼�
void OpenGLWidget::keyPressEvent(QKeyEvent* event)
{
   camera.keyPressEvent(event);
    if (event->key() == Qt::Key_T) // test
    {
        pushObject(new Point({ 0.1,0.1,0.1 }));
        //pushObject(new Point({ 0,0,0 }));
        pushObject(new Point({ 0,0,0 }, Qt::red, Point::Shape::Circle));
    }
}

// �����ͷż����¼�
void OpenGLWidget::keyReleaseEvent(QKeyEvent* event)
{
    camera.keyReleaseEvent(event);
}

// ��������ƶ��¼�
void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    camera.mouseMoveEvent(event);
}

// ������갴���¼�
void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    camera.mousePressEvent(event);
}

// ��������ɿ��¼�
void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    camera.mouseReleaseEvent(event);
}

// ��������¼�
void OpenGLWidget::wheelEvent(QWheelEvent* event)
{
    camera.mouseScrollEvent(event); // �������¼����ݸ� Camera ������
}

