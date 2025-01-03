#include "mainWindow.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QDockWidget>
#include "comm.h"


mainWindow::mainWindow(QWidget* parent)
    : QMainWindow(parent), canvas(new Canvas(this))
{
     //设置中心控件为 Canvas
     setCentralWidget(canvas);

    // 创建菜单栏、工具栏、侧边栏和状态栏
    createMenuBar();
    createToolBar();
    createStatusBar();
    createSideBar();

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


    connect(drawPointAction, &QAction::triggered, this, [=]() {canvas->setDrawMode(Canvas::DrawMode::DrawPoint); });
    connect(drawLineAction, &QAction::triggered, this, [=]() {canvas->setDrawMode(Canvas::DrawMode::DrawPolyline); });
    //connect(drawAreaAction, &QAction::triggered, this, [=]() {canvas->setDrawMode(Canvas::DrawMode::DrawPolygon); });
    addToolBar(toolBar);
}

void mainWindow::createSideBar() {
    sideBarWidget = new QWidget(this);
    sideBar = new QDockWidget(L("设置"), this);
    sideBar->setWidget(sideBarWidget);
    sideBar->setVisible(true); // 默认显示

    pointSettings = new PointSettings();
    //lineSettings = new LineSettings();

    // 默认显示点设置
    sideBar->setWidget(pointSettings);
    pointSettings->reset();

    addDockWidget(Qt::RightDockWidgetArea, sideBar);
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



// ===================================================================================================================== PointSettings
DrawingSettings::DrawingSettings(QWidget* parent):QWidget(parent)
{
}


PointSettings::PointSettings(QWidget* parent)
    : DrawingSettings(parent), currentColor(Qt::black) {
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* sizeLabel = new QLabel(L("绘制大小"), this);
    layout->addWidget(sizeLabel);

    sizeSlider = new QSlider(Qt::Horizontal, this);
    sizeSlider->setRange(1, 20);
    layout->addWidget(sizeSlider);

    QLabel* colorLabel = new QLabel(L("选择颜色"), this);
    layout->addWidget(colorLabel);

    colorButton = new QPushButton(L("选择颜色"), this);
    layout->addWidget(colorButton);

    connect(colorButton, &QPushButton::clicked, this, &PointSettings::onColorButtonClicked);
}

void PointSettings::reset() {
    sizeSlider->setValue(1); // 重置大小
    colorButton->setStyleSheet("background-color: " + defaultColor.name());
}

void PointSettings::onColorButtonClicked() {
    currentColor = QColorDialog::getColor(currentColor, this, L("选择颜色"));
    if (currentColor.isValid()) {
        colorButton->setStyleSheet("background-color: " + currentColor.name());
    }
}