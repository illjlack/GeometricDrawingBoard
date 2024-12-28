#include "MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setVersion(4, 6); // 设置 OpenGL 版本为 4.6
    format.setProfile(QSurfaceFormat::CoreProfile); // 强制核心模式
    QSurfaceFormat::setDefaultFormat(format);


    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
