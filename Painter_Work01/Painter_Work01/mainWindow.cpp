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
#include <QCheckBox>
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


    connect(canvas, &Canvas::selectedGeo, propertyEditor, &GeoPropertyEditor::setGeo);
    connect(propertyEditor, &GeoPropertyEditor::updateGeo, canvas, [this]() {
        canvas->update();
        });


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
    QAction* selectAction = toolBar->addAction(L("选择模式"));
    selectAction->setCheckable(true); // 设置为可选中
    QAction* drawPointAction = toolBar->addAction(L("绘制点"));
    drawPointAction->setCheckable(true);
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
    actionGroup->addAction(selectAction);
    actionGroup->addAction(drawPointAction);
    actionGroup->addAction(drawSimpleLineAction);
    actionGroup->addAction(drawDoubleLineAction);
    actionGroup->addAction(drawParallelLineAction);
    actionGroup->addAction(drawTwoPointCircleAction);
    actionGroup->addAction(drawSimpleAreaAction);
    actionGroup->addAction(drawComplexAreaAction);
    actionGroup->setExclusive(true); // 设置为互斥

    // 默认选中折线按钮
    selectAction->setChecked(true);

    // 连接信号和槽
    connect(selectAction, &QAction::triggered, this, [this] { canvas->modeChange(); GlobalDrawMode = DrawMode::DrawSelect; });
    connect(drawPointAction, &QAction::triggered, this, [this] { canvas->modeChange(); GlobalDrawMode = DrawMode::DrawPoint; });
    connect(drawSimpleLineAction, &QAction::triggered, this, [this] { canvas->modeChange(); GlobalDrawMode = DrawMode::DrawSimpleLine; });
    connect(drawDoubleLineAction, &QAction::triggered, this, [this] { canvas->modeChange(); GlobalDrawMode = DrawMode::DrawDoubleLine; });
    connect(drawParallelLineAction, &QAction::triggered, this, [this] { canvas->modeChange(); GlobalDrawMode = DrawMode::DrawParallelLine; });
    connect(drawTwoPointCircleAction, &QAction::triggered, this, [this] { canvas->modeChange(); GlobalDrawMode = DrawMode::DrawTwoPointCircle; });
    connect(drawSimpleAreaAction, &QAction::triggered, this, [this] { canvas->modeChange(); GlobalDrawMode = DrawMode::DrawSimpleArea; });
    connect(drawComplexAreaAction, &QAction::triggered, this, [this] { canvas->modeChange(); GlobalDrawMode = DrawMode::DrawComplexArea; });

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
    connect(polylineAction, &QAction::triggered, this, [this] { canvas->modeChange(); GlobalNodeLineStyle = NodeLineStyle::StylePolyline; });
    connect(splineAction, &QAction::triggered, this, [this] { canvas->modeChange(); GlobalNodeLineStyle = NodeLineStyle::StyleSpline; });
    connect(threePointArcAction, &QAction::triggered, this, [this] { canvas->modeChange(); GlobalNodeLineStyle = NodeLineStyle::StyleThreePointArc; });
    connect(arcAction, &QAction::triggered, this, [this] { canvas->modeChange(); GlobalNodeLineStyle = NodeLineStyle::StyleArc; });
    connect(streamlineAction, &QAction::triggered, this, [this] { canvas->modeChange(); GlobalNodeLineStyle = NodeLineStyle::StyleStreamline; });
    
    addToolBar(nodeLineToolBar);
}

void MainWindow::createStatusBar()
{
    setStatusBar(GlobalStatusBar);
    GlobalStatusBar->showMessage(L("准备就绪"));
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

    setFocusPolicy(Qt::StrongFocus); // 启用鼠标点击和键盘切换获取焦点
}

Canvas::~Canvas() {}

void Canvas::resetView()
{
    view.reset(); // 重置为单位矩阵
    update();     // 触发重绘
}

