#include "test_QOpenGLWidget.h"
#include <QtWidgets/QApplication>
#include <qDebug>

int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setVersion(4, 6); // ���� OpenGL �汾Ϊ 4.6
    format.setProfile(QSurfaceFormat::CoreProfile); // ǿ�ƺ���ģʽ
    QSurfaceFormat::setDefaultFormat(format);


    QApplication a(argc, argv);
    test_QOpenGLWidget w;
    w.show();
    return a.exec();
}
