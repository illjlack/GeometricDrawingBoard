#include "MainWindow.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QDockWidget>
#include <QSplitter>
#include "comm.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    canvas(new Canvas(this)),
    propertyEditor(new GeoPropertyEditor(this))
{
    // ����һ���ָ��������ڷָ� Canvas �� PropertyEditor
    QSplitter* splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Horizontal);

    // �� Canvas ��ӵ��ָ������
    splitter->addWidget(canvas);

    // �� GeoPropertyEditor ��ӵ��ָ����Ҳ�
    splitter->addWidget(propertyEditor);

    // ���÷ָ����ı�����1:3��
    splitter->setStretchFactor(0, 8);
    splitter->setStretchFactor(1, 1);

    // �������Ŀؼ�Ϊ�ָ���
    setCentralWidget(splitter);

    // �����˵�������������״̬��
    createMenuBar();
    createToolBar();
    createNodeLineToolBar();
    createStatusBar();

    // ���ô��ڱ���ʹ�С
    setWindowTitle(QStringLiteral("����ͼ�λ����뻺��������"));
    resize(1024, 768);
}

MainWindow::~MainWindow()
{

}

void MainWindow::createMenuBar()
{
    QMenuBar* menuBar = new QMenuBar(this);

    // �ļ��˵�
    QMenu* fileMenu = menuBar->addMenu(L("�ļ�(&F)"));
    QAction* openAction = fileMenu->addAction(L("��(&O)..."));
    QAction* saveAction = fileMenu->addAction(L("����(&S)..."));
    QAction* exportAction = fileMenu->addAction(L("������ SHP �ļ�(&E)..."));
    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction(L("�˳�(&Q)"));

    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(exportAction, &QAction::triggered, this, &MainWindow::exportToShp);
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);

    // �����˵�
    QMenu* helpMenu = menuBar->addMenu(L("����(&H)"));
    QAction* aboutAction = helpMenu->addAction(L("����(&A)..."));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);

    setMenuBar(menuBar);
}

void MainWindow::createToolBar()
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
    connect(drawPointAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); GlobalDrawMode = DrawMode::DrawPoint; });
    connect(drawSimpleLineAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); GlobalDrawMode = DrawMode::DrawSimpleLine; });
    connect(drawDoubleLineAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); GlobalDrawMode = DrawMode::DrawDoubleLine; });
    connect(drawParallelLineAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); GlobalDrawMode = DrawMode::DrawParallelLine; });
    connect(drawTwoPointCircleAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); GlobalDrawMode = DrawMode::DrawTwoPointCircle; });
    connect(drawSimpleAreaAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); GlobalDrawMode = DrawMode::DrawSimpleArea; });
    connect(drawComplexAreaAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); GlobalDrawMode = DrawMode::DrawComplexArea; });

    addToolBar(toolBar);
}

void MainWindow::createNodeLineToolBar()
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
    connect(polylineAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); GlobalNodeLineStyle = NodeLineStyle::StylePolyline; });
    connect(splineAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); GlobalNodeLineStyle = NodeLineStyle::StyleSpline; });
    connect(threePointArcAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); GlobalNodeLineStyle = NodeLineStyle::StyleThreePointArc; });
    connect(arcAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); GlobalNodeLineStyle = NodeLineStyle::StyleArc; });
    connect(streamlineAction, &QAction::triggered, this, [this] { canvas->CompleteDrawing(); GlobalNodeLineStyle = NodeLineStyle::StyleStreamline; });
    
    addToolBar(nodeLineToolBar);
}

void MainWindow::createStatusBar()
{
    QStatusBar* statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    statusBar->showMessage(L("׼������"));
}

void MainWindow::openFile()
{
}

void MainWindow::saveFile()
{
}

void MainWindow::exportToShp()
{
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, L("����"), L("����ͼ�λ����뻺�����������\n\n�汾 1.0"));
}

// =========================================================================== Canvas
Canvas::Canvas(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true); // ����������
}

Canvas::~Canvas() {}

void Canvas::pushShape(Geo* geo)
{
    vec.push_back(geo);
}

void Canvas::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // ���ÿ����
    painter.fillRect(this->rect(), Qt::white); // ���ư�ɫ����

    for (auto shape : vec)
    {
        shape->draw(painter);
    }
}

void Canvas::keyPressEvent(QKeyEvent* event)
{
}

void Canvas::keyReleaseEvent(QKeyEvent* event)
{
}

// ǿ����ɻ���
void Canvas::CompleteDrawing()
{
    if (currentDrawGeo)currentDrawGeo->completeDrawing();
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    if (DrawMode::None != GlobalDrawMode)
    {
        if (!currentDrawGeo || currentDrawGeo->isStateComplete())
        {
            currentDrawGeo = createGeo(GlobalDrawMode);
            pushShape(currentDrawGeo);
        }
        currentDrawGeo->mousePressEvent(event);
    }
    update();
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    if (currentDrawGeo)currentDrawGeo->mouseMoveEvent(event);
    update();
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (currentDrawGeo)currentDrawGeo->mouseReleaseEvent(event);
    update();
}