void Canvas::scaleView(qreal scaleFactor)
{
    QTransform scale;
    scale.scale(scaleFactor, scaleFactor);
    view *= scale; // 累积缩放
    update();      // 触发重绘
}

void Canvas::translateView(qreal dx, qreal dy)
{
    QTransform translation;
    translation.translate(dx, dy);
    view *= translation; // 累积平移
    update();            // 触发重绘
}

// 添加对象到链表和映射表
void Canvas::pushGeo(Geo* geo)
{
    if (!geo)
        return;

    geoList.push_back(geo);

    auto it = std::prev(geoList.end());
    geoMap[geo] = it;
}

// 从链表和映射表中移除对象
void Canvas::removeGeo(Geo* geo)
{
    if (!geo || geoMap.find(geo) == geoMap.end())
        return;

    auto it = geoMap[geo];
    geoList.erase(it);

    geoMap.erase(geo);
    update();
}


void Canvas::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    painter.setTransform(view); // 应用视图矩阵
    
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    painter.setRenderHint(QPainter::Antialiasing); // 启用抗锯齿
    
    // 无限白色背景：绘制一个大于视口范围的矩形作为背景
    QRectF extendedRect = QRectF(
        view.inverted().map(QPointF(0, 0)),                        // 左上角
        view.inverted().map(QPointF(width(), height()))           // 右下角
    ).adjusted(-10000, -10000, 10000, 10000);                     // 延伸范围
    painter.fillRect(extendedRect, Qt::white);


    // 遍历链表中的所有形状并绘制
    for (const auto& shape : geoList)
    {
        if (shape)
        {
            shape->draw(painter);
        }
    }
}


