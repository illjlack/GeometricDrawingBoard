#include "mainWindow.h"
#include <QtWidgets/QApplication>
#include "comm.h"

#include "GeoMathUtil.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    initializeGlobalDrawSettings();

    MainWindow w;
    w.show();

    test();

    return a.exec();
}
