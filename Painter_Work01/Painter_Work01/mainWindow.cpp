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
    // 创建一个分割器，用于分隔 Canvas 和 PropertyEditor
    QSplitter* splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Horizontal);

    // 将 Canvas 添加到分割器左侧
    splitter->addWidget(canvas);

    // 将 GeoPropertyEditor 添加到分割器右侧
    splitter->addWidget(propertyEditor);

    // 设置分割器的比例（1:3）
    splitter->setStretchFactor(0, 8);
    splitter->setStretchFactor(1, 1);

    // 设置中心控件为分割器
    setCentralWidget(splitter);

    // 创建菜单栏、工具栏和状态栏
    createMenuBar();
    createToolBar();
    createNodeLineToolBar();
    createStatusBar();

    // 设置窗口标题和大小
    setWindowTitle(QStringLiteral("几何图形绘制与缓冲区分析"));
    resize(1024, 768);
}

MainWindow::~MainWindow()
{

}

void MainWindow::createMenuBar()
{
    QMenuBar* menuBar = new QMenuBar(this);

    // 文件菜单
    QMenu* fileMenu = menuBar->addMenu(L("文件(&F)"));
    QAction* openAction = fileMenu->addAction(L("打开(&O)..."));
    QAction* saveAction = fileMenu->addAction(L("保存(&S)..."));
    QAction* exportAction = fileMenu->addAction(L("导出到 SHP 文件(&E)..."));
    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction(L("退出(&Q)"));

    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(exportAction, &QAction::triggered, this, &MainWindow::exportToShp);
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);

    // 帮助菜单
    QMenu* helpMenu = menuBar->addMenu(L("帮助(&H)"));
    QAction* aboutAction = helpMenu->addAction(L("关于(&A)..."));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);

    setMenuBar(menuBar);
}

void MainWindow::createToolBar()
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
    statusBar->showMessage(L("准备就绪"));
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
    QMessageBox::about(this, L("关于"), L("几何图形绘制与缓冲区分析软件\n\n版本 1.0"));
}

// =========================================================================== Canvas
Canvas::Canvas(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true); // 启用鼠标跟踪
}

Canvas::~Canvas() {}

void Canvas::pushShape(Geo* geo)
{
    vec.push_back(geo);
}

void Canvas::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 启用抗锯齿
    painter.fillRect(this->rect(), Qt::white); // 绘制白色背景

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

// 强制完成绘制
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

    // 点属性
    pointShapeComboBox = new QComboBox(this);
    pointShapeComboBox->addItems({ L("圆点"), L("方点") });

    pointColorButton = new QPushButton(L("选择点颜色"), this);

    // 线属性
    lineStyleComboBox = new QComboBox(this);
    lineStyleComboBox->addItems({ L("实线"), L("虚线") });

    lineWidthSpinBox = new QSpinBox(this);
    lineWidthSpinBox->setRange(1, 50);

    lineColorButton = new QPushButton(L("选择线颜色"), this);

    lineDashPatternSpinBox = new QSpinBox(this);
    lineDashPatternSpinBox->setRange(1, 20);

    // 面属性
    fillColorButton = new QPushButton(L("选择填充颜色"), this);

    // 样条属性
    splineOrderSpinBox = new QSpinBox(this);
    splineOrderSpinBox->setRange(1, 10);

    splineNodeCountSpinBox = new QSpinBox(this);
    splineNodeCountSpinBox->setRange(2, 100);

    stepsSpinBox = new QSpinBox(this);
    stepsSpinBox->setRange(1, 100);

    // 布局
    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow(L("点形状:"), pointShapeComboBox);
    formLayout->addRow(L("点颜色:"), pointColorButton);
    formLayout->addRow(L("线样式:"), lineStyleComboBox);
    formLayout->addRow(L("线宽:"), lineWidthSpinBox);
    formLayout->addRow(L("线颜色:"), lineColorButton);
    formLayout->addRow(L("虚线段长:"), lineDashPatternSpinBox);
    formLayout->addRow(L("填充颜色:"), fillColorButton);
    formLayout->addRow(L("样条阶数:"), splineOrderSpinBox);
    formLayout->addRow(L("样条节点数:"), splineNodeCountSpinBox);
    formLayout->addRow(L("曲线密度:"), stepsSpinBox);

    layout->addLayout(formLayout);
    

    // 槽信号绑定前，先初始化
    applyGlobalSettings();

    // 信号槽
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
    // 从全局变量中读取并应用到各组件
    lineWidthSpinBox->setValue(GlobalLineWidth);                                                                           // 线宽
    lineStyleComboBox->setCurrentText(lineStyleToString(GlobalLineStyle));                                                 // 线形
    lineDashPatternSpinBox->setValue(GlobalLineDashPattern);                                                               // 段长
    splineOrderSpinBox->setValue(GlobalSplineOrder);                                                                       // 样条阶数
    splineNodeCountSpinBox->setValue(GlobalSplineNodeCount);                                                               // 样条节点个数
    stepsSpinBox->setValue(GlobalSteps);                                                                                   // 点的密度
    pointShapeComboBox->setCurrentText(pointShapeToString(GlobalPointShape));                                              // 点形状
    fillColorButton->setStyleSheet(QString("background-color: %1;").arg(QColor(GlobalFillColor).name()));                  // 填充颜色
}

void GeoPropertyEditor::setGeo(Geo* geo) 
{
    currentGeo = geo;

    if (geo) 
    {
        // 设置字段值
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
    QColor selectedColor = QColorDialog::getColor(Qt::white, this, L("选择颜色"));

    if (!selectedColor.isValid())
        return;

    senderButton->setStyleSheet(QString("background-color: %1").arg(selectedColor.name()));

    if (currentGeo) 
    {
        // 更新当前选中 Geo 对象的颜色
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
        // 设置全局默认颜色
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
        // 更新当前 Geo 的属性
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
        // 设置全局默认属性
        GlobalPointShape = stringToPointShape(pointShapeComboBox->currentText());
        GlobalLineStyle = stringToLineStyle(lineStyleComboBox->currentText());
        GlobalLineWidth = lineWidthSpinBox->value();
        GlobalLineDashPattern = lineDashPatternSpinBox->value();
        GlobalSplineOrder = splineOrderSpinBox->value();
        GlobalSplineNodeCount = splineNodeCountSpinBox->value();
        GlobalSteps = stepsSpinBox->value();

    }
}


// 文本到枚举的映射
LineStyle stringToLineStyle(const QString& styleText) 
{
    static const QMap<QString, LineStyle> lineStyleMap = 
    {
        {L("实线"), LineStyle::Solid},
        {L("虚线"), LineStyle::Dashed}
    };
    return lineStyleMap.value(styleText, LineStyle::Solid); // 默认返回 Solid
}

PointShape stringToPointShape(const QString& shapeText) 
{
    static const QMap<QString, PointShape> pointShapeMap = 
    {
        {L("方形"), PointShape::Square},
        {L("圆形"), PointShape::Circle}
    };
    return pointShapeMap.value(shapeText, PointShape::Square); // 默认返回 Square
}


QString lineStyleToString(LineStyle style)
{
    switch (style) 
    {
    case LineStyle::Solid:
        return L("实线");
    case LineStyle::Dashed:
        return L("虚线");
    default:
        return L("未知");
    }
}

QString pointShapeToString(PointShape shape) 
{
    switch (shape) 
    {
    case PointShape::Square:
        return L("方形");
    case PointShape::Circle:
        return L("圆形");
    default:
        return L("未知");
    }
}