void Canvas::keyPressEvent(QKeyEvent* event)
{
    if (currentSelectGeo &&(event->key() == Qt::Key_Delete|| event->key() == Qt::Key_Backspace))
    {
        removeGeo(currentSelectGeo);
        currentSelectGeo = nullptr;
        selectedGeo(nullptr);
    }

    qreal step = 50.0; // 平移步长
    switch (event->key())
    {
    case Qt::Key_R:
        resetView();
        break;
    case Qt::Key_Left:
        translateView(step, 0);
        break;
    case Qt::Key_Right:
        translateView(-step, 0);
        break;
    case Qt::Key_Up:
        translateView(0, step);
        break;
    case Qt::Key_Down:
        translateView(0, -step);
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}

void Canvas::keyReleaseEvent(QKeyEvent* event)
{
}

// 强制完成绘制
void Canvas::modeChange()
{
    if (currentSelectGeo)
    {
        if (currentSelectGeo->isStateDrawing())
        {
            currentSelectGeo->completeDrawing();
        }

        // 如果不合法，删除
        if (currentSelectGeo->isStateInvalid())
        {
            removeGeo(currentSelectGeo);
            currentSelectGeo = nullptr;
        }
 
        if (currentSelectGeo)
        {
            currentSelectGeo->setStateNotSelected();
            emit selectedGeo(nullptr);
        }
    }
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    QPointF pos = mapPoint(event->pos());

    if (event->button() == Qt::LeftButton) // 检测是否是左键
    {
        isLeftButtonPressed = true;
        hitPoint = event -> pos(); //  屏幕坐标，因为移动的时候已经缩放了（移动和缩放操作是一起累积的）
    }

    int selected = false;
    // 如果是选择模式，做点击测试
    if (DrawMode::DrawSelect == GlobalDrawMode)
    {
        if (currentSelectGeo)
        {
            currentSelectGeo->setStateNotSelected();
            currentSelectGeo = nullptr;
        }
        for (auto it = geoList.rbegin(); it != geoList.rend(); ++it)
        {
            Geo* geo = *it;
            if (geo && geo->hitTesting(pos))
            {
                currentSelectGeo = geo;
                currentSelectGeo->setStateSelected();

                emit selectedGeo(geo);
                selected = true;
                break;
            }
        }
    }
    else
    {
        if (!currentSelectGeo || currentSelectGeo->isStateComplete())
        {
            currentSelectGeo = createGeo(GlobalDrawMode);
            pushGeo(currentSelectGeo);
        }
        currentSelectGeo->mousePressEvent(event, pos);
    }
    // 空表示没有选中对象
    if (!selected)
    {
        emit selectedGeo(nullptr);
    }
    update();
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    QPointF pos = mapPoint(event->pos());

    QString coordinateText = QString("X: %1, Y: %2")
        .arg(pos.x(), 0, 'f', 2)  // 'f' 表示固定点格式，2 表示显示小数点后两位
        .arg(pos.y(), 0, 'f', 2);
    GlobalStatusBar->showMessage(coordinateText);

    if (currentSelectGeo)
    {
        // 如果是选择模式，可以进行拖拽
        if (DrawMode::DrawSelect == GlobalDrawMode && isLeftButtonPressed)
        {
            // 为了避免拖拽的时候一直算缓冲区，这里先关了
            GeoParameters geoParameters = currentSelectGeo->getGeoParameters();
            if (geoParameters.bufferVisible)
            {
                geoParameters.bufferVisible = false;
                //currentSelectGeo->setGeoParameters(geoParameters);
                //emit selectedGeo(currentSelectGeo); // 更新一下uis
            }
            currentSelectGeo->dragGeo(pos);
        }
        else
        {
            currentSelectGeo->mouseMoveEvent(event, pos);
        }
    }
    else
    {
        if (DrawMode::DrawSelect == GlobalDrawMode && isLeftButtonPressed)
        {
            QPointF d = event->pos() - hitPoint;
            translateView(d.x(), d.y());
        }
    }
    hitPoint = event->pos();
    update();
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    QPointF pos = mapPoint(event->pos());

    if (event->button() == Qt::LeftButton) // 检测是否是左键
    {
        isLeftButtonPressed = false;
    }

    if (currentSelectGeo)
    {
        currentSelectGeo->mouseReleaseEvent(event);
    }
    update();
}

void Canvas::wheelEvent(QWheelEvent* event)
{
    qreal scaleFactor = 1.1;
    if (event->angleDelta().y() < 0)
    {
        scaleFactor = 1.0 / scaleFactor;
    }
    scaleView(scaleFactor);
}

QPointF Canvas::mapPoint(const QPointF& pos) const
{
    // 使用视图矩阵的逆变换将事件位置映射为浮动坐标
    return view.inverted().map(pos);
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

    // 缓冲区属性
    bufferVisibleCheckBox = new QCheckBox(L("显示缓冲区"), this);

    bufferCalculationModeComboBox = new QComboBox(this);
    bufferCalculationModeComboBox->addItems({ L("矢量"), L("栅格") });

    bufferDistanceSpinBox = new QSpinBox(this);
    bufferDistanceSpinBox->setRange(1, 1000);

    // 缓冲区线属性
    bufferLineStyleComboBox = new QComboBox(this);
    bufferLineStyleComboBox->addItems({ L("实线"), L("虚线") });

    bufferLineWidthSpinBox = new QSpinBox(this);
    bufferLineWidthSpinBox->setRange(1, 50);

    bufferLineColorButton = new QPushButton(L("选择缓冲区线颜色"), this);

    bufferLineDashPatternSpinBox = new QSpinBox(this);
    bufferLineDashPatternSpinBox->setRange(1, 20);

    // 缓冲区面属性
    bufferFillColorButton = new QPushButton(L("选择缓冲区填充颜色"), this);

    // 缓冲区边框属性
    bufferHasBorderCheckBox = new QCheckBox(L("缓冲区是否有边框"), this);

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
    formLayout->addRow(L("显示缓冲区:"), bufferVisibleCheckBox);
    formLayout->addRow(L("缓冲区计算方式:"), bufferCalculationModeComboBox);
    formLayout->addRow(L("缓冲区距离:"), bufferDistanceSpinBox);
    formLayout->addRow(L("缓冲区线样式:"), bufferLineStyleComboBox);
    formLayout->addRow(L("缓冲区线宽:"), bufferLineWidthSpinBox);
    formLayout->addRow(L("缓冲区线颜色:"), bufferLineColorButton);
    formLayout->addRow(L("缓冲区虚线段长:"), bufferLineDashPatternSpinBox);
    formLayout->addRow(L("缓冲区填充颜色:"), bufferFillColorButton);
    formLayout->addRow(L("缓冲区有边框:"), bufferHasBorderCheckBox);

    layout->addLayout(formLayout);

    // 初始化
    applyGlobalSettings();

    // 信号槽绑定
    connect(pointColorButton, &QPushButton::clicked, this, &GeoPropertyEditor::onColorButtonClicked);
    connect(lineColorButton, &QPushButton::clicked, this, &GeoPropertyEditor::onColorButtonClicked);
    connect(fillColorButton, &QPushButton::clicked, this, &GeoPropertyEditor::onColorButtonClicked);
    connect(bufferLineColorButton, &QPushButton::clicked, this, &GeoPropertyEditor::onColorButtonClicked);
    connect(bufferFillColorButton, &QPushButton::clicked, this, &GeoPropertyEditor::onColorButtonClicked);

    connect(pointShapeComboBox, &QComboBox::currentTextChanged, this, &GeoPropertyEditor::onValueChanged);
    connect(lineStyleComboBox, &QComboBox::currentTextChanged, this, &GeoPropertyEditor::onValueChanged);
    connect(bufferLineStyleComboBox, &QComboBox::currentTextChanged, this, &GeoPropertyEditor::onValueChanged);
    connect(bufferCalculationModeComboBox, &QComboBox::currentTextChanged, this, &GeoPropertyEditor::onValueChanged);

    connect(lineWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &GeoPropertyEditor::onValueChanged);
    connect(lineDashPatternSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &GeoPropertyEditor::onValueChanged);
    connect(bufferLineWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &GeoPropertyEditor::onValueChanged);
    connect(bufferLineDashPatternSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &GeoPropertyEditor::onValueChanged);
    connect(splineOrderSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &GeoPropertyEditor::onValueChanged);
    connect(splineNodeCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &GeoPropertyEditor::onValueChanged);
    connect(stepsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &GeoPropertyEditor::onValueChanged);
    connect(bufferDistanceSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &GeoPropertyEditor::onValueChanged);

    connect(bufferVisibleCheckBox, &QCheckBox::stateChanged, this, &GeoPropertyEditor::onValueChanged);
    connect(bufferHasBorderCheckBox, &QCheckBox::stateChanged, this, &GeoPropertyEditor::onValueChanged);
}


void GeoPropertyEditor::setGeo(Geo* geo) 
{
    // 这个也会引起组件更新，加一个标记是否是切换对象，如果是切换对象引起的组件变换，不设置给对象
    isSwitchingObject = true;

    currentGeo = geo;
    if (geo)setGeoParameters(geo->getGeoParameters());
    else applyGlobalSettings();
    
    isSwitchingObject = false;
}

void GeoPropertyEditor::onColorButtonClicked() {
    if (isSwitchingObject) return; // 处于切换对象期间,不处理

    QPushButton* senderButton = qobject_cast<QPushButton*>(sender());
    QColor* selectedColor = nullptr;

    if (senderButton == pointColorButton) {
        selectedColor = &currentPointColor;
    }
    else if (senderButton == lineColorButton) {
        selectedColor = &currentLineColor;
    }
    else if (senderButton == fillColorButton) {
        selectedColor = &currentFillColor;
    }
    else if (senderButton == bufferLineColorButton) {
        selectedColor = &currentBufferLineColor;
    }
    else if (senderButton == bufferFillColorButton) {
        selectedColor = &currentBufferFillColor;
    }

    if (!selectedColor) {
        return; // 未匹配到对应的按钮
    }

    *selectedColor = QColorDialog::getColor(
        *selectedColor,                // 初始颜色
        this,                          // 父窗口
        L("选择颜色"),                 // 对话框标题
        QColorDialog::ShowAlphaChannel // 启用透明度选择
    );

    if (!selectedColor->isValid()) {
        return; // 如果用户取消了颜色选择，则退出
    }

    // 设置按钮背景颜色
    senderButton->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);")
        .arg(selectedColor->red())
        .arg(selectedColor->green())
        .arg(selectedColor->blue())
        .arg(selectedColor->alpha()));

    if (currentGeo)
    {
        currentGeo->setGeoParameters(getGeoParameters());
    }
    else {
        // 设置全局默认颜色
        if (senderButton == pointColorButton) {
            GlobalPointColor = *selectedColor;
        }
        else if (senderButton == lineColorButton) {
            GlobalLineColor = *selectedColor;
        }
        else if (senderButton == fillColorButton) {
            GlobalFillColor = *selectedColor;        
        }
        else if (senderButton == bufferLineColorButton) {
            GlobalBufferLineColor = *selectedColor;
        }
        else if (senderButton == bufferFillColorButton) {
            GlobalBufferFillColor = *selectedColor;
        }
    }
}



