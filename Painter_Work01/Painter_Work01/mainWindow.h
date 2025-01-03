#pragma once

#include <QtWidgets/QMainWindow>
#include <QSlider>
#include <QPushButton>
#include <QColorDialog>

#include "Canvas.h"

class DrawingSettings;
class PointSettings;

class mainWindow : public QMainWindow
{
    Q_OBJECT

public:
    mainWindow(QWidget *parent = nullptr);
    ~mainWindow();

private:
    void createMenuBar();   // 创建菜单栏
    void createToolBar();   // 创建工具栏
    void createSideBar();   // 侧边栏
    void createStatusBar(); // 创建状态栏

   Canvas* canvas;   //绘制区域

   QDockWidget* sideBar; // 侧边栏
   QWidget* sideBarWidget; // 侧边栏内容
   DrawingSettings* currentSettings; // 当前显示的侧边栏

   PointSettings* pointSettings;
   //LineSettings* lineSettings;

private slots:
    void openFile();        // 打开文件
    void saveFile();        // 保存文件
    void exportToShp();     // 导出到 SHP 文件
    void showAbout();       // 显示关于信息
};



// 绘制设置 和 图形修改
// 可以序列化
// ==================================================== DrawingSettings

class DrawingSettings : public QWidget {
    Q_OBJECT
public:
    DrawingSettings(QWidget* parent = nullptr);
    virtual ~DrawingSettings() = default;
    virtual void reset() = 0; // 重置设置
};

// ==================================================== PointSettings

class PointSettings : public DrawingSettings {
    Q_OBJECT
public:
    PointSettings(QWidget* parent = nullptr);
    void reset() override;

private slots:
    void onColorButtonClicked();

private:
    QSlider* sizeSlider;
    QPushButton* colorButton;
    QColor currentColor;
    QColor defaultColor = Qt::black;
};

// ===================================================== LineSettings

//class LineSettings : public DrawingSettings {
//    Q_OBJECT
//public:
//    explicit LineSettings(QWidget* parent = nullptr);
//    void reset() override;
//
//private slots:
//    void onSolidLineSelected();
//    void onDashedLineSelected();
//
//private:
//    QSlider* widthSlider;
//    QPushButton* solidLineButton;
//    QPushButton* dashedLineButton;
//};
