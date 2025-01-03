#include "mainWindow.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include "comm.h"


mainWindow::mainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 设置中心控件为 Canvas
    // setCentralWidget(canvas);

    // 创建菜单栏、工具栏和状态栏
    createMenuBar();
    createToolBar();
    createStatusBar();

    // 设置窗口标题和大小
    setWindowTitle(L("几何图形绘制与缓冲区分析"));
    resize(1024, 768);
}

mainWindow::~mainWindow()
{

}

void mainWindow::createMenuBar()
{
    QMenuBar* menuBar = new QMenuBar(this);

    // 文件菜单
    QMenu* fileMenu = menuBar->addMenu(L("文件(&F)"));
    QAction* openAction = fileMenu->addAction(L("打开(&O)..."));
    QAction* saveAction = fileMenu->addAction(L("保存(&S)..."));
    QAction* exportAction = fileMenu->addAction(L("导出到 SHP 文件(&E)..."));
    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction(L("退出(&Q)"));

    connect(openAction, &QAction::triggered, this, &mainWindow::openFile);
    connect(saveAction, &QAction::triggered, this, &mainWindow::saveFile);
    connect(exportAction, &QAction::triggered, this, &mainWindow::exportToShp);
    connect(exitAction, &QAction::triggered, this, &mainWindow::close);

    // 帮助菜单
    QMenu* helpMenu = menuBar->addMenu(L("帮助(&H)"));
    QAction* aboutAction = helpMenu->addAction(L("关于(&A)..."));
    connect(aboutAction, &QAction::triggered, this, &mainWindow::showAbout);

    setMenuBar(menuBar);
}

void mainWindow::createToolBar()
{
    QToolBar* toolBar = new QToolBar(this);

    // 添加工具栏按钮
    QAction* drawPointAction = toolBar->addAction(L("绘制点"));
    QAction* drawLineAction = toolBar->addAction(L("绘制线"));
    QAction* drawAreaAction = toolBar->addAction(L("绘制面"));

    //connect(drawPointAction, &QAction::triggered, canvas, &Canvas::setDrawPointMode);
    //connect(drawLineAction, &QAction::triggered, canvas, &Canvas::setDrawLineMode);
    //connect(drawAreaAction, &QAction::triggered, canvas, &Canvas::setDrawAreaMode);

    addToolBar(toolBar);
}

void mainWindow::createStatusBar()
{
    QStatusBar* statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    statusBar->showMessage(L("准备就绪"));
}

void mainWindow::openFile()
{
    //QString fileName = QFileDialog::getOpenFileName(this, L("打开文件"), "", L("JSON 文件 (*.json);;所有文件 (*.*)"));
    //if (!fileName.isEmpty()) {
    //    if (canvas->loadFromFile(fileName)) {
    //        statusBar()->showMessage(L("文件已打开: %1").arg(fileName));
    //    }
    //    else {
    //        QMessageBox::warning(this, L("打开文件"), L("无法打开文件: %1").arg(fileName));
    //    }
    //}
}

void mainWindow::saveFile()
{
    //QString fileName = QFileDialog::getSaveFileName(this, L("保存文件"), "", L("JSON 文件 (*.json);;所有文件 (*.*)"));
    //if (!fileName.isEmpty()) {
    //    if (canvas->saveToFile(fileName)) {
    //        statusBar()->showMessage(L("文件已保存: %1").arg(fileName));
    //    }
    //    else {
    //        QMessageBox::warning(this, L("保存文件"), L("无法保存文件: %1").arg(fileName));
    //    }
    //}
}

void mainWindow::exportToShp()
{
    //QString fileName = QFileDialog::getSaveFileName(this, L("导出到 SHP 文件"), "", L("SHP 文件 (*.shp)"));
    //if (!fileName.isEmpty()) {
    //    if (canvas->exportToShp(fileName)) {
    //        statusBar()->showMessage(L("文件已导出: %1").arg(fileName));
    //    }
    //    else {
    //        QMessageBox::warning(this, L("导出文件"), L("无法导出文件: %1").arg(fileName));
    //    }
    //}
}

void mainWindow::showAbout()
{
    QMessageBox::about(this, L("关于"), L("几何图形绘制与缓冲区分析软件\n\n版本 1.0"));
}
