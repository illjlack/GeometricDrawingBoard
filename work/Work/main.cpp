#include "WorkWindow.h"
#include <QtWidgets/QApplication>
#include "OpenGLWidget.h"

#include <Point.h>
#include <QThread>

#include "codec.h"

int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setVersion(4, 6); // ���� OpenGL �汾Ϊ 4.6
    format.setProfile(QSurfaceFormat::CoreProfile); // ǿ�ƺ���ģʽ
    format.setSamples(4); // ���ö��ز����ȼ�������4x MSAA��
    QSurfaceFormat::setDefaultFormat(format);

    qDebug() << "=============Runtime OpenGL version:" << (const char*)glGetString(GL_VERSION);
    qDebug() << "=============GLSL version:" << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

    QApplication a(argc, argv);
    WorkWindow w;
    w.show();

    return a.exec();
}
