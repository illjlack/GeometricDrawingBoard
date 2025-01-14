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


    connect(canvas, &Canvas::selectedGeo, propertyEditor, &GeoPropertyEditor::setGeo);
    connect(propertyEditor, &GeoPropertyEditor::updateGeo, canvas, [this]() {
        canvas->update();
        });


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
    QAction* selectAction = toolBar->addAction(L("ѡ��ģʽ"));
    selectAction->setCheckable(true); // ����Ϊ��ѡ��
    QAction* drawPointAction = toolBar->addAction(L("���Ƶ�"));
    drawPointAction->setCheckable(true);
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
    actionGroup->addAction(selectAction);
    actionGroup->addAction(drawPointAction);
    actionGroup->addAction(drawSimpleLineAction);
    actionGroup->addAction(drawDoubleLineAction);
    actionGroup->addAction(drawParallelLineAction);
    actionGroup->addAction(drawTwoPointCircleAction);
    actionGroup->addAction(drawSimpleAreaAction);
    actionGroup->addAction(drawComplexAreaAction);
    actionGroup->setExclusive(true); // ����Ϊ����

    // Ĭ��ѡ�����߰�ť
    selectAction->setChecked(true);

    // �����źźͲ�
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
    GlobalStatusBar->showMessage(L("׼������"));
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

    setFocusPolicy(Qt::StrongFocus); // ����������ͼ����л���ȡ����
}

Canvas::~Canvas() {}

void Canvas::resetView()
{
    view.reset(); // ����Ϊ��λ����
    update();     // �����ػ�
}

void Canvas::scaleView(qreal scaleFactor)
{
    QTransform scale;
    scale.scale(scaleFactor, scaleFactor);
    view *= scale; // �ۻ�����
    update();      // �����ػ�
}

void Canvas::translateView(qreal dx, qreal dy)
{
    QTransform translation;
    translation.translate(dx, dy);
    view *= translation; // �ۻ�ƽ��
    update();            // �����ػ�
}

// ��Ӷ��������ӳ���
void Canvas::pushGeo(Geo* geo)
{
    if (!geo)
        return;

    geoList.push_back(geo);

    auto it = std::prev(geoList.end());
    geoMap[geo] = it;
}

// �������ӳ������Ƴ�����
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

    painter.setTransform(view); // Ӧ����ͼ����
    
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    painter.setRenderHint(QPainter::Antialiasing); // ���ÿ����
    
    // ���ް�ɫ����������һ�������ӿڷ�Χ�ľ�����Ϊ����
    QRectF extendedRect = QRectF(
        view.inverted().map(QPointF(0, 0)),                        // ���Ͻ�
        view.inverted().map(QPointF(width(), height()))           // ���½�
    ).adjusted(-10000, -10000, 10000, 10000);                     // ���췶Χ
    painter.fillRect(extendedRect, Qt::white);


    // ���������е�������״������
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

    qreal step = 50.0; // ƽ�Ʋ���
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

