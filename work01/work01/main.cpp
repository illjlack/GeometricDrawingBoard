#include <GL/glew.h>
#include <GLFW/glfw3.h>
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

void setupLighting() {
    // 启用光照
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // 设置光源位置（可视化效果的关键）
    GLfloat light_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // 设置光源颜色（白色光）
    GLfloat light_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);

    // 设置材质属性
    GLfloat material_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
    GLfloat material_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0f);  // 光泽度
}

int main() {
    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Shapes", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();  // 初始化 GLEW

    // 设置视角和投影矩阵
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0);  // 透视投影
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.0f);  // 移动物体，使其位于视野中

    // 设置光照
    setupLighting();

    ShapeRenderer renderer;

    // 创建几何图形
    Point p1(0.0f, 0.0f, 0.0f);
    Line l1(-0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f);
    Triangle t1(-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f);

    // 将几何图形添加到渲染器
    renderer.addShape(&p1);
    renderer.addShape(&l1);
    renderer.addShape(&t1);

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 清空缓冲区

        // 获取程序开始以来的时间（秒）
        float time = glfwGetTime();

        // 根据时间控制旋转角度，旋转速度为每秒 30 度
        float angle = time * 30.0f;  // 每秒旋转 30 度

        // 重新设置光源位置，避免受模型视图矩阵影响
        GLfloat light_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);  // 确保光源位置不受旋转影响

        // 旋转物体
        glPushMatrix();  // 保存当前的矩阵
        glRotatef(angle, 0.0f, 1.0f, 0.0f);  // 绕Y轴旋转物体

        // 绘制所有形状
        renderer.drawShapes();

        glPopMatrix();  // 恢复矩阵

        glfwSwapBuffers(window);  // 交换缓冲区
        glfwPollEvents();  // 处理事件
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
