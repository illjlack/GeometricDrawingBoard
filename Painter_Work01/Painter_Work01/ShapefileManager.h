#pragma once


#include <QString>
#include <QStringList>
#include <QVector>
#include <QDebug>
#include <unordered_map>
#include <gdal.h>
#include <ogrsf_frmts.h>
#include "Geo.h"
#include "comm.h"

class ShapefileManager
{
public:
    ShapefileManager();
    bool openFile(const QString& filePath);  // 打开 Shapefile 文件
    bool saveFile(const QString& filePath);
    void clearGeometry();
    // 保存 Shapefile 文件
    void addGeometry(const QString& geometry, const QMap<QString, QVariant>& attr);  // 添加几何数据
    QVector<QString> getAllGeometries() const;                       // 获取所有几何数据
    void addField(const QString& fieldName);                    // 添加字段
    QVector<QString> getFields() const;                              // 获取所有字段

    void initializeFields();

    using Line = QVector<QPointF>;                   // 表示一条线
    using Polygon = QVector<Line>;                 // 表示一个面
    using Polygons = QVector<Polygon>;             // 表示面集合

    QString lineToWKT(const Line& line) const;
    Line wktToLine(const QString& wkt) const;

    void GeoParametersToAttributes(const GeoParameters& params, QMap<QString, QVariant>& attributes);
    void AttributesToGeoParameters(const QMap<QString, QVariant>& attributes, GeoParameters& params);

    void saveGeo(Geo* geo);

    Geo* loadGeo(const QMap<QString, QVariant>& attributes, const QString& geometryWKT);

private:
    QVector<QString> geometries;                                  // 存储几何数据
    QVector<QMap<QString, QVariant>> attributes;     // 存储属性数据
    QVector<QString> fields;                                         // 存储字段
    GDALDataset* dataset;                                       // GDAL 数据集，管理读取和写入的 Shapefile 文件
};