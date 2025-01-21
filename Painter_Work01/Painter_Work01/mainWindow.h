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
    void createMenuBar();               // �����˵���
    void createToolBar();               // ����������
    void createNodeLineToolBar();       // �����ڵ����͹�����
    void createStatusBar();             // ����״̬��

    Canvas* canvas;   // ��������
    GeoPropertyEditor* geoEditor; // ���ô���
    GeoPropertyEditor* propertyEditor; // ���Դ���
    ShapefileManager* shapefileManager; // �ļ����ء�����

private slots:
    void openFile();        // ���ļ�
    void saveFile();        // �����ļ�
    void showAbout();       // ��ʾ������Ϣ
};

// ===================================================== Canvas
class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget* parent = nullptr);
    ~Canvas();

    void modeChange();         // ǿ����ɻ���

    // ������ͼ����
    void resetView();                         // ������ͼ����Ϊ��λ����
    void scaleView(qreal scaleFactor);        // ��ͼ����
    void translateView(qreal dx, qreal dy);   // ��ͼƽ��

    void pushGeo(Geo* shape);           // �������
    void getGeos(QVector<Geo*>& geos);
protected:
    void removeGeo(Geo* geo);           // �Ƴ�����

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
    Geo* currentSelectGeo = nullptr;    // ��ǰѡ�е�geo
    bool isLeftButtonPressed = false;

    // ��������ά�� Geo �����˳��
    std::list<Geo*> geoList;
    // ӳ�� Geo ָ�뵽�����ж�ӦԪ�صĵ�����
    std::map<Geo*, std::list<Geo*>::iterator> geoMap;
    QTransform view; // ��ͼ�����������ź�ƽ��
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
    void setGeo(Geo* geo); // ���õ�ǰ Geo ����
    void applyGlobalSettings(); // Ӧ��ȫ�����õ��༭��

    void setGeoParameters(const GeoParameters& params); // ���� UI ����
    GeoParameters getGeoParameters() const; // ��ȡ UI ����

signals:
    void updateGeo();

private slots:
    void onColorButtonClicked(); // ��ɫѡ��ť�����
    void onValueChanged(); // ����ֵ�ı��

private:
    Geo* currentGeo; // ��ǰ�༭�� Geo ����

    bool isSwitchingObject = false; // ��ʾ�Ƿ����л������ڼ�

    // �������ֶ�
    QComboBox* pointShapeComboBox; // ����״������
    QPushButton* pointColorButton; // ����ɫѡ��ť

    // �������ֶ�
    QComboBox* lineStyleComboBox; // ����ʽ������
    QSpinBox* lineWidthSpinBox; // �߿������
    QSpinBox* lineDashPatternSpinBox; // ���߶γ��ȵ�����
    QPushButton* lineColorButton; // ����ɫѡ��ť

    // �������ֶ�
    QPushButton* fillColorButton; // �����ɫѡ��ť

    // ���������ֶ�
    QSpinBox* splineOrderSpinBox; // ��������������
    QSpinBox* splineNodeCountSpinBox; // �����ڵ���������
    QSpinBox* stepsSpinBox; // �����ܶȵ�����

    // ����������ֶ�
    QCheckBox* bufferVisibleCheckBox; // �������ɼ��Ը�ѡ��
    QComboBox* bufferCalculationModeComboBox; // ����������ģʽ������
    QSpinBox* bufferDistanceSpinBox; // ���������������

    // �������ߵ������ֶ�
    QComboBox* bufferLineStyleComboBox; // ����������ʽ������
    QSpinBox* bufferLineWidthSpinBox; // �������߿������
    QSpinBox* bufferLineDashPatternSpinBox; // ���������߶γ��ȵ�����
    QPushButton* bufferLineColorButton; // ����������ɫѡ��ť

    // ��������������ֶ�
    QPushButton* bufferFillColorButton; // �����������ɫѡ��ť

    // �������߿�������ֶ�
    QCheckBox* bufferHasBorderCheckBox; // �Ƿ��б߿�ѡ��

    // ��ǰ��ɫ״̬
    QColor currentPointColor; // ��ǰ����ɫ
    QColor currentLineColor; // ��ǰ����ɫ
    QColor currentFillColor; // ��ǰ�����ɫ
    QColor currentBufferLineColor; // ��ǰ����������ɫ
    QColor currentBufferFillColor; // ��ǰ�����������ɫ

};



// �ı���ö�ٵ�ӳ��
LineStyle stringToLineStyle(const QString& styleText);
PointShape stringToPointShape(const QString& shapeText);

QString lineStyleToString(LineStyle style);
QString pointShapeToString(PointShape shape);

BufferCalculationMode stringToBufferCalculationMode(const QString& modeText);
QString bufferCalculationModeToString(BufferCalculationMode mode);