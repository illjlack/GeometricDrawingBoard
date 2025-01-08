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
     //�������Ŀؼ�Ϊ Canvas
     setCentralWidget(canvas);

    // �����˵��������������������״̬��
    createMenuBar();
    createToolBar();
    createNodeLineToolBar();
    createStatusBar();
    createSideBar();
    

    // ���ô��ڱ���ʹ�С
    setWindowTitle(L("����ͼ�λ����뻺��������"));
    resize(1024, 768);
}

mainWindow::~mainWindow()
{

}

void mainWindow::createMenuBar()
{
    QMenuBar* menuBar = new QMenuBar(this);

    // �ļ��˵�
    QMenu* fileMenu = menuBar->addMenu(L("�ļ�(&F)"));
    QAction* openAction = fileMenu->addAction(L("��(&O)..."));
    QAction* saveAction = fileMenu->addAction(L("����(&S)..."));
    QAction* exportAction = fileMenu->addAction(L("������ SHP �ļ�(&E)..."));
    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction(L("�˳�(&Q)"));

    connect(openAction, &QAction::triggered, this, &mainWindow::openFile);
    connect(saveAction, &QAction::triggered, this, &mainWindow::saveFile);
    connect(exportAction, &QAction::triggered, this, &mainWindow::exportToShp);
    connect(exitAction, &QAction::triggered, this, &mainWindow::close);

    // �����˵�
    QMenu* helpMenu = menuBar->addMenu(L("����(&H)"));
    QAction* aboutAction = helpMenu->addAction(L("����(&A)..."));
    connect(aboutAction, &QAction::triggered, this, &mainWindow::showAbout);

    setMenuBar(menuBar);
}

void mainWindow::createToolBar()
{
    QToolBar* toolBar = new QToolBar(this);

    // ��ӹ�������ť
    QAction* drawPointAction = toolBar->addAction(L("���Ƶ�"));
    drawPointAction->setCheckable(true); // ����Ϊ��ѡ��
    QAction* drawSimpleLineAction = toolBar->addAction(L("���Ƽ���"));
    drawSimpleLineAction->setCheckable(true);
    QAction* drawDoubleLineAction = toolBar->addAction(L("����˫��"));
    drawDoubleLineAction->setCheckable(true);
    QAction* drawParallelLineAction = toolBar->addAction(L("����ƽ����"));
    drawParallelLineAction->setCheckable(true);
    QAction* drawTwoPointCircleAction = toolBar->addAction(L("��������Բ"));
    drawTwoPointCircleAction->setCheckable(true);
    QAction* drawSimpleAreaAction = toolBar->addAction(L("���Ƽ���"));
    drawSimpleAreaAction->setCheckable(true);
    QAction* drawComplexAreaAction = toolBar->addAction(L("���Ƹ�����"));
    drawComplexAreaAction->setCheckable(true);

    // ����ť���飬��ֻ֤��ѡ��һ����ť
    QActionGroup* actionGroup = new QActionGroup(this);
    actionGroup->addAction(drawPointAction);
    actionGroup->addAction(drawSimpleLineAction);
    actionGroup->addAction(drawDoubleLineAction);
    actionGroup->addAction(drawParallelLineAction);
    actionGroup->addAction(drawTwoPointCircleAction);
    actionGroup->addAction(drawSimpleAreaAction);
    actionGroup->addAction(drawComplexAreaAction);
    actionGroup->setExclusive(true); // ����Ϊ����

    // �����źźͲ�
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

    // ��ӹ�������ť
    QAction* polylineAction = nodeLineToolBar->addAction(L("����"));
    polylineAction->setCheckable(true);
    QAction* splineAction = nodeLineToolBar->addAction(L("������"));
    splineAction->setCheckable(true);
    QAction* threePointArcAction = nodeLineToolBar->addAction(L("����Բ��"));
    threePointArcAction->setCheckable(true);
    QAction* arcAction = nodeLineToolBar->addAction(L("Բ��"));
    arcAction->setCheckable(true);
    QAction* streamlineAction = nodeLineToolBar->addAction(L("����"));
    streamlineAction->setCheckable(true);

    // ����ť���飬��ֻ֤��ѡ��һ����ť
    QActionGroup* actionGroup = new QActionGroup(this);
    actionGroup->addAction(polylineAction);
    actionGroup->addAction(splineAction);
    actionGroup->addAction(threePointArcAction);
    actionGroup->addAction(arcAction);
    actionGroup->addAction(streamlineAction);
    actionGroup->setExclusive(true); // ����Ϊ����

    // Ĭ��ѡ�����߰�ť
    polylineAction->setChecked(true);

    // �����źźͲ�
    connect(polylineAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_NodeLineStyle, NodeLineStyle::StylePolyline); });
    connect(splineAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_NodeLineStyle, NodeLineStyle::StyleSpline); });
    connect(threePointArcAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_NodeLineStyle, NodeLineStyle::StyleThreePointArc); });
    connect(arcAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_NodeLineStyle, NodeLineStyle::StyleArc); });
    connect(streamlineAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_NodeLineStyle, NodeLineStyle::StyleStreamline); });

    addToolBar(nodeLineToolBar);
}



