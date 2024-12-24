#include <GL/glew.h>
#include <GLFW/glfw3.h>
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

void setupLighting() {
    // ���ù���
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // ���ù�Դλ�ã����ӻ�Ч���Ĺؼ���
    GLfloat light_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // ���ù�Դ��ɫ����ɫ�⣩
    GLfloat light_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);

    // ���ò�������
    GLfloat material_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
    GLfloat material_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0f);  // �����
}

int main() {
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

    // �����ӽǺ�ͶӰ����
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0);  // ͸��ͶӰ
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.0f);  // �ƶ����壬ʹ��λ����Ұ��

    // ���ù���
    setupLighting();

    ShapeRenderer renderer;

    // ��������ͼ��
    Point p1(0.0f, 0.0f, 0.0f);
    Line l1(-0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f);
    Triangle t1(-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f);

    // ������ͼ����ӵ���Ⱦ��
    renderer.addShape(&p1);
    renderer.addShape(&l1);
    renderer.addShape(&t1);

    // ��Ⱦѭ��
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // ��ջ�����

        // ��ȡ����ʼ������ʱ�䣨�룩
        float time = glfwGetTime();

        // ����ʱ�������ת�Ƕȣ���ת�ٶ�Ϊÿ�� 30 ��
        float angle = time * 30.0f;  // ÿ����ת 30 ��

        // �������ù�Դλ�ã�������ģ����ͼ����Ӱ��
        GLfloat light_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);  // ȷ����Դλ�ò�����תӰ��

        // ��ת����
        glPushMatrix();  // ���浱ǰ�ľ���
        glRotatef(angle, 0.0f, 1.0f, 0.0f);  // ��Y����ת����

        // ����������״
        renderer.drawShapes();

        glPopMatrix();  // �ָ�����

        glfwSwapBuffers(window);  // ����������
        glfwPollEvents();  // �����¼�
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
