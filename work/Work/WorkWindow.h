#pragma once

#include <QtWidgets/QMainWindow>
#include <QDockWidget>
#include "OpenGLWidget.h" 

namespace My
{
    class WorkWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        WorkWindow(QMainWindow* parent = nullptr);
        ~WorkWindow();

        std::map<QString, std::map<QString, const char*>> mp;
    private:
        // OpenGL widget 用于显示 OpenGL 渲染
        OpenGLWidget* openGLWidget;

        // 侧边栏操作
        QDockWidget* sideDockWidget;

        void initMap();
    };

}