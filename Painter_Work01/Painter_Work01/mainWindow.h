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
    void createMenuBar();               // �����˵���
    void createToolBar();               // ����������
    void createNodeLineToolBar();       // �����ڵ����͹�����
    void createStatusBar();             // ����״̬��

    Canvas* canvas;   // ��������
    GeoPropertyEditor* geoEditor; // ���ô���
    GeoPropertyEditor* propertyEditor; // ���Դ���

private slots:
    void openFile();        // ���ļ�
    void saveFile();        // �����ļ�
    void exportToShp();     // ������ SHP �ļ�
    void showAbout();       // ��ʾ������Ϣ
};

// ===================================================== Canvas
class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget* parent = nullptr);
    ~Canvas();

    void CompleteDrawing();         // ǿ����ɻ���

protected:
    void pushShape(Geo* shape);     // �������

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
    void geoUpdated();  // ֪ͨ���Ը���

private slots:
    void onColorButtonClicked();
    void onValueChanged();

private:
    Geo* currentGeo;

    // �����ֶ�
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

// �ı���ö�ٵ�ӳ��
LineStyle stringToLineStyle(const QString& styleText);
PointShape stringToPointShape(const QString& shapeText);


QString lineStyleToString(LineStyle style);

QString pointShapeToString(PointShape shape);
