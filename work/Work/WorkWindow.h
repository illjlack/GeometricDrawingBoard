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
        // OpenGL widget ������ʾ OpenGL ��Ⱦ
        OpenGLWidget* openGLWidget;

        // ���������
        QDockWidget* sideDockWidget;

        void initMap();
    };

}