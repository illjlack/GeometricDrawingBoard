#pragma once

#include <QtWidgets/QMainWindow>
#include <QSlider>
#include <QPushButton>
#include <QColorDialog>
#include <QDockWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QTransform>
#include "Geo.h"
#include "ShapefileManager.h"

class Canvas;
class GeoPropertyEditor;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    void createMenuBar();               // 创建菜单栏
    void createToolBar();               // 创建工具栏
    void createNodeLineToolBar();       // 创建节点线型工具栏
    void createStatusBar();             // 创建状态栏

    Canvas* canvas;   // 绘制区域
    GeoPropertyEditor* geoEditor; // 配置窗口
    GeoPropertyEditor* propertyEditor; // 属性窗口
    ShapefileManager* shapefileManager; // 文件加载、保存

private slots:
    void openFile();        // 打开文件
    void saveFile();        // 保存文件
    void showAbout();       // 显示关于信息
};

// ===================================================== Canvas
class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget* parent = nullptr);
    ~Canvas();

    void modeChange();         // 强制完成绘制

    // 设置视图矩阵
    void resetView();                         // 重置视图矩阵为单位矩阵
    void scaleView(qreal scaleFactor);        // 视图缩放
    void translateView(qreal dx, qreal dy);   // 视图平移

    void pushGeo(Geo* shape);           // 加入对象
    void getGeos(QVector<Geo*>& geos);
protected:
    void removeGeo(Geo* geo);           // 移除对象

    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    QPointF mapPoint(const QPointF& pos) const;

signals:
    void selectedGeo(Geo* geo);

private:
    Geo* currentSelectGeo = nullptr;    // 当前选中的geo
    bool isLeftButtonPressed = false;

    // 链表用于维护 Geo 对象的顺序
    std::list<Geo*> geoList;
    // 映射 Geo 指针到链表中对应元素的迭代器
    std::map<Geo*, std::list<Geo*>::iterator> geoMap;
    QTransform view; // 视图矩阵（用于缩放和平移
    QPointF hitPoint;
};

// ===================================================== GeoPropertyEditor

class CoordinateInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CoordinateInputDialog(QWidget* parent = nullptr);
    QPointF getCoordinates() const;

private slots:
    void onOkClicked();

private:
    QLineEdit* xEdit;
    QLineEdit* yEdit;
};

// ===================================================== GeoPropertyEditor

class GeoPropertyEditor : public QWidget {
    Q_OBJECT

public:
    explicit GeoPropertyEditor(QWidget* parent = nullptr);
    void setGeo(Geo* geo); // 设置当前 Geo 对象
    void applyGlobalSettings(); // 应用全局设置到编辑器

    void setGeoParameters(const GeoParameters& params); // 设置 UI 参数
    GeoParameters getGeoParameters() const; // 获取 UI 参数

signals:
    void updateGeo();

private slots:
    void onColorButtonClicked(); // 颜色选择按钮点击槽
    void onValueChanged(); // 属性值改变槽

private:
    Geo* currentGeo; // 当前编辑的 Geo 对象

    bool isSwitchingObject = false; // 表示是否处于切换对象期间

    // 点属性字段
    QComboBox* pointShapeComboBox; // 点形状下拉框
    QPushButton* pointColorButton; // 点颜色选择按钮

    // 线属性字段
    QComboBox* lineStyleComboBox; // 线样式下拉框
    QSpinBox* lineWidthSpinBox; // 线宽调整框
    QSpinBox* lineDashPatternSpinBox; // 虚线段长度调整框
    QPushButton* lineColorButton; // 线颜色选择按钮

    // 面属性字段
    QPushButton* fillColorButton; // 填充颜色选择按钮

    // 样条属性字段
    QSpinBox* splineOrderSpinBox; // 样条阶数调整框
    QSpinBox* splineNodeCountSpinBox; // 样条节点数调整框
    QSpinBox* stepsSpinBox; // 曲线密度调整框

    // 缓冲区相关字段
    QCheckBox* bufferVisibleCheckBox; // 缓冲区可见性复选框
    QComboBox* bufferCalculationModeComboBox; // 缓冲区计算模式下拉框
    QSpinBox* bufferDistanceSpinBox; // 缓冲区距离调整框

    // 缓冲区线的属性字段
    QComboBox* bufferLineStyleComboBox; // 缓冲区线样式下拉框
    QSpinBox* bufferLineWidthSpinBox; // 缓冲区线宽调整框
    QSpinBox* bufferLineDashPatternSpinBox; // 缓冲区虚线段长度调整框
    QPushButton* bufferLineColorButton; // 缓冲区线颜色选择按钮

    // 缓冲区面的属性字段
    QPushButton* bufferFillColorButton; // 缓冲区填充颜色选择按钮

    // 缓冲区边框的属性字段
    QCheckBox* bufferHasBorderCheckBox; // 是否有边框复选框

    // 当前颜色状态
    QColor currentPointColor; // 当前点颜色
    QColor currentLineColor; // 当前线颜色
    QColor currentFillColor; // 当前填充颜色
    QColor currentBufferLineColor; // 当前缓冲区线颜色
    QColor currentBufferFillColor; // 当前缓冲区填充颜色

};



// 文本到枚举的映射
LineStyle stringToLineStyle(const QString& styleText);
PointShape stringToPointShape(const QString& shapeText);

QString lineStyleToString(LineStyle style);
QString pointShapeToString(PointShape shape);

BufferCalculationMode stringToBufferCalculationMode(const QString& modeText);
QString bufferCalculationModeToString(BufferCalculationMode mode);