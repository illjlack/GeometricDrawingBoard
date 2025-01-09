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

#include "Geo.h"


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

private slots:
    void openFile();        // 打开文件
    void saveFile();        // 保存文件
    void exportToShp();     // 导出到 SHP 文件
    void showAbout();       // 显示关于信息
};

// ===================================================== Canvas
class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget* parent = nullptr);
    ~Canvas();

    void CompleteDrawing();         // 强制完成绘制

protected:
    void pushShape(Geo* shape);     // 加入对象

    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:

    Geo* currentDrawGeo = nullptr;
    std::vector<Geo*> vec;
};


// ===================================================== GeoPropertyEditor

class GeoPropertyEditor : public QWidget {
    Q_OBJECT

public:
    explicit GeoPropertyEditor(QWidget* parent = nullptr);
    void setGeo(Geo* geo);
    void applyGlobalSettings();
signals:
    void geoUpdated();  // 通知属性更改

private slots:
    void onColorButtonClicked();
    void onValueChanged();

private:
    Geo* currentGeo;

    // 属性字段
    QComboBox* pointShapeComboBox;
    QPushButton* pointColorButton;

    QComboBox* lineStyleComboBox;
    QSpinBox* lineWidthSpinBox;
    QPushButton* lineColorButton;
    QSpinBox* lineDashPatternSpinBox;

    QPushButton* fillColorButton;

    QSpinBox* splineOrderSpinBox;
    QSpinBox* splineNodeCountSpinBox;
    QSpinBox* stepsSpinBox;

    QColor currentPointColor;
    QColor currentLineColor;
    QColor currentFillColor;
};

// 文本到枚举的映射
LineStyle stringToLineStyle(const QString& styleText);
PointShape stringToPointShape(const QString& shapeText);


QString lineStyleToString(LineStyle style);

QString pointShapeToString(PointShape shape);
