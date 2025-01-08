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
    QAction* drawPolylineAction = toolBar->addAction(L("��������"));
    drawPolylineAction->setCheckable(true);
    QAction* drawSplineAction = toolBar->addAction(L("����������"));
    drawSplineAction->setCheckable(true);
    QAction* drawArc3PointsAction = toolBar->addAction(L("��������Բ��")); 
    drawArc3PointsAction->setCheckable(true); 
    QAction* drawCircle2PointsAction = toolBar->addAction(L("��������Բ"));  
    drawCircle2PointsAction->setCheckable(true);
    QAction* drawPolygonAction = toolBar->addAction(L("���Ƽ���"));
    drawPolygonAction->setCheckable(true);
    QAction* drawComplexPolygonAction = toolBar->addAction(L("���Ƹ�����"));
    drawComplexPolygonAction->setCheckable(true);

    // ����ť���飬��ֻ֤��ѡ��һ����ť
    QActionGroup* actionGroup = new QActionGroup(this);
    actionGroup->addAction(drawPointAction);
    actionGroup->addAction(drawPolylineAction);
    actionGroup->addAction(drawSplineAction);
    actionGroup->addAction(drawArc3PointsAction);
    actionGroup->addAction(drawCircle2PointsAction);
    actionGroup->addAction(drawPolygonAction);
    actionGroup->addAction(drawComplexPolygonAction);
    actionGroup->setExclusive(true); // ����Ϊ����

    // �����źźͲ�
    connect(drawPointAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_DrawMode, DrawMode::DrawPoint); });
    connect(drawPolylineAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_DrawMode, DrawMode::DrawPolyline); });
    connect(drawSplineAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_DrawMode, DrawMode::DrawSpline); });
    connect(drawArc3PointsAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_DrawMode, DrawMode::DrawArcThreePoints); });
    connect(drawCircle2PointsAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing();setSetting(Key_DrawMode, DrawMode::DrawArcTwoPoints);});
    connect(drawPolygonAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_DrawMode, DrawMode::DrawPolygon); });
    connect(drawComplexPolygonAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); setSetting(Key_DrawMode, DrawMode::DrawComplexPolygon); });
    addToolBar(toolBar);
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