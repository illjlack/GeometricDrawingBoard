#include "WorkWindow.h"

#include <QMenuBar>
#include <QAction>
#include <QDockWidget>
#include <QVBoxLayout>
#include "Point.h"
#include <QPushButton>
#include "codec.h"
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QToolBar>

WorkWindow::WorkWindow(QMainWindow* parent)
    : QMainWindow(parent),
    openGLWidget(new OpenGLWidget(this))  // 创建OpenGL窗口
{
    openGLWidget->setFocus();

    initMap();

    // 设置主窗口的布局
    setWindowTitle("OpenGL with Qt");
    setCentralWidget(openGLWidget);

    // 创建菜单栏并添加选项
    QMenu* fileMenu = menuBar()->addMenu(L("文件"));
    QAction* exitAction = new QAction(L("退出"), this);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    fileMenu->addAction(exitAction);



    // 图形输入
    {
        QDockWidget* sideDockWidget = new QDockWidget(L("图形输入"), this);
        QComboBox* shapeComboBox = new QComboBox();
        QTextEdit* inputText = new QTextEdit();
        QPushButton* okButton = new QPushButton(L("确定"));
        QLabel* exampleLabel = new QLabel(L("输入样例格式：...(请选择图形)"));
        QAction* drew = menuBar()->addAction(L("图形输入"));
        sideDockWidget->setAllowedAreas(Qt::RightDockWidgetArea); // 只允许停靠在右边
        sideDockWidget->setFeatures(QDockWidget::DockWidgetClosable); // 允许关闭侧边栏

        QWidget* sidePanel = new QWidget();
        QVBoxLayout* sideLayout = new QVBoxLayout();
        sidePanel->setLayout(sideLayout);

        // 创建形状选择下拉框
        
        shapeComboBox->addItem(L("点"));
        shapeComboBox->addItem("Rectangle");
        shapeComboBox->addItem("Triangle");
        sideLayout->addWidget(shapeComboBox);
        connect(shapeComboBox, &QComboBox::currentTextChanged, this, [this, exampleLabel](const QString& test)
            {
                exampleLabel->setText(L(this->mp[test]["input_rule"]));
            });

        // 创建文本框用于输入形状信息
        
        inputText->setPlaceholderText(L("输入形状信息"));
        sideLayout->addWidget(inputText);

        // 创建确定按钮
        
        connect(okButton, &QPushButton::clicked, this, [=]() {
            // 获取用户输入的形状和文本
            QString selectedShape = shapeComboBox->currentText();
            QString userInput = inputText->toPlainText();  // 获取输入框中的内容

            //exampleLabel->setText(L("输入样例格式：\nCircle: 半径为 5"));
           
            });
        sideLayout->addWidget(okButton);

        // 创建显示输入样例格式的区域
        
        sideLayout->addWidget(exampleLabel);

        // 设置侧边栏
        sideDockWidget->setWidget(sidePanel);
        addDockWidget(Qt::RightDockWidgetArea, sideDockWidget); // 将侧边栏添加到右侧

        // 创建菜单项并连接到显示图形输入侧边栏
        
        connect(drew, &QAction::triggered, sideDockWidget, &QDockWidget::show);
    }

    

    // 添加输出栏
    {
        QDockWidget* outputDockWidget = new QDockWidget(L("输出窗口"), this);
        outputDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea);  // 输出栏停靠在底部
        outputDockWidget->setFeatures(QDockWidget::DockWidgetClosable);  // 启用关闭按钮
        QTextEdit* outputText = new QTextEdit();
        outputText->setReadOnly(true);
        outputDockWidget->setWidget(outputText);
        addDockWidget(Qt::BottomDockWidgetArea, outputDockWidget);
        
        QAction* outputWindowAction = menuBar()->addAction(L("输出窗口"));
        connect(outputWindowAction, &QAction::triggered, outputDockWidget, &QDockWidget::show);
    }

    {
        // 创建正交视图按钮
        QPushButton* orthoButton = new QPushButton(L("正交视图"));
        orthoButton->setCheckable(true);  // 使按钮可选中
        orthoButton->setChecked(false);  // 初始时设置按钮为未按下状态

        // 创建一个工具栏并将按钮添加到其中
        QToolBar* toolBar = addToolBar(L("工具栏"));
        toolBar->addWidget(orthoButton);  // 将按钮添加到工具栏

        // 连接按钮点击事件
        connect(orthoButton, &QPushButton::clicked, this, [=]() {
            if (orthoButton->isChecked()) {
                openGLWidget->setIsOrtho(true);  // 启用正交视图
            }
            else {
                openGLWidget->setIsOrtho(false);  // 禁用正交视图
            }
            });
        
        // 通过菜单打开工具栏
        QAction* showToolBarAction = new QAction(L("显示工具栏"), this);
        connect(showToolBarAction, &QAction::triggered, toolBar, &QToolBar::setVisible);
        menuBar()->addAction(showToolBarAction);  // 将动作添加到菜单
    }
    // 设置窗口的初始大小为 800x600
    resize(800, 600);
}



WorkWindow::~WorkWindow()
{
    // 确保 OpenGL widget 被销毁
    delete openGLWidget;
}

void WorkWindow::initMap()
{
    mp[L("点")]["input_rule"] =
        "第一行输入一个整数n,表示接下来要输入n个点。\n"
        "接下来n行,每行用空格间隔输入三个浮点数或整数x,y,z,表示三维坐标中点的位置\n"
        "example:\n"
        "2\n"
        "1.0 1.0 1.0\n"
        "0.0 0.0 0\n";
}
