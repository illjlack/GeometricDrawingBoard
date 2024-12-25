
#include "Camera.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <iostream>
#include <vector>


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
