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
    openGLWidget(new OpenGLWidget(this))  // 创建OpenGL窗口
{
    // 设置主窗口的布局
    setWindowTitle("OpenGL with Qt");

    // 添加 OpenGL 组件到左下角
    setCentralWidget(openGLWidget);

    // 创建菜单栏并添加选项
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    QAction* exitAction = new QAction(tr("Exit"), this);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    fileMenu->addAction(exitAction);

    // 添加侧边栏
    sideDockWidget = new QDockWidget(tr("Control Panel"), this);
    sideDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    QWidget* sidePanel = new QWidget();
    QVBoxLayout* sideLayout = new QVBoxLayout();
    sidePanel->setLayout(sideLayout);

    // 创建一个按钮并添加到侧边栏
    QPushButton* testButton = new QPushButton(tr("Run Test"), this);
    connect(testButton, &QPushButton::clicked, this, &MainWindow::test);  // 连接按钮的点击信号到test函数
    sideLayout->addWidget(testButton);  // 将按钮添加到布局中

    // 设置侧边栏内容
    sideDockWidget->setWidget(sidePanel);
    addDockWidget(Qt::LeftDockWidgetArea, sideDockWidget);

    // 设置窗口的初始大小为 800x600
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
    // 确保 OpenGL widget 被销毁
    delete openGLWidget;
}