// ǿ����ɻ���
void Canvas::modeChange()
{
    if (currentSelectGeo)
    {
        if (currentSelectGeo->isStateDrawing())
        {
            currentSelectGeo->completeDrawing();
        }

        // ������Ϸ���ɾ��
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

    if (event->button() == Qt::LeftButton) // ����Ƿ������
    {
        isLeftButtonPressed = true;
        hitPoint = event -> pos(); //  ��Ļ���꣬��Ϊ�ƶ���ʱ���Ѿ������ˣ��ƶ������Ų�����һ���ۻ��ģ�
    }

    int selected = false;
    // �����ѡ��ģʽ�����������
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
    // �ձ�ʾû��ѡ�ж���
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
        .arg(pos.x(), 0, 'f', 2)  // 'f' ��ʾ�̶����ʽ��2 ��ʾ��ʾС�������λ
        .arg(pos.y(), 0, 'f', 2);
    GlobalStatusBar->showMessage(coordinateText);

    if (currentSelectGeo)
    {
        // �����ѡ��ģʽ�����Խ�����ק
        if (DrawMode::DrawSelect == GlobalDrawMode && isLeftButtonPressed)
        {
            // Ϊ�˱�����ק��ʱ��һֱ�㻺�����������ȹ���
            GeoParameters geoParameters = currentSelectGeo->getGeoParameters();
            if (geoParameters.bufferVisible)
            {
                geoParameters.bufferVisible = false;
                //currentSelectGeo->setGeoParameters(geoParameters);
                //emit selectedGeo(currentSelectGeo); // ����һ��uis
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

    if (event->button() == Qt::LeftButton) // ����Ƿ������
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
    // ʹ����ͼ�������任���¼�λ��ӳ��Ϊ��������
    return view.inverted().map(pos);
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

    // ����������
    bufferVisibleCheckBox = new QCheckBox(L("��ʾ������"), this);

    bufferCalculationModeComboBox = new QComboBox(this);
    bufferCalculationModeComboBox->addItems({ L("ʸ��"), L("դ��") });

    bufferDistanceSpinBox = new QSpinBox(this);
    bufferDistanceSpinBox->setRange(1, 1000);

    // ������������
    bufferLineStyleComboBox = new QComboBox(this);
    bufferLineStyleComboBox->addItems({ L("ʵ��"), L("����") });

    bufferLineWidthSpinBox = new QSpinBox(this);
    bufferLineWidthSpinBox->setRange(1, 50);

    bufferLineColorButton = new QPushButton(L("ѡ�񻺳�������ɫ"), this);

    bufferLineDashPatternSpinBox = new QSpinBox(this);
    bufferLineDashPatternSpinBox->setRange(1, 20);

    // ������������
    bufferFillColorButton = new QPushButton(L("ѡ�񻺳��������ɫ"), this);

    // �������߿�����
    bufferHasBorderCheckBox = new QCheckBox(L("�������Ƿ��б߿�"), this);

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
    formLayout->addRow(L("��ʾ������:"), bufferVisibleCheckBox);
    formLayout->addRow(L("���������㷽ʽ:"), bufferCalculationModeComboBox);
    formLayout->addRow(L("����������:"), bufferDistanceSpinBox);
    formLayout->addRow(L("����������ʽ:"), bufferLineStyleComboBox);
    formLayout->addRow(L("�������߿�:"), bufferLineWidthSpinBox);
    formLayout->addRow(L("����������ɫ:"), bufferLineColorButton);
    formLayout->addRow(L("���������߶γ�:"), bufferLineDashPatternSpinBox);
    formLayout->addRow(L("�����������ɫ:"), bufferFillColorButton);
    formLayout->addRow(L("�������б߿�:"), bufferHasBorderCheckBox);

    layout->addLayout(formLayout);

    // ��ʼ��
    applyGlobalSettings();

    // �źŲ۰�
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
    // ���Ҳ������������£���һ������Ƿ����л�����������л��������������任�������ø�����
    isSwitchingObject = true;

    currentGeo = geo;
    if (geo)setGeoParameters(geo->getGeoParameters());
    else applyGlobalSettings();
    
    isSwitchingObject = false;
}

void GeoPropertyEditor::onColorButtonClicked() {
    if (isSwitchingObject) return; // �����л������ڼ�,������

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
        return; // δƥ�䵽��Ӧ�İ�ť
    }

    *selectedColor = QColorDialog::getColor(
        *selectedColor,                // ��ʼ��ɫ
        this,                          // ������
        L("ѡ����ɫ"),                 // �Ի������
        QColorDialog::ShowAlphaChannel // ����͸����ѡ��
    );

    if (!selectedColor->isValid()) {
        return; // ����û�ȡ������ɫѡ�����˳�
    }

    // ���ð�ť������ɫ
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
        // ����ȫ��Ĭ����ɫ
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
    if (isSwitchingObject)return; // �����л������ڼ�,������

    if (currentGeo)
    {
        // �����ǰ���� Geo ���������������
        currentGeo->setGeoParameters(getGeoParameters());
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

        // ����������
        GlobalBufferVisible = bufferVisibleCheckBox->isChecked();  // �������Ƿ�ɼ�
        GlobalBufferCalculationMode = stringToBufferCalculationMode(bufferCalculationModeComboBox->currentText());  // ����������ģʽ
        GlobalBufferDistance = bufferDistanceSpinBox->value();  // ����������

        // ����������ʽ����
        GlobalBufferLineStyle = stringToLineStyle(bufferLineStyleComboBox->currentText());  // ����������ʽ
        GlobalBufferLineWidth = bufferLineWidthSpinBox->value();  // �������߿�
        GlobalBufferLineDashPattern = bufferLineDashPatternSpinBox->value();  // ���������߶γ���

        // ��ɫ����
        GlobalPointColor = currentPointColor;  // ����ɫ
        GlobalLineColor = currentLineColor;  // ����ɫ
        GlobalFillColor = currentFillColor;  // �����ɫ
        GlobalBufferLineColor = currentBufferLineColor;  // ����������ɫ
        GlobalBufferFillColor = currentBufferFillColor;  // �����������ɫ
    }
    emit updateGeo();
}


void GeoPropertyEditor::applyGlobalSettings()
{
    // ��ȫ�ֱ����ж�ȡ��Ӧ�õ������

// ������ɫ������ԺͰ�ť��ʽ
    currentPointColor = QColor(GlobalPointColor); // ��ǰ����ɫ
    pointColorButton->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);")
        .arg(currentPointColor.red())
        .arg(currentPointColor.green())
        .arg(currentPointColor.blue())
        .arg(currentPointColor.alpha()));

    currentLineColor = QColor(GlobalLineColor); // ��ǰ����ɫ
    lineColorButton->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);")
        .arg(currentLineColor.red())
        .arg(currentLineColor.green())
        .arg(currentLineColor.blue())
        .arg(currentLineColor.alpha()));

    currentFillColor = QColor(GlobalFillColor); // ��ǰ�����ɫ
    fillColorButton->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);")
        .arg(currentFillColor.red())
        .arg(currentFillColor.green())
        .arg(currentFillColor.blue())
        .arg(currentFillColor.alpha()));

    currentBufferLineColor = QColor(GlobalBufferLineColor); // ��ǰ����������ɫ
    bufferLineColorButton->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);")
        .arg(currentBufferLineColor.red())
        .arg(currentBufferLineColor.green())
        .arg(currentBufferLineColor.blue())
        .arg(currentBufferLineColor.alpha()));

    currentBufferFillColor = QColor(GlobalBufferFillColor); // ��ǰ����������ɫ
    bufferFillColorButton->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);")
        .arg(currentBufferFillColor.red())
        .arg(currentBufferFillColor.green())
        .arg(currentBufferFillColor.blue())
        .arg(currentBufferFillColor.alpha()));


    // ����������
    pointShapeComboBox->setCurrentText(pointShapeToString(GlobalPointShape)); // ����״

    // ����������
    lineStyleComboBox->setCurrentText(lineStyleToString(GlobalLineStyle)); // ����ʽ
    lineWidthSpinBox->setValue(GlobalLineWidth); // �߿�
    lineDashPatternSpinBox->setValue(GlobalLineDashPattern); // ���߶γ���

    // ����������
    // �����ɫ������ɫ��������

    // ������������
    splineOrderSpinBox->setValue(GlobalSplineOrder); // ��������
    splineNodeCountSpinBox->setValue(GlobalSplineNodeCount); // �����ڵ���
    stepsSpinBox->setValue(GlobalSteps); // �����ܶ�

    // �������������
    bufferVisibleCheckBox->setChecked(GlobalBufferVisible); // �������ɼ���
    bufferCalculationModeComboBox->setCurrentText(bufferCalculationModeToString(GlobalBufferCalculationMode)); // ����������ģʽ
    bufferDistanceSpinBox->setValue(GlobalBufferDistance); // ����������

    // ����������������
    bufferLineStyleComboBox->setCurrentText(lineStyleToString(GlobalBufferLineStyle)); // ����������ʽ
    bufferLineWidthSpinBox->setValue(GlobalBufferLineWidth); // �������߿�
    bufferLineDashPatternSpinBox->setValue(GlobalBufferLineDashPattern); // ���������߶γ���

    // �������߿���������
    bufferHasBorderCheckBox->setChecked(GlobalBufferHasBorder); // �Ƿ��б߿�
}

