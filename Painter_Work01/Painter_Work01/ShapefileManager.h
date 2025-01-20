//#pragma once
//
//
//#include <QString>
//#include <QStringList>
//#include <QVector>
//#include <QDebug>
//#include <unordered_map>
//#include <gdal.h>
//#include <ogrsf_frmts.h>
//
//class ShapefileManager
//{
//public:
//    ShapefileManager();
//    bool openFile(const QString& filePath);  // �� Shapefile �ļ�
//    bool saveFile(const QString& filePath);  // ���� Shapefile �ļ�
//    void addGeometry(const QString& geometry, const std::unordered_map<QString, QString>& attr);  // ��Ӽ�������
//    QStringList getAllGeometries() const;                       // ��ȡ���м�������
//    void addField(const QString& fieldName);                    // ����ֶ�
//    QStringList getFields() const;                              // ��ȡ�����ֶ�
//
//private:
//    QList<QString> geometries;                                  // �洢��������
//    QList<std::unordered_map<QString, QString>> attributes;     // �洢��������
//    QStringList fields;                                         // �洢�ֶ�
//    GDALDataset* dataset;                                       // GDAL ���ݼ��������ȡ��д��� Shapefile �ļ�
//};