void GeoPropertyEditor::onValueChanged()
{
    if (isSwitchingObject)return; // 处于切换对象期间,不处理

    if (currentGeo)
    {
        // 如果当前绑定了 Geo 对象，则更新其属性
        currentGeo->setGeoParameters(getGeoParameters());
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

        // 缓冲区属性
        GlobalBufferVisible = bufferVisibleCheckBox->isChecked();  // 缓冲区是否可见
        GlobalBufferCalculationMode = stringToBufferCalculationMode(bufferCalculationModeComboBox->currentText());  // 缓冲区计算模式
        GlobalBufferDistance = bufferDistanceSpinBox->value();  // 缓冲区距离

        // 缓冲区线样式属性
        GlobalBufferLineStyle = stringToLineStyle(bufferLineStyleComboBox->currentText());  // 缓冲区线样式
        GlobalBufferLineWidth = bufferLineWidthSpinBox->value();  // 缓冲区线宽
        GlobalBufferLineDashPattern = bufferLineDashPatternSpinBox->value();  // 缓冲区虚线段长度

        // 颜色设置
        GlobalPointColor = currentPointColor;  // 点颜色
        GlobalLineColor = currentLineColor;  // 线颜色
        GlobalFillColor = currentFillColor;  // 填充颜色
        GlobalBufferLineColor = currentBufferLineColor;  // 缓冲区线颜色
        GlobalBufferFillColor = currentBufferFillColor;  // 缓冲区填充颜色
    }
    emit updateGeo();
}


