#pragma once

#include <QtWidgets/QMainWindow>
#include "Canvas.h"

class mainWindow : public QMainWindow
{
    Q_OBJECT

public:
    mainWindow(QWidget *parent = nullptr);
    ~mainWindow();

private:
    void createMenuBar();   // 创建菜单栏
    void createToolBar();   // 创建工具栏
    void createStatusBar(); // 创建状态栏

   Canvas* canvas;   //绘制区域

private slots:
    void openFile();        // 打开文件
    void saveFile();        // 保存文件
    void exportToShp();     // 导出到 SHP 文件
    void showAbout();       // 显示关于信息
};
