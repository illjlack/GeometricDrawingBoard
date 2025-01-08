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
#include "DrawSettings.h"

mainWindow::mainWindow(QWidget* parent)
    : QMainWindow(parent), canvas(new Canvas(this))
{
     //设置中心控件为 Canvas
     setCentralWidget(canvas);

    // 创建菜单栏、工具栏、侧边栏和状态栏
    createMenuBar();
    createToolBar();
    createNodeLineToolBar();
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
    drawPointAction->setCheckable(true); // 设置为可选中
    QAction* drawSimpleLineAction = toolBar->addAction(L("绘制简单线"));
    drawSimpleLineAction->setCheckable(true);
    QAction* drawDoubleLineAction = toolBar->addAction(L("绘制双线"));
    drawDoubleLineAction->setCheckable(true);
    QAction* drawParallelLineAction = toolBar->addAction(L("绘制平行线"));
    drawParallelLineAction->setCheckable(true);
    QAction* drawTwoPointCircleAction = toolBar->addAction(L("绘制两点圆"));
    drawTwoPointCircleAction->setCheckable(true);
    QAction* drawSimpleAreaAction = toolBar->addAction(L("绘制简单面"));
    drawSimpleAreaAction->setCheckable(true);
    QAction* drawComplexAreaAction = toolBar->addAction(L("绘制复杂面"));
    drawComplexAreaAction->setCheckable(true);

    // 将按钮分组，保证只能选中一个按钮
    QActionGroup* actionGroup = new QActionGroup(this);
    actionGroup->addAction(drawPointAction);
    actionGroup->addAction(drawSimpleLineAction);
    actionGroup->addAction(drawDoubleLineAction);
    actionGroup->addAction(drawParallelLineAction);
    actionGroup->addAction(drawTwoPointCircleAction);
    actionGroup->addAction(drawSimpleAreaAction);
    actionGroup->addAction(drawComplexAreaAction);
    actionGroup->setExclusive(true); // 设置为互斥

    // 连接信号和槽
    connect(drawPointAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_DrawMode, DrawMode::DrawPoint); });
    connect(drawSimpleLineAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_DrawMode, DrawMode::DrawSimpleLine); });
    connect(drawDoubleLineAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_DrawMode, DrawMode::DrawDoubleLine); });
    connect(drawParallelLineAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_DrawMode, DrawMode::DrawParallelLine); });
    connect(drawTwoPointCircleAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_DrawMode, DrawMode::DrawTwoPointCircle); });
    connect(drawSimpleAreaAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_DrawMode, DrawMode::DrawSimpleArea); });
    connect(drawComplexAreaAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_DrawMode, DrawMode::DrawComplexArea); });

    addToolBar(toolBar);
}

void mainWindow::createNodeLineToolBar()
{
    QToolBar* nodeLineToolBar = new QToolBar(this);

    // 添加工具栏按钮
    QAction* polylineAction = nodeLineToolBar->addAction(L("折线"));
    polylineAction->setCheckable(true);
    QAction* splineAction = nodeLineToolBar->addAction(L("样条线"));
    splineAction->setCheckable(true);
    QAction* threePointArcAction = nodeLineToolBar->addAction(L("三点圆弧"));
    threePointArcAction->setCheckable(true);
    QAction* arcAction = nodeLineToolBar->addAction(L("圆弧"));
    arcAction->setCheckable(true);
    QAction* streamlineAction = nodeLineToolBar->addAction(L("流线"));
    streamlineAction->setCheckable(true);

    // 将按钮分组，保证只能选中一个按钮
    QActionGroup* actionGroup = new QActionGroup(this);
    actionGroup->addAction(polylineAction);
    actionGroup->addAction(splineAction);
    actionGroup->addAction(threePointArcAction);
    actionGroup->addAction(arcAction);
    actionGroup->addAction(streamlineAction);
    actionGroup->setExclusive(true); // 设置为互斥

    // 默认选中折线按钮
    polylineAction->setChecked(true);

    // 连接信号和槽
    connect(polylineAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_NodeLineStyle, NodeLineStyle::StylePolyline); });
    connect(splineAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_NodeLineStyle, NodeLineStyle::StyleSpline); });
    connect(threePointArcAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_NodeLineStyle, NodeLineStyle::StyleThreePointArc); });
    connect(arcAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_NodeLineStyle, NodeLineStyle::StyleArc); });
    connect(streamlineAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_NodeLineStyle, NodeLineStyle::StyleStreamline); });

    addToolBar(nodeLineToolBar);
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