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
    // �������Ŀؼ�Ϊ Canvas
    // setCentralWidget(canvas);

    // �����˵�������������״̬��
    createMenuBar();
    createToolBar();
    createStatusBar();

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
    QAction* drawLineAction = toolBar->addAction(L("������"));
    QAction* drawAreaAction = toolBar->addAction(L("������"));

    //connect(drawPointAction, &QAction::triggered, canvas, &Canvas::setDrawPointMode);
    //connect(drawLineAction, &QAction::triggered, canvas, &Canvas::setDrawLineMode);
    //connect(drawAreaAction, &QAction::triggered, canvas, &Canvas::setDrawAreaMode);

    addToolBar(toolBar);
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
