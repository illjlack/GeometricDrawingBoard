#include "Work.h"
#include <QtWidgets/QApplication>
#include "OpenGLWidget.h"

#include <Point.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Work w;
    OpenGLWidget w;
    w.show();
    QVector3D p = { 0,0,0 };

    w.pushObject(new Point(p,Qt::white,Point::Shape::Circle));

    return a.exec();
}
