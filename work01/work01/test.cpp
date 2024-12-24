#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <iostream>
#include <vector>

#include "Camera.h"

class Shape {
public:
    virtual void draw() = 0;  // ÿ��������Ļ��ƽӿ�
    virtual ~Shape() = default;
};

class Point : public Shape {
private:
    float x, y, z;  // �������
    float r, g, b, a;  // �����ɫ

public:
    Point(float x, float y, float z, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
        : x(x), y(y), z(z), r(r), g(g), b(b), a(a) {
    }

    void draw() override {
        glBegin(GL_POINTS);  // OpenGL ���Ƶ�
        glColor4f(r, g, b, a);  // ������ɫ
        glVertex3f(x, y, z);  // ���Ƶ�
        glEnd();
    }
};

class Line : public Shape {
private:
    float x1, y1, z1, x2, y2, z2;  // �߶ε������˵�
    float r, g, b, a;  // �ߵ���ɫ
    float width;  // �߿�

public:
    Line(float x1, float y1, float z1, float x2, float y2, float z2,
        float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f, float width = 1.0f)
        : x1(x1), y1(y1), z1(z1), x2(x2), y2(y2), z2(z2), r(r), g(g), b(b), a(a), width(width) {
    }

    void draw() override {
        glLineWidth(width);  // �����߿�
        glBegin(GL_LINES);  // OpenGL �����߶�
        glColor4f(r, g, b, a);  // ������ɫ
        glVertex3f(x1, y1, z1);  // ���
        glVertex3f(x2, y2, z2);  // �յ�
        glEnd();
    }
};

class Triangle : public Shape {
private:
    float x1, y1, z1, x2, y2, z2, x3, y3, z3;  // �����ε���������
    float r, g, b, a;  // �����ε���ɫ

public:
    Triangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3,
        float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
        : x1(x1), y1(y1), z1(z1), x2(x2), y2(y2), z2(z2), x3(x3), y3(y3), z3(z3), r(r), g(g), b(b), a(a) {
    }

    void draw() override {
        glBegin(GL_TRIANGLES);  // OpenGL ����������
        glColor4f(r, g, b, a);  // ������ɫ
        glVertex3f(x1, y1, z1);  // ��һ������
        glVertex3f(x2, y2, z2);  // �ڶ�������
        glVertex3f(x3, y3, z3);  // ����������
        glEnd();
    }
};

class ShapeRenderer {
private:
    std::vector<Shape*> shapes;  // �洢������״

public:
    // �����״
    void addShape(Shape* shape) {
        shapes.push_back(shape);
    }

    // ����������״
    void drawShapes() {
        for (Shape* shape : shapes) {
            shape->draw();
        }
    }
};

void setupLighting() {
    // ���ù���
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // ���ù�Դλ��
    GLfloat light_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // ���ù�Դ��ɫ����ɫ�⣩
    GLfloat light_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);

    // ���ò�������
    GLfloat material_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // ���ò��ʵ���������ɫ
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
    GLfloat material_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0f);  // ���ù����
}


Camera camera;

void processKeyboardInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processMovement(Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processMovement(Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processMovement(Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processMovement(Movement::RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.processRotation(Rotate::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.processRotation(Rotate::DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.processRotation(Rotate::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.processRotation(Rotate::RIGHT, deltaTime);
}

int main1() {
    // ��ʼ�� GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // ��������
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Shapes", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();  // ��ʼ�� GLEW

    glEnable(GL_DEPTH_TEST);  // ������Ȳ���

    // �����ӽǺ�ͶӰ����
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0);  // ͸��ͶӰ

    // ���ù���
    setupLighting();

    // ��������ͼ��
    ShapeRenderer renderer;

    // ����������
    Line xAxis(-10.0f, 0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);  // X�᣺��ɫ
    Line yAxis(0.0f, -10.0f, 0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f);  // Y�᣺��ɫ
    Line zAxis(0.0f, 0.0f, -10.0f, 0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 1.0f);  // Z�᣺��ɫ

    // ��������ᵽ��Ⱦ��
    renderer.addShape(&xAxis);
    renderer.addShape(&yAxis);
    renderer.addShape(&zAxis);

    // ������������ͼ��
    //Point p1(5.0f, 5.0f, 1.0f);
    //Line l1(-0.5f, -0.5f, -1.0f, 0.5f, 0.5f, 0.0f);
    //Triangle t1(-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 1.0f);

    //renderer.addShape(&p1);
    //renderer.addShape(&l1);
    //renderer.addShape(&t1);



    // ��Ⱦѭ��
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // ��ջ�����

        // ��ȡʱ��
        float deltaTime = 0.0f;
        float lastFrame = 0.0f;
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processKeyboardInput(window, deltaTime);

        // ������ͼ����
        glm::mat4 view = camera.getViewMatrix();
        glLoadMatrixf(&view[0][0]);  // ������ͼ����

        // ����������״
        renderer.drawShapes();

        glfwSwapBuffers(window);  // ����������
        glfwPollEvents();  // �����¼�
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
