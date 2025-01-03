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
    void createMenuBar();   // �����˵���
    void createToolBar();   // ����������
    void createStatusBar(); // ����״̬��

   Canvas* canvas;   //��������

private slots:
    void openFile();        // ���ļ�
    void saveFile();        // �����ļ�
    void exportToShp();     // ������ SHP �ļ�
    void showAbout();       // ��ʾ������Ϣ
};
