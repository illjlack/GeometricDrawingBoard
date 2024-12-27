#include "MainWindow.h"
#include <QMenuBar>
#include <QAction>
#include <QDockWidget>
#include <QVBoxLayout>
#include "Point.h"
#include <QPushButton>
#include "test.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    openGLWidget(new OpenGLWidget(this))  // ����OpenGL����
{
    // ���������ڵĲ���
    setWindowTitle("OpenGL with Qt");

    // ��� OpenGL ��������½�
    setCentralWidget(openGLWidget);

    // �����˵��������ѡ��
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    QAction* exitAction = new QAction(tr("Exit"), this);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    fileMenu->addAction(exitAction);

    // ��Ӳ����
    sideDockWidget = new QDockWidget(tr("Control Panel"), this);
    sideDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    QWidget* sidePanel = new QWidget();
    QVBoxLayout* sideLayout = new QVBoxLayout();
    sidePanel->setLayout(sideLayout);

    // ����һ����ť����ӵ������
    QPushButton* testButton = new QPushButton(tr("Run Test"), this);
    connect(testButton, &QPushButton::clicked, this, &MainWindow::test);  // ���Ӱ�ť�ĵ���źŵ�test����
    sideLayout->addWidget(testButton);  // ����ť��ӵ�������

    // ���ò��������
    sideDockWidget->setWidget(sidePanel);
    addDockWidget(Qt::LeftDockWidgetArea, sideDockWidget);

    // ���ô��ڵĳ�ʼ��СΪ 800x600
    resize(800, 600);
}

void MainWindow::test()
{
    // test
    //openGLWidget->pushObject(new Point({ 1, 1 , 1 }));
    //openGLWidget->pushObject(new Point({ 0.5, 1 , 0 }));
    //openGLWidget->pushObject(new Point({ 0.5, 0.5 , 0 }));
    openGLWidget->pushObject(new CubeRenderer());
}

MainWindow::~MainWindow()
{
    // ȷ�� OpenGL widget ������
    delete openGLWidget;
}
