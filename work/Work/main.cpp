#include "WorkWindow.h"
#include <QtWidgets/QApplication>
#include "OpenGLWidget.h"

#include <Point.h>
#include <QThread>

#include "codec.h"

int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setVersion(4, 6); // 设置 OpenGL 版本为 4.6
    format.setProfile(QSurfaceFormat::CoreProfile); // 强制核心模式
    format.setSamples(4); // 设置多重采样等级（例如4x MSAA）
    QSurfaceFormat::setDefaultFormat(format);

    qDebug() << "=============Runtime OpenGL version:" << (const char*)glGetString(GL_VERSION);
    qDebug() << "=============GLSL version:" << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

    QApplication a(argc, argv);
    WorkWindow w;
    w.show();

    return a.exec();
}
