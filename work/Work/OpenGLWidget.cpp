#include <QTimer>
#include "OpenGLWidget.h"
#include "Point.h"
#include "line.h"
#include "comm.h"

namespace My
{
    OpenGLWidget::OpenGLWidget(QWidget* parent)
        : QOpenGLWidget(parent), shaderProgram(nullptr)
    {
        setFocusPolicy(Qt::StrongFocus);

        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this]() {update(); });
        timer->start(16);  // 大约60FPS
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

        glEnable(GL_DEPTH_TEST);  // 启用深度测试
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  // 设置清除颜色

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

        uniform vec4 color;

        void main()
        {
            FragColor = color;
        })";

        shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
        shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
        shaderProgram->link();

        GLint* maxLineWidth = new GLint;
        glGetIntegerv(GL_LINE_WIDTH_RANGE, maxLineWidth);
        qDebug() << L("最大支持线宽：") << *maxLineWidth;
    }

    void OpenGLWidget::resizeGL(int w, int h)
    {
        glViewport(0, 0, w, h);
    }

    void OpenGLWidget::paintGL()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 处理输入
        processInput();
        // 更新矩阵
        updateCameraMovement();

        test();

        for (auto obj : vec)
        {
            obj->draw();
        }

        // 透明的缓冲区后绘制
        for (auto obj : vec)
        {
            obj->drawBufferZone();
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
            vbo.allocate(axisVertices, sizeof(axisVertices));

            // 设置位置属性
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);  // 位置
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
            shaderProgram->setUniformValue("color", QVector4D(1.0f, 1.0f, 1.0f, 1.0f));
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

    // 更新相机运动
    void OpenGLWidget::updateCameraMovement()
    {
        // 激活着色器程序
        shaderProgram->bind();
        QMatrix4x4 view = camera.GetViewMatrix();
        shaderProgram->setUniformValue("view", view);
        QMatrix4x4 projection = camera.GetProjectionMatrix(width(), height());
        shaderProgram->setUniformValue("projection", projection);
        shaderProgram->release();
    }

    void OpenGLWidget::processInput()
    {
        // 每一帧都画一次，因为没有初始化，第一帧的操作很大，但应该不会在第一帧能操作吧
        float currentFrame = static_cast<float>(frameTimer.elapsed()) / 1000.0f;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        camera.processInput(deltaTime);
    }


    // 处理按下键盘事件
    void OpenGLWidget::keyPressEvent(QKeyEvent* event)
    {
        camera.keyPressEvent(event);
        if (event->key() == Qt::Key_T) // test
        {
            pushObject(new Point({ 0.1,0.1,0.1 }));
            //pushObject(new Point({ 0,0,0 }));
            pushObject(new Point({ 0,0,0 }, Qt::red, Point::Shape::Circle));


            QVector<QVector3D> v3d1{ {0,0,0} ,{ 0.1,0.1,0.1 } };

            QVector<QVector3D> v3d2{ {-0.1,0.1,0.1} ,{-0.1,-0.1,0.1 },{-0.1,-0.1,-0.1} };

            pushObject(new Polyline(v3d1));

            pushObject(new Polyline(v3d2));
        }
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
        if (event->button() == Qt::LeftButton) {
            camera.mousePressEvent(event);
        }
    }

    // 处理鼠标松开事件
    void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
    {
        if (event->button() == Qt::LeftButton) {
            camera.mouseReleaseEvent(event);
        }
    }

    // 处理滚轮事件
    void OpenGLWidget::wheelEvent(QWheelEvent* event)
    {
        camera.mouseScrollEvent(event); // 将滚轮事件传递给 Camera 来处理
    }

    void OpenGLWidget::focusOutEvent(QFocusEvent* event)
    {
        camera.focusOutEvent();
    }

    void OpenGLWidget::setIsOrtho(bool flag)
    {
        if (flag)camera.setProjectionMode(Camera::ProjectionMode::Orthographic);
        else camera.setProjectionMode(Camera::ProjectionMode::Perspective);
    }

}