void GeoPropertyEditor::applyGlobalSettings()
{
    // 从全局变量中读取并应用到各组件

// 更新颜色相关属性和按钮样式
    currentPointColor = QColor(GlobalPointColor); // 当前点颜色
    pointColorButton->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);")
        .arg(currentPointColor.red())
        .arg(currentPointColor.green())
        .arg(currentPointColor.blue())
        .arg(currentPointColor.alpha()));

    currentLineColor = QColor(GlobalLineColor); // 当前线颜色
    lineColorButton->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);")
        .arg(currentLineColor.red())
        .arg(currentLineColor.green())
        .arg(currentLineColor.blue())
        .arg(currentLineColor.alpha()));

    currentFillColor = QColor(GlobalFillColor); // 当前填充颜色
    fillColorButton->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);")
        .arg(currentFillColor.red())
        .arg(currentFillColor.green())
        .arg(currentFillColor.blue())
        .arg(currentFillColor.alpha()));

    currentBufferLineColor = QColor(GlobalBufferLineColor); // 当前缓冲区线颜色
    bufferLineColorButton->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);")
        .arg(currentBufferLineColor.red())
        .arg(currentBufferLineColor.green())
        .arg(currentBufferLineColor.blue())
        .arg(currentBufferLineColor.alpha()));

    currentBufferFillColor = QColor(GlobalBufferFillColor); // 当前缓冲区面颜色
    bufferFillColorButton->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);")
        .arg(currentBufferFillColor.red())
        .arg(currentBufferFillColor.green())
        .arg(currentBufferFillColor.blue())
        .arg(currentBufferFillColor.alpha()));


    // 点属性设置
    pointShapeComboBox->setCurrentText(pointShapeToString(GlobalPointShape)); // 点形状

    // 线属性设置
    lineStyleComboBox->setCurrentText(lineStyleToString(GlobalLineStyle)); // 线样式
    lineWidthSpinBox->setValue(GlobalLineWidth); // 线宽
    lineDashPatternSpinBox->setValue(GlobalLineDashPattern); // 虚线段长度

    // 面属性设置
    // 填充颜色已在颜色部分设置

    // 样条属性设置
    splineOrderSpinBox->setValue(GlobalSplineOrder); // 样条阶数
    splineNodeCountSpinBox->setValue(GlobalSplineNodeCount); // 样条节点数
    stepsSpinBox->setValue(GlobalSteps); // 曲线密度

    // 缓冲区相关设置
    bufferVisibleCheckBox->setChecked(GlobalBufferVisible); // 缓冲区可见性
    bufferCalculationModeComboBox->setCurrentText(bufferCalculationModeToString(GlobalBufferCalculationMode)); // 缓冲区计算模式
    bufferDistanceSpinBox->setValue(GlobalBufferDistance); // 缓冲区距离

    // 缓冲区线属性设置
    bufferLineStyleComboBox->setCurrentText(lineStyleToString(GlobalBufferLineStyle)); // 缓冲区线样式
    bufferLineWidthSpinBox->setValue(GlobalBufferLineWidth); // 缓冲区线宽
    bufferLineDashPatternSpinBox->setValue(GlobalBufferLineDashPattern); // 缓冲区虚线段长度

    // 缓冲区边框属性设置
    bufferHasBorderCheckBox->setChecked(GlobalBufferHasBorder); // 是否有边框
}