void GeoPropertyEditor::setGeoParameters(const GeoParameters& params)
{
    // ���� UI �����ֵ
    lineStyleComboBox->setCurrentText(lineStyleToString(params.lineStyle));
    lineWidthSpinBox->setValue(params.lineWidth);
    lineDashPatternSpinBox->setValue(params.lineDashPattern);
    splineOrderSpinBox->setValue(params.splineOrder);
    splineNodeCountSpinBox->setValue(params.splineNodeCount);
    stepsSpinBox->setValue(params.steps);
    pointShapeComboBox->setCurrentText(pointShapeToString(params.pointShape));

    // ���û�����������
    bufferVisibleCheckBox->setChecked(params.bufferVisible);  // �������ɼ���
    bufferCalculationModeComboBox->setCurrentText(bufferCalculationModeToString(params.bufferCalculationMode));  // ����������ģʽ
    bufferDistanceSpinBox->setValue(params.bufferDistance);  // ����������

    // ���û������ߵ�����
    bufferLineStyleComboBox->setCurrentText(lineStyleToString(params.bufferLineStyle)); // ����������ʽ
    bufferLineWidthSpinBox->setValue(params.bufferLineWidth); // �������߿�
    bufferLineDashPatternSpinBox->setValue(params.bufferLineDashPattern); // ���������߶γ���
    bufferHasBorderCheckBox->setChecked(params.bufferHasBorder); // �Ƿ��б߿�

    // ������ɫ��ť����
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

    // �� UI �����ȡֵ
    // ��ȡ��ɫֵ
    params.pointColor = currentPointColor;
    params.lineColor = currentLineColor;
    params.fillColor = currentFillColor;
    params.bufferLineColor = currentBufferLineColor;
    params.bufferFillColor = currentBufferFillColor;

    // ��ȡ����������
    params.pointShape = stringToPointShape(pointShapeComboBox->currentText());

    // ��ȡ����������
    params.lineStyle = stringToLineStyle(lineStyleComboBox->currentText());
    params.lineWidth = lineWidthSpinBox->value();
    params.lineDashPattern = lineDashPatternSpinBox->value();

    // ��ȡ������������
    params.splineOrder = splineOrderSpinBox->value();
    params.splineNodeCount = splineNodeCountSpinBox->value();
    params.steps = stepsSpinBox->value();

    // ��ȡ�������������
    params.bufferVisible = bufferVisibleCheckBox->isChecked();  // �������ɼ���
    params.bufferCalculationMode = stringToBufferCalculationMode(bufferCalculationModeComboBox->currentText());  // ����������ģʽ
    params.bufferDistance = bufferDistanceSpinBox->value();  // ����������

    // ��ȡ���������������
    params.bufferLineStyle = stringToLineStyle(bufferLineStyleComboBox->currentText());
    params.bufferLineWidth = bufferLineWidthSpinBox->value();
    params.bufferLineDashPattern = bufferLineDashPatternSpinBox->value();

    // ��ȡ�������߿�����
    params.bufferHasBorder = bufferHasBorderCheckBox->isChecked();

    return params;
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

// �ı���ö�ٵ�ӳ�䣺����������ģʽ
BufferCalculationMode stringToBufferCalculationMode(const QString& modeText)
{
    static const QMap<QString, BufferCalculationMode> bufferCalculationModeMap =
    {
        {L("ʸ��"), BufferCalculationMode::Vector},
        {L("դ��"), BufferCalculationMode::Raster}
    };
    return bufferCalculationModeMap.value(modeText, BufferCalculationMode::Vector); // Ĭ�Ϸ��� Vector
}

// ö�ٵ��ı���ӳ�䣺����������ģʽ
QString bufferCalculationModeToString(BufferCalculationMode mode)
{
    switch (mode)
    {
    case BufferCalculationMode::Vector:
        return L("ʸ��");
    case BufferCalculationMode::Raster:
        return L("դ��");
    default:
        return L("δ֪");
    }
}