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
//    bool openFile(const QString& filePath);  // 打开 Shapefile 文件
//    bool saveFile(const QString& filePath);  // 保存 Shapefile 文件
//    void addGeometry(const QString& geometry, const std::unordered_map<QString, QString>& attr);  // 添加几何数据
//    QStringList getAllGeometries() const;                       // 获取所有几何数据
//    void addField(const QString& fieldName);                    // 添加字段
//    QStringList getFields() const;                              // 获取所有字段
//
//private:
//    QList<QString> geometries;                                  // 存储几何数据
//    QList<std::unordered_map<QString, QString>> attributes;     // 存储属性数据
//    QStringList fields;                                         // 存储字段
//    GDALDataset* dataset;                                       // GDAL 数据集，管理读取和写入的 Shapefile 文件
//};