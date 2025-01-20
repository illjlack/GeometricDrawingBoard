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
    bool openFile(const QString& filePath);  // �� Shapefile �ļ�
    bool saveFile(const QString& filePath);
    void clearGeometry();
    // ���� Shapefile �ļ�
    void addGeometry(const QString& geometry, const QMap<QString, QVariant>& attr);  // ��Ӽ�������
    QVector<QString> getAllGeometries() const;                       // ��ȡ���м�������
    void addField(const QString& fieldName);                    // ����ֶ�
    QVector<QString> getFields() const;                              // ��ȡ�����ֶ�

    void initializeFields();

    using Line = QVector<QPointF>;                   // ��ʾһ����
    using Polygon = QVector<Line>;                 // ��ʾһ����
    using Polygons = QVector<Polygon>;             // ��ʾ�漯��

    QString lineToWKT(const Line& line) const;
    Line wktToLine(const QString& wkt) const;

    void GeoParametersToAttributes(const GeoParameters& params, QMap<QString, QVariant>& attributes);
    void AttributesToGeoParameters(const QMap<QString, QVariant>& attributes, GeoParameters& params);

    void saveGeo(Geo* geo);

    Geo* loadGeo(const QMap<QString, QVariant>& attributes, const QString& geometryWKT);

private:
    QVector<QString> geometries;                                  // �洢��������
    QVector<QMap<QString, QVariant>> attributes;     // �洢��������
    QVector<QString> fields;                                         // �洢�ֶ�
    GDALDataset* dataset;                                       // GDAL ���ݼ��������ȡ��д��� Shapefile �ļ�
};