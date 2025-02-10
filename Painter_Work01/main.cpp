#include "mainWindow.h"
#include <QtWidgets/QApplication>
#include "comm.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    initializeGlobalDrawSettings();

    MainWindow w;
    w.show();

    return a.exec();
}
