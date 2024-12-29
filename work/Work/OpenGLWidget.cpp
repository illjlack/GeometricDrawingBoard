#include "OpenGLWidget.h"

#include <QTimer>

OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent), shaderProgram(nullptr)
{
    // ���ö�ʱ����ÿ��һ��ʱ�����һ��
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {update(); });
    timer->start(16);  // ��Լ60FPS
}

OpenGLWidget::~OpenGLWidget()
{
    // ȷ����ǰ OpenGL ��������Ч
    makeCurrent();  

    // ɾ����ɫ������
    if (shaderProgram) {
        delete shaderProgram;
    }

    doneCurrent();  // �ͷŵ�ǰ������
}

void OpenGLWidget::pushObject(Object* obj)
{
    obj->setShader(shaderProgram);
    obj->initialize();
    vec.push_back(obj);
}

void OpenGLWidget::initializeGL()
{
    // ��ʼ�� OpenGL ����
    initializeOpenGLFunctions();

    // ��ʼ��OpenGL����
    glEnable(GL_DEPTH_TEST);  // ������Ȳ���
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  // ���������ɫ
    
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
        })";

    // Ƭ����ɫ��Դ����
    const char* fragmentShaderSource = R"(
        #version 460 core
        out vec4 FragColor;

        uniform vec3 color;

        void main()
        {
            FragColor = vec4(color, 1.0);
        })";

    // �����������ɫ��
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    shaderProgram->link();

    // ��ʼ�� VAO �� VBO
    vao = new  QOpenGLVertexArrayObject(this);
    vao->create();
    vertexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vertexBuffer->create();
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

    shaderProgram->bind();

    // ÿһ֡����һ�Σ���Ϊû�г�ʼ������һ֡�Ĳ����ܴ󣬵�Ӧ�ò����ڵ�һ֡�ܲ�����
    float currentFrame = static_cast<float>(frameTimer.elapsed()) / 1000.0f;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    camera.processInput(deltaTime);

    //camera.setProjectionMode(Camera::ProjectionMode::Orthographic);
    // test
    {

        vao->bind();
        vertexBuffer->bind();

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
        vertexBuffer->allocate(axisVertices, sizeof(axisVertices));

        // ����λ������
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);  // λ��
        glEnableVertexAttribArray(0);
    
        // ������ɫ������
        shaderProgram->bind();

        updateCameraMovement();
        
        // ������ɫ��������Ҫ���Ըı���ɫ��
        shaderProgram->setUniformValue("color", QVector3D(1.0f, 1.0f, 1.0f)); 
        
        // ����ģ�;���ƽ�Ƶ� position��
        QMatrix4x4 modelMatrix;
        // ����ģ�;�����ɫ��
        shaderProgram->setUniformValue("model", modelMatrix);

        // ���� XYZ ����
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
 
// �����¼����¼�
void OpenGLWidget::keyPressEvent(QKeyEvent* event)
{
    camera.keyPressEvent(event);
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

// ��������˶�
void OpenGLWidget::updateCameraMovement()
{
    QMatrix4x4 view = camera.GetViewMatrix();
    shaderProgram->setUniformValue("view", view);
    QMatrix4x4 projection = camera.GetProjectionMatrix(width(),height());
    shaderProgram->setUniformValue("projection", projection);
}