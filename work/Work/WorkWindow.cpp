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
    openGLWidget(new OpenGLWidget(this))  // ����OpenGL����
{
    openGLWidget->setFocus();

    initMap();

    // ���������ڵĲ���
    setWindowTitle("OpenGL with Qt");
    setCentralWidget(openGLWidget);

    // �����˵��������ѡ��
    QMenu* fileMenu = menuBar()->addMenu(L("�ļ�"));
    QAction* exitAction = new QAction(L("�˳�"), this);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    fileMenu->addAction(exitAction);



    // ͼ������
    {
        QDockWidget* sideDockWidget = new QDockWidget(L("ͼ������"), this);
        QComboBox* shapeComboBox = new QComboBox();
        QTextEdit* inputText = new QTextEdit();
        QPushButton* okButton = new QPushButton(L("ȷ��"));
        QLabel* exampleLabel = new QLabel(L("����������ʽ��...(��ѡ��ͼ��)"));
        QAction* drew = menuBar()->addAction(L("ͼ������"));
        sideDockWidget->setAllowedAreas(Qt::RightDockWidgetArea); // ֻ����ͣ�����ұ�
        sideDockWidget->setFeatures(QDockWidget::DockWidgetClosable); // ����رղ����

        QWidget* sidePanel = new QWidget();
        QVBoxLayout* sideLayout = new QVBoxLayout();
        sidePanel->setLayout(sideLayout);

        // ������״ѡ��������
        
        shapeComboBox->addItem(L("��"));
        shapeComboBox->addItem("Rectangle");
        shapeComboBox->addItem("Triangle");
        sideLayout->addWidget(shapeComboBox);
        connect(shapeComboBox, &QComboBox::currentTextChanged, this, [this, exampleLabel](const QString& test)
            {
                exampleLabel->setText(L(this->mp[test]["input_rule"]));
            });

        // �����ı�������������״��Ϣ
        
        inputText->setPlaceholderText(L("������״��Ϣ"));
        sideLayout->addWidget(inputText);

        // ����ȷ����ť
        
        connect(okButton, &QPushButton::clicked, this, [=]() {
            // ��ȡ�û��������״���ı�
            QString selectedShape = shapeComboBox->currentText();
            QString userInput = inputText->toPlainText();  // ��ȡ������е�����

            //exampleLabel->setText(L("����������ʽ��\nCircle: �뾶Ϊ 5"));
           
            });
        sideLayout->addWidget(okButton);

        // ������ʾ����������ʽ������
        
        sideLayout->addWidget(exampleLabel);

        // ���ò����
        sideDockWidget->setWidget(sidePanel);
        addDockWidget(Qt::RightDockWidgetArea, sideDockWidget); // ���������ӵ��Ҳ�

        // �����˵�����ӵ���ʾͼ����������
        
        connect(drew, &QAction::triggered, sideDockWidget, &QDockWidget::show);
    }

    

    // ��������
    {
        QDockWidget* outputDockWidget = new QDockWidget(L("�������"), this);
        outputDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea);  // �����ͣ���ڵײ�
        outputDockWidget->setFeatures(QDockWidget::DockWidgetClosable);  // ���ùرհ�ť
        QTextEdit* outputText = new QTextEdit();
        outputText->setReadOnly(true);
        outputDockWidget->setWidget(outputText);
        addDockWidget(Qt::BottomDockWidgetArea, outputDockWidget);
        
        QAction* outputWindowAction = menuBar()->addAction(L("�������"));
        connect(outputWindowAction, &QAction::triggered, outputDockWidget, &QDockWidget::show);
    }

    {
        // ����������ͼ��ť
        QPushButton* orthoButton = new QPushButton(L("������ͼ"));
        orthoButton->setCheckable(true);  // ʹ��ť��ѡ��
        orthoButton->setChecked(false);  // ��ʼʱ���ð�ťΪδ����״̬

        // ����һ��������������ť��ӵ�����
        QToolBar* toolBar = addToolBar(L("������"));
        toolBar->addWidget(orthoButton);  // ����ť��ӵ�������

        // ���Ӱ�ť����¼�
        connect(orthoButton, &QPushButton::clicked, this, [=]() {
            if (orthoButton->isChecked()) {
                openGLWidget->setIsOrtho(true);  // ����������ͼ
            }
            else {
                openGLWidget->setIsOrtho(false);  // ����������ͼ
            }
            });
        
        // ͨ���˵��򿪹�����
        QAction* showToolBarAction = new QAction(L("��ʾ������"), this);
        connect(showToolBarAction, &QAction::triggered, toolBar, &QToolBar::setVisible);
        menuBar()->addAction(showToolBarAction);  // ��������ӵ��˵�
    }
    // ���ô��ڵĳ�ʼ��СΪ 800x600
    resize(800, 600);
}



WorkWindow::~WorkWindow()
{
    // ȷ�� OpenGL widget ������
    delete openGLWidget;
}

void WorkWindow::initMap()
{
    mp[L("��")]["input_rule"] =
        "��һ������һ������n,��ʾ������Ҫ����n���㡣\n"
        "������n��,ÿ���ÿո�����������������������x,y,z,��ʾ��ά�����е��λ��\n"
        "example:\n"
        "2\n"
        "1.0 1.0 1.0\n"
        "0.0 0.0 0\n";
}
