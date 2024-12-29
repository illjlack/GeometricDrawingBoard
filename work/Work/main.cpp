#include "Work.h"
#include <QtWidgets/QApplication>
#include "OpenGLWidget.h"

#include <Point.h>

int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setVersion(4, 6); // ���� OpenGL �汾Ϊ 4.6
    format.setProfile(QSurfaceFormat::CoreProfile); // ǿ�ƺ���ģʽ
    format.setSamples(4); // ���ö��ز����ȼ�������4x MSAA��
    QSurfaceFormat::setDefaultFormat(format);



    QApplication a(argc, argv);
    // Work w;
    OpenGLWidget w;
    w.show();
    QVector3D p = { 0,0,0 };

    QVector3D pp = { 1,1,1 };

    w.pushObject(new Point(p,Qt::white,Point::Shape::Circle));
    w.pushObject(new Point(pp, Qt::white, Point::Shape::Square));

    return a.exec();
}
