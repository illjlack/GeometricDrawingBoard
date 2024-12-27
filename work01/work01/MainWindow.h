#pragma once

#include <QtWidgets/QMainWindow>
#include <QDockWidget>
#include "OpenGLWidget.h"  // 包含OpenGLWidget头文件

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    void test();
    ~MainWindow();

private:

    // OpenGL widget 用于显示 OpenGL 渲染
    OpenGLWidget* openGLWidget;

    // 侧边栏操作
    QDockWidget* sideDockWidget;
};