void GeoPropertyEditor::setGeoParameters(const GeoParameters& params)
{
    // 设置 UI 组件的值
    lineStyleComboBox->setCurrentText(lineStyleToString(params.lineStyle));
    lineWidthSpinBox->setValue(params.lineWidth);
    lineDashPatternSpinBox->setValue(params.lineDashPattern);
    splineOrderSpinBox->setValue(params.splineOrder);
    splineNodeCountSpinBox->setValue(params.splineNodeCount);
    stepsSpinBox->setValue(params.steps);
    pointShapeComboBox->setCurrentText(pointShapeToString(params.pointShape));

    // 设置缓冲区相关组件
    bufferVisibleCheckBox->setChecked(params.bufferVisible);  // 缓冲区可见性
    bufferCalculationModeComboBox->setCurrentText(bufferCalculationModeToString(params.bufferCalculationMode));  // 缓冲区计算模式
    bufferDistanceSpinBox->setValue(params.bufferDistance);  // 缓冲区距离

    // 设置缓冲区线的属性
    bufferLineStyleComboBox->setCurrentText(lineStyleToString(params.bufferLineStyle)); // 缓冲区线样式
    bufferLineWidthSpinBox->setValue(params.bufferLineWidth); // 缓冲区线宽
    bufferLineDashPatternSpinBox->setValue(params.bufferLineDashPattern); // 缓冲区虚线段长度
    bufferHasBorderCheckBox->setChecked(params.bufferHasBorder); // 是否有边框

    // 设置颜色按钮背景
    currentPointColor = params.pointColor;
    currentLineColor = params.lineColor;
    currentFillColor = params.fillColor;
    currentBufferLineColor = params.bufferLineColor;
    currentBufferFillColor = params.bufferFillColor;

    bufferLineColorButton->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);")
        .arg(currentBufferLineColor.red())
        .arg(currentBufferLineColor.green())
        .arg(currentBufferLineColor.blue())
        .arg(currentBufferLineColor.alpha()));

    bufferFillColorButton->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);")
        .arg(currentBufferFillColor.red())
        .arg(currentBufferFillColor.green())
        .arg(currentBufferFillColor.blue())
        .arg(currentBufferFillColor.alpha()));

    pointColorButton->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);")
        .arg(currentPointColor.red())
        .arg(currentPointColor.green())
        .arg(currentPointColor.blue())
        .arg(currentPointColor.alpha()));

    lineColorButton->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);")
        .arg(currentLineColor.red())
        .arg(currentLineColor.green())
        .arg(currentLineColor.blue())
        .arg(currentLineColor.alpha()));

    fillColorButton->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);")
        .arg(currentFillColor.red())
        .arg(currentFillColor.green())
        .arg(currentFillColor.blue())
        .arg(currentFillColor.alpha()));

}


