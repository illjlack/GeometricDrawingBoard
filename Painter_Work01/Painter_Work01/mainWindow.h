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
    void createMenuBar();   // �����˵���
    void createToolBar();   // ����������
    void createSideBar();   // �����
    void createStatusBar(); // ����״̬��

   Canvas* canvas;   //��������

   QDockWidget* sideBar; // �����
   QWidget* sideBarWidget; // ���������
   DrawingSettings* currentSettings; // ��ǰ��ʾ�Ĳ����

   PointSettings* pointSettings;
   //LineSettings* lineSettings;

private slots:
    void openFile();        // ���ļ�
    void saveFile();        // �����ļ�
    void exportToShp();     // ������ SHP �ļ�
    void showAbout();       // ��ʾ������Ϣ
};



// �������� �� ͼ���޸�
// �������л�
// ==================================================== DrawingSettings

class DrawingSettings : public QWidget {
    Q_OBJECT
public:
    DrawingSettings(QWidget* parent = nullptr);
    virtual ~DrawingSettings() = default;
    virtual void reset() = 0; // ��������
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
