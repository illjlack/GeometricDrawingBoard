#pragma once

#include <QtWidgets/QMainWindow>
#include <QDockWidget>
#include "OpenGLWidget.h"  // ����OpenGLWidgetͷ�ļ�

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    void test();
    ~MainWindow();

private:

    // OpenGL widget ������ʾ OpenGL ��Ⱦ
    OpenGLWidget* openGLWidget;

    // ���������
    QDockWidget* sideDockWidget;
};