GeoParameters GeoPropertyEditor::getGeoParameters() const
{
    GeoParameters params = currentGeo->getGeoParameters();

    // 从 UI 组件获取值
    // 获取颜色值
    params.pointColor = currentPointColor;
    params.lineColor = currentLineColor;
    params.fillColor = currentFillColor;
    params.bufferLineColor = currentBufferLineColor;
    params.bufferFillColor = currentBufferFillColor;

    // 获取点属性设置
    params.pointShape = stringToPointShape(pointShapeComboBox->currentText());

    // 获取线属性设置
    params.lineStyle = stringToLineStyle(lineStyleComboBox->currentText());
    params.lineWidth = lineWidthSpinBox->value();
    params.lineDashPattern = lineDashPatternSpinBox->value();

    // 获取样条属性设置
    params.splineOrder = splineOrderSpinBox->value();
    params.splineNodeCount = splineNodeCountSpinBox->value();
    params.steps = stepsSpinBox->value();

    // 获取缓冲区相关设置
    params.bufferVisible = bufferVisibleCheckBox->isChecked();  // 缓冲区可见性
    params.bufferCalculationMode = stringToBufferCalculationMode(bufferCalculationModeComboBox->currentText());  // 缓冲区计算模式
    params.bufferDistance = bufferDistanceSpinBox->value();  // 缓冲区距离

    // 获取缓冲区线相关设置
    params.bufferLineStyle = stringToLineStyle(bufferLineStyleComboBox->currentText());
    params.bufferLineWidth = bufferLineWidthSpinBox->value();
    params.bufferLineDashPattern = bufferLineDashPatternSpinBox->value();

    // 获取缓冲区边框设置
    params.bufferHasBorder = bufferHasBorderCheckBox->isChecked();

    return params;
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
        {L("方点"), PointShape::Square},
        {L("圆点"), PointShape::Circle}
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
        return L("方点");
    case PointShape::Circle:
        return L("圆点");
    default:
        return L("未知");
    }
}

// 文本到枚举的映射：缓冲区计算模式
BufferCalculationMode stringToBufferCalculationMode(const QString& modeText)
{
    static const QMap<QString, BufferCalculationMode> bufferCalculationModeMap =
    {
        {L("矢量"), BufferCalculationMode::Vector},
        {L("栅格"), BufferCalculationMode::Raster}
    };
    return bufferCalculationModeMap.value(modeText, BufferCalculationMode::Vector); // 默认返回 Vector
}

// 枚举到文本的映射：缓冲区计算模式
QString bufferCalculationModeToString(BufferCalculationMode mode)
{
    switch (mode)
    {
    case BufferCalculationMode::Vector:
        return L("矢量");
    case BufferCalculationMode::Raster:
        return L("栅格");
    default:
        return L("未知");
    }
}