void mainWindow::createSideBar() {
    sideBarWidget = new QWidget(this);
    sideBar = new QDockWidget(L("����"), this);
    sideBar->setWidget(sideBarWidget);
    sideBar->setVisible(true); // Ĭ����ʾ

    pointSettings = new PointSettings();
    //lineSettings = new LineSettings();

    // Ĭ����ʾ������
    sideBar->setWidget(pointSettings);
    pointSettings->reset();

    addDockWidget(Qt::RightDockWidgetArea, sideBar);
}

void mainWindow::createStatusBar()
{
    QStatusBar* statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    statusBar->showMessage(L("׼������"));
}

void mainWindow::openFile()
{
    //QString fileName = QFileDialog::getOpenFileName(this, L("���ļ�"), "", L("JSON �ļ� (*.json);;�����ļ� (*.*)"));
    //if (!fileName.isEmpty()) {
    //    if (canvas->loadFromFile(fileName)) {
    //        statusBar()->showMessage(L("�ļ��Ѵ�: %1").arg(fileName));
    //    }
    //    else {
    //        QMessageBox::warning(this, L("���ļ�"), L("�޷����ļ�: %1").arg(fileName));
    //    }
    //}
}

void mainWindow::saveFile()
{
    //QString fileName = QFileDialog::getSaveFileName(this, L("�����ļ�"), "", L("JSON �ļ� (*.json);;�����ļ� (*.*)"));
    //if (!fileName.isEmpty()) {
    //    if (canvas->saveToFile(fileName)) {
    //        statusBar()->showMessage(L("�ļ��ѱ���: %1").arg(fileName));
    //    }
    //    else {
    //        QMessageBox::warning(this, L("�����ļ�"), L("�޷������ļ�: %1").arg(fileName));
    //    }
    //}
}

void mainWindow::exportToShp()
{
    //QString fileName = QFileDialog::getSaveFileName(this, L("������ SHP �ļ�"), "", L("SHP �ļ� (*.shp)"));
    //if (!fileName.isEmpty()) {
    //    if (canvas->exportToShp(fileName)) {
    //        statusBar()->showMessage(L("�ļ��ѵ���: %1").arg(fileName));
    //    }
    //    else {
    //        QMessageBox::warning(this, L("�����ļ�"), L("�޷������ļ�: %1").arg(fileName));
    //    }
    //}
}

void mainWindow::showAbout()
{
    QMessageBox::about(this, L("����"), L("����ͼ�λ����뻺�����������\n\n�汾 1.0"));
}



// ===================================================================================================================== PointSettings
DrawingSettings::DrawingSettings(QWidget* parent):QWidget(parent)
{
}


PointSettings::PointSettings(QWidget* parent)
    : DrawingSettings(parent), currentColor(Qt::black) {
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* sizeLabel = new QLabel(L("���ƴ�С"), this);
    layout->addWidget(sizeLabel);

    sizeSlider = new QSlider(Qt::Horizontal, this);
    sizeSlider->setRange(1, 20);
    layout->addWidget(sizeSlider);

    QLabel* colorLabel = new QLabel(L("ѡ����ɫ"), this);
    layout->addWidget(colorLabel);

    colorButton = new QPushButton(L("ѡ����ɫ"), this);
    layout->addWidget(colorButton);

    connect(colorButton, &QPushButton::clicked, this, &PointSettings::onColorButtonClicked);
}

void PointSettings::reset() {
    sizeSlider->setValue(1); // ���ô�С
    colorButton->setStyleSheet("background-color: " + defaultColor.name());
}

void PointSettings::onColorButtonClicked() {
    currentColor = QColorDialog::getColor(currentColor, this, L("ѡ����ɫ"));
    if (currentColor.isValid()) {
        colorButton->setStyleSheet("background-color: " + currentColor.name());
    }
}