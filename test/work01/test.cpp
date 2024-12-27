
#include "Camera.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <iostream>
#include <vector>


class Shape {
public:
    virtual void draw() = 0;  // 每个几何体的绘制接口
    virtual ~Shape() = default;
};

class Point : public Shape {
private:
    float x, y, z;  // 点的坐标
    float r, g, b, a;  // 点的颜色

public:
    Point(float x, float y, float z, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
        : x(x), y(y), z(z), r(r), g(g), b(b), a(a) {
    }

    void draw() override {
        glBegin(GL_POINTS);  // OpenGL 绘制点
        glColor4f(r, g, b, a);  // 设置颜色
        glVertex3f(x, y, z);  // 绘制点
        glEnd();
    }
};

class Line : public Shape {
private:
    float x1, y1, z1, x2, y2, z2;  // 线段的两个端点
    float r, g, b, a;  // 线的颜色
    float width;  // 线宽

public:
    Line(float x1, float y1, float z1, float x2, float y2, float z2,
        float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f, float width = 1.0f)
        : x1(x1), y1(y1), z1(z1), x2(x2), y2(y2), z2(z2), r(r), g(g), b(b), a(a), width(width) {
    }

    void draw() override {
        glLineWidth(width);  // 设置线宽
        glBegin(GL_LINES);  // OpenGL 绘制线段
        glColor4f(r, g, b, a);  // 设置颜色
        glVertex3f(x1, y1, z1);  // 起点
        glVertex3f(x2, y2, z2);  // 终点
        glEnd();
    }
};

class Triangle : public Shape {
private:
    float x1, y1, z1, x2, y2, z2, x3, y3, z3;  // 三角形的三个顶点
    float r, g, b, a;  // 三角形的颜色

public:
    Triangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3,
        float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
        : x1(x1), y1(y1), z1(z1), x2(x2), y2(y2), z2(z2), x3(x3), y3(y3), z3(z3), r(r), g(g), b(b), a(a) {
    }

    void draw() override {
        glBegin(GL_TRIANGLES);  // OpenGL 绘制三角形
        glColor4f(r, g, b, a);  // 设置颜色
        glVertex3f(x1, y1, z1);  // 第一个顶点
        glVertex3f(x2, y2, z2);  // 第二个顶点
        glVertex3f(x3, y3, z3);  // 第三个顶点
        glEnd();
    }
};

class ShapeRenderer {
private:
    std::vector<Shape*> shapes;  // 存储所有形状

public:
    // 添加形状
    void addShape(Shape* shape) {
        shapes.push_back(shape);
    }

    // 绘制所有形状
    void drawShapes() {
        for (Shape* shape : shapes) {
            shape->draw();
        }
    }
};