void Canvas::wheelEvent(QWheelEvent* event)
{
}


// =========================================================================== GeoPropertyEditor
GeoPropertyEditor::GeoPropertyEditor(QWidget* parent)
    : QWidget(parent), currentGeo(nullptr) {
    QVBoxLayout* layout = new QVBoxLayout(this);

    // ������
    pointShapeComboBox = new QComboBox(this);
    pointShapeComboBox->addItems({ L("Բ��"), L("����") });

    pointColorButton = new QPushButton(L("ѡ�����ɫ"), this);

    // ������
    lineStyleComboBox = new QComboBox(this);
    lineStyleComboBox->addItems({ L("ʵ��"), L("����") });

    lineWidthSpinBox = new QSpinBox(this);
    lineWidthSpinBox->setRange(1, 50);

    lineColorButton = new QPushButton(L("ѡ������ɫ"), this);

    lineDashPatternSpinBox = new QSpinBox(this);
    lineDashPatternSpinBox->setRange(1, 20);

    // ������
    fillColorButton = new QPushButton(L("ѡ�������ɫ"), this);

    // ��������
    splineOrderSpinBox = new QSpinBox(this);
    splineOrderSpinBox->setRange(1, 10);

    splineNodeCountSpinBox = new QSpinBox(this);
    splineNodeCountSpinBox->setRange(2, 100);

    stepsSpinBox = new QSpinBox(this);
    stepsSpinBox->setRange(1, 100);

    // ����
    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow(L("����״:"), pointShapeComboBox);
    formLayout->addRow(L("����ɫ:"), pointColorButton);
    formLayout->addRow(L("����ʽ:"), lineStyleComboBox);
    formLayout->addRow(L("�߿�:"), lineWidthSpinBox);
    formLayout->addRow(L("����ɫ:"), lineColorButton);
    formLayout->addRow(L("���߶γ�:"), lineDashPatternSpinBox);
    formLayout->addRow(L("�����ɫ:"), fillColorButton);
    formLayout->addRow(L("��������:"), splineOrderSpinBox);
    formLayout->addRow(L("�����ڵ���:"), splineNodeCountSpinBox);
    formLayout->addRow(L("�����ܶ�:"), stepsSpinBox);

    layout->addLayout(formLayout);
    

    // ���źŰ�ǰ���ȳ�ʼ��
    applyGlobalSettings();

    // �źŲ�
    connect(pointColorButton, &QPushButton::clicked, this, &GeoPropertyEditor::onColorButtonClicked);
    connect(lineColorButton, &QPushButton::clicked, this, &GeoPropertyEditor::onColorButtonClicked);
    connect(fillColorButton, &QPushButton::clicked, this, &GeoPropertyEditor::onColorButtonClicked);
    connect(pointShapeComboBox, &QComboBox::currentTextChanged, this, &GeoPropertyEditor::onValueChanged);
    connect(lineStyleComboBox, &QComboBox::currentTextChanged, this, &GeoPropertyEditor::onValueChanged);
    connect(lineWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &GeoPropertyEditor::onValueChanged);
    connect(lineDashPatternSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &GeoPropertyEditor::onValueChanged);
    connect(splineOrderSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &GeoPropertyEditor::onValueChanged);
    connect(splineNodeCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &GeoPropertyEditor::onValueChanged);
    connect(stepsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &GeoPropertyEditor::onValueChanged);
}

void GeoPropertyEditor::applyGlobalSettings() 
{
    // ��ȫ�ֱ����ж�ȡ��Ӧ�õ������
    lineWidthSpinBox->setValue(GlobalLineWidth);                                                                           // �߿�
    lineStyleComboBox->setCurrentText(lineStyleToString(GlobalLineStyle));                                                 // ����
    lineDashPatternSpinBox->setValue(GlobalLineDashPattern);                                                               // �γ�
    splineOrderSpinBox->setValue(GlobalSplineOrder);                                                                       // ��������
    splineNodeCountSpinBox->setValue(GlobalSplineNodeCount);                                                               // �����ڵ����
    stepsSpinBox->setValue(GlobalSteps);                                                                                   // ����ܶ�
    pointShapeComboBox->setCurrentText(pointShapeToString(GlobalPointShape));                                              // ����״
    fillColorButton->setStyleSheet(QString("background-color: %1;").arg(QColor(GlobalFillColor).name()));                  // �����ɫ
}

void GeoPropertyEditor::setGeo(Geo* geo) 
{
    currentGeo = geo;

    if (geo) 
    {
        // �����ֶ�ֵ
        //pointShapeComboBox->setCurrentText(QString::fromStdString(geo->getPointShape()));
        //currentPointColor = geo->getPointColor();
        //pointColorButton->setStyleSheet(QString("background-color: %1").arg(currentPointColor.name()));

        //lineStyleComboBox->setCurrentText(QString::fromStdString(geo->getLineStyle()));
        //lineWidthSpinBox->setValue(geo->getLineWidth());
        //currentLineColor = geo->getLineColor();
        //lineColorButton->setStyleSheet(QString("background-color: %1").arg(currentLineColor.name()));
        //lineDashPatternSpinBox->setValue(geo->getLineDashPattern());

        //currentFillColor = geo->getFillColor();
        //fillColorButton->setStyleSheet(QString("background-color: %1").arg(currentFillColor.name()));

        //splineOrderSpinBox->setValue(geo->getSplineOrder());
        //splineNodeCountSpinBox->setValue(geo->getSplineNodeCount());
        //stepsSpinBox->setValue(geo->getSteps());
    }
}

void GeoPropertyEditor::onColorButtonClicked() {
    QPushButton* senderButton = qobject_cast<QPushButton*>(sender());
    QColor selectedColor = QColorDialog::getColor(Qt::white, this, L("ѡ����ɫ"));

    if (!selectedColor.isValid())
        return;

    senderButton->setStyleSheet(QString("background-color: %1").arg(selectedColor.name()));

    if (currentGeo) 
    {
        // ���µ�ǰѡ�� Geo �������ɫ
        //if (senderButton == pointColorButton) {
        //    currentGeo->setPointColor(selectedColor);
        //}
        //else if (senderButton == lineColorButton) {
        //    currentGeo->setLineColor(selectedColor);
        //}
        //else if (senderButton == fillColorButton) {
        //    currentGeo->setFillColor(selectedColor);
        //}
        emit geoUpdated();
    }
    else 
    {
        // ����ȫ��Ĭ����ɫ
        if (senderButton == pointColorButton)
        {
            GlobalPointColor = selectedColor.rgba();
        }
        else if (senderButton == lineColorButton)
        {
            GlobalLineColor = selectedColor.rgba();
        }
        else if (senderButton == fillColorButton)
        {
            GlobalFillColor = selectedColor.rgba();
        }
    }
}



void GeoPropertyEditor::onValueChanged() 
{
    if (currentGeo) 
    {
        // ���µ�ǰ Geo ������
        //currentGeo->setPointShape(pointShapeComboBox->currentText().toStdString());
        //currentGeo->setLineStyle(lineStyleComboBox->currentText().toStdString());
        //currentGeo->setLineWidth(lineWidthSpinBox->value());
        //currentGeo->setLineDashPattern(lineDashPatternSpinBox->value());
        //currentGeo->setSplineOrder(splineOrderSpinBox->value());
        //currentGeo->setSplineNodeCount(splineNodeCountSpinBox->value());
        //currentGeo->setSteps(stepsSpinBox->value());
        emit geoUpdated();
    }
    else 
    {
        // ����ȫ��Ĭ������
        GlobalPointShape = stringToPointShape(pointShapeComboBox->currentText());
        GlobalLineStyle = stringToLineStyle(lineStyleComboBox->currentText());
        GlobalLineWidth = lineWidthSpinBox->value();
        GlobalLineDashPattern = lineDashPatternSpinBox->value();
        GlobalSplineOrder = splineOrderSpinBox->value();
        GlobalSplineNodeCount = splineNodeCountSpinBox->value();
        GlobalSteps = stepsSpinBox->value();

    }
}


// �ı���ö�ٵ�ӳ��
LineStyle stringToLineStyle(const QString& styleText) 
{
    static const QMap<QString, LineStyle> lineStyleMap = 
    {
        {L("ʵ��"), LineStyle::Solid},
        {L("����"), LineStyle::Dashed}
    };
    return lineStyleMap.value(styleText, LineStyle::Solid); // Ĭ�Ϸ��� Solid
}

PointShape stringToPointShape(const QString& shapeText) 
{
    static const QMap<QString, PointShape> pointShapeMap = 
    {
        {L("����"), PointShape::Square},
        {L("Բ��"), PointShape::Circle}
    };
    return pointShapeMap.value(shapeText, PointShape::Square); // Ĭ�Ϸ��� Square
}


QString lineStyleToString(LineStyle style)
{
    switch (style) 
    {
    case LineStyle::Solid:
        return L("ʵ��");
    case LineStyle::Dashed:
        return L("����");
    default:
        return L("δ֪");
    }
}

QString pointShapeToString(PointShape shape) 
{
    switch (shape) 
    {
    case PointShape::Square:
        return L("����");
    case PointShape::Circle:
        return L("Բ��");
    default:
        return L("δ֪");
    }
}
