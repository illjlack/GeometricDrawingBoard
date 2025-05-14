/*
#include "ShapefileManager.h"
#include "ogrsf_frmts.h"
#include <QtCore/qregularexpression.h>
#include <QDir>

ShapefileManager::ShapefileManager() : dataset(nullptr)
{
    // ��ʼ�� GDAL �⣨����һ�Σ�
    GDALAllRegister();
    initializeFields();

    qRegisterMetaType<Component>("Component");
    qRegisterMetaType<QVector<Component>>("QVector<Component>");
}



bool ShapefileManager::getGeos(QVector<Geo*>& geos)
{
    int n = geometries.size();
    assert(n == attributes.size());

    for (int i = 0; i < n; i++)
    {
        geos.append(loadGeo(attributes[i], geometries[i]));
    }
    return true;
}

bool ShapefileManager::openFile(const QString& shpFilePath)
{
    if (shpFilePath.isEmpty())
    {
        Log << L("�ļ�·��Ϊ�գ��޷����ļ�");
        return false;
    }

    QFileInfo fileInfo(shpFilePath);
    if (!fileInfo.exists() || fileInfo.suffix().toLower() != "shp")
    {
        Log << L("ָ�����ļ������ڻ���һ�� Shapefile �ļ���") << shpFilePath;
        return false;
    }

    // �� Shapefile �ļ�
    dataset = static_cast<GDALDataset*>(GDALOpenEx(shpFilePath.toUtf8().data(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr));
    if (dataset == nullptr)
    {
        Log << L("�޷��� Shapefile �ļ���") << shpFilePath;
        return false;
    }

    Log << L("�ɹ��� Shapefile �ļ���") << shpFilePath;

    // ��ȡ��һ����Ч��ͼ��
    OGRLayer* layer = nullptr;
    for (int i = 0; i < dataset->GetLayerCount(); ++i)
    {
        layer = dataset->GetLayer(i);
        if (layer != nullptr)
        {
            break;
        }
    }

    if (layer == nullptr)
    {
        Log << L("�޷���ȡͼ��");
        GDALClose(dataset);
        dataset = nullptr;
        return false;
    }

    // ���֮ǰ������
    geometries.clear();
    attributes.clear();

    // ����ͼ���е�Ҫ��
    OGRFeature* feature = nullptr;
    layer->ResetReading();

    if (layer->GetFeatureCount() == 0) {
        Log << L("ͼ����û��Ҫ��");
        GDALClose(dataset);  // ȷ��������ر��ļ�
        dataset = nullptr;
        return false;
    }

    while ((feature = layer->GetNextFeature()) != nullptr)
    {
        // ��ȡ�������ݲ�ת��Ϊ WKT ��ʽ
        OGRGeometry* geometry = feature->GetGeometryRef();
        if (geometry != nullptr)
        {
            char* wkt = nullptr;
            OGRErr err = geometry->exportToWkt(&wkt);
            if (err == OGRERR_NONE && wkt != nullptr)
            {
                geometries.append(QString::fromUtf8(wkt));
                CPLFree(wkt);
            }
            else
            {
                Log << L("�޷�ת����������Ϊ WKT ��ʽ");
            }
        }

        // ��ȡ��������
        QMap<QString, QVariant> attr;
        for (int i = 0; i < feature->GetFieldCount(); ++i)
        {
            OGRFieldDefn* fieldDefn = feature->GetFieldDefnRef(i);
            if (fieldDefn != nullptr)
            {
                QString fieldName = QString::fromUtf8(fieldDefn->GetNameRef());
                QString fieldValue = QString::fromUtf8(feature->GetFieldAsString(i));
                attr[fieldName] = fieldValue;
            }
        }
        attributes.append(attr);

        // ���ٵ�ǰҪ��
        OGRFeature::DestroyFeature(feature);
    }

    GDALClose(dataset);  // ȷ���ļ��ں�������ʱ���ر�
    dataset = nullptr;

    return true;
}



bool ShapefileManager::saveFile(const QString& folderPath)
{
    if (folderPath.isEmpty())
    {
        Log << L("�ļ���·��Ϊ�գ��޷������ļ�");
        return false;
    }

    QDir folder(folderPath);
    if (!folder.exists() && !folder.mkpath("."))
    {
        Log << L("�޷������ļ��У�") << folderPath;
        return false;
    }

    QString shpFilePath = folder.filePath("main.shp");

    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
    if (driver == nullptr)
    {
        Log << L("�޷���ȡ Shapefile ����");
        return false;
    }

    // ɾ�������ļ���������ڣ�
    if (QFile::exists(shpFilePath))
    {
        if (driver->Delete(shpFilePath.toUtf8().data()) != CE_None)
        {
            Log << L("�޷�ɾ������ Shapefile �ļ���") << shpFilePath;
            return false;
        }
    }

    dataset = driver->Create(shpFilePath.toUtf8().data(), 0, 0, 0, GDT_Unknown, nullptr);
    if (dataset == nullptr)
    {
        Log << L("�޷����� Shapefile �ļ���") << shpFilePath;
        return false;
    }

    OGRLayer* layer = dataset->CreateLayer("Layer1", nullptr, wkbLineString, nullptr);
    if (layer == nullptr)
    {
        Log << L("�޷�����ͼ��");
        GDALClose(dataset);
        return false;
    }

    for (const auto& field : fields)
    {
        OGRFieldDefn fieldDefn(field.toUtf8().data(), OFTString);
        layer->CreateField(&fieldDefn);
    }

    for (int i = 0; i < geometries.size(); ++i)
    {
        OGRFeature* feature = OGRFeature::CreateFeature(layer->GetLayerDefn());

        OGRGeometry* geometry = nullptr;
        std::string wktStr = geometries[i].toStdString();
        const char* wkt = wktStr.c_str();
        // ������ͶӰ������ϵ
        OGRSpatialReference spatialRef;
        spatialRef.SetLocalCS("Non-Geographic");

        // ���� WKT ����ʱָ������ϵ
        OGRErr err = OGRGeometryFactory::createFromWkt(wkt, &spatialRef, &geometry);

        // ����ʧ��
        if (err != OGRERR_NONE || geometry == nullptr)
        {
            // ���ݴ�����빹��������Ϣ
            QString errMsg = QString("OGRErr code: %1").arg(err);

            // ��¼��־�����Ӹ�����ϸ��Ϣ
            Log << L("�޷����� WKT �������ݣ�") << geometries[i]
                << L(" �����룺") << errMsg;

            // ���� feature ��������ǰѭ��
            OGRFeature::DestroyFeature(feature);
            continue;
        }

        // ���ü������ݵ� feature
        feature->SetGeometry(geometry);

        // �����ֶ�
        for (auto it = attributes[i].begin(); it != attributes[i].end(); ++it)
        {
            feature->SetField(it.key().toUtf8().data(), it.value().toString().toUtf8().data());
        }

        // ���� feature ��ͼ��
        OGRErr featureErr = layer->CreateFeature(feature);
        if (featureErr != OGRERR_NONE)
        {
            Log << L("�޷�����Ҫ�أ������룺") << featureErr;
            OGRFeature::DestroyFeature(feature);
            continue;
        }
        OGRFeature::DestroyFeature(feature);
    }

    GDALClose(dataset);

    Log << L("�ļ�����ɹ���") << shpFilePath;

    return true;
}


void ShapefileManager::addField(const QString& fieldName)
{
    // ����ֶ���
    fields.append(fieldName);
}

QVector<QString> ShapefileManager::getFields() const
{
    // ���������ֶ���
    return fields;
}

void ShapefileManager::initializeFields()
{
    addField("GeoType");
    addField("Components");

    // ����״����ֶ�
    addField("NodeLine");
    addField("PointShape");
    addField("PointColor");

    // ��������ֶ�
    addField("LineStyle");
    addField("LineWidth");
    addField("LineColor");
    addField("LineDash");

    // �����ɫ�ֶ�
    addField("FillColor");

    // ��������ֶ�
    addField("SplineOrd");
    addField("SplineCnt");
    addField("Steps");

    // ����������ֶ�
    addField("BufferVis");
    addField("BufferMode");
    addField("BufferDist");

    // �������������ֶ�
    addField("BufferBord");
    addField("BufferLine");
    addField("BufferWid");
    addField("BufferCol");
    addField("BufferDash");

    // �������������ֶ�
    addField("BufferFill");
}


using Line = QVector<QPointF>;                      // ��ʾһ����
using Polygon = QVector<Line>;                      // ��ʾһ����
using Polygons = QVector<Polygon>;                  // ��ʾ�漯��

void ShapefileManager::clearGeometry()
{
    geometries.clear();
    attributes.clear();
}

void ShapefileManager::addGeometry(const QString& geometry, const QMap<QString, QVariant>& attr)
{
    // ��Ӽ������ݺ���������
    geometries.append(geometry);
    attributes.append(attr);
}

// �� Line ת��Ϊ WKT
QString ShapefileManager::lineToWKT(const Line& line) const
{
    QStringList points;
    for (const auto& point : line)
    {
        points.append(QString("%1 %2").arg(point.x()).arg(point.y()));
    }

    return QString("LINESTRING (%1)").arg(points.join(", "));
}

Line ShapefileManager::wktToLine(const QString& wkt) const
{
    Line line;

    // ƥ�������е����겿��
    QRegularExpression re(R"(\(([^()]+)\))");
    QRegularExpressionMatch match = re.match(wkt.trimmed());

    if (match.hasMatch())
    {
        QString lineStr = match.captured(1).trimmed();
        // ʹ��������ʽ�Ƴ����ܵĶ���ո�
        QStringList pointStrs = lineStr.split(QRegularExpression(R"(\s*,\s*)")); // ����ǰ���пո�
        for (const auto& pointStr : pointStrs)
        {
            QStringList coords = pointStr.split(QRegularExpression(R"(\s+)")); // ����֮���ÿո�ָ�
            if (coords.size() == 2)
            {
                bool ok1, ok2;
                double x = coords[0].toDouble(&ok1);
                double y = coords[1].toDouble(&ok2);
                if (ok1 && ok2)
                {
                    line.append(QPointF(x, y));
                }
                else
                {
                    Log << L("����ת��ʧ�ܣ�") << pointStr;
                }
            }
            else
            {
                Log << L("�����ʽ����") << pointStr;
            }
        }
    }
    else
    {
        Log << L("�޷�ƥ�� WKT ���ݣ�") << wkt;
    }

    return line;
}


void ShapefileManager::GeoParametersToAttributes(const GeoParameters& params, QMap<QString, QVariant>& attributes)
{
    // ����״����ֶ�
    attributes["NodeLine"] = static_cast<int>(params.nodeLineStyle);
    attributes["PointShape"] = static_cast<int>(params.pointShape);
    attributes["PointColor"] = params.pointColor.name(QColor::HexArgb); // ����͸����

    // ��������ֶ�
    attributes["LineStyle"] = static_cast<int>(params.lineStyle);
    attributes["LineWidth"] = params.lineWidth;
    attributes["LineColor"] = params.lineColor.name(QColor::HexArgb);  // ����͸����
    attributes["LineDash"] = params.lineDashPattern;

    // �����ɫ�ֶ�
    attributes["FillColor"] = params.fillColor.name(QColor::HexArgb);  // ����͸����

    // ��������ֶ�
    attributes["SplineOrd"] = params.splineOrder;
    attributes["SplineCnt"] = params.splineNodeCount;
    attributes["Steps"] = params.steps;

    // ����������ֶ�
    attributes["BufferVis"] = params.bufferVisible;
    attributes["BufferMode"] = static_cast<int>(params.bufferCalculationMode);
    attributes["BufferDist"] = params.bufferDistance;

    // �������������ֶ�
    attributes["BufferBord"] = params.bufferHasBorder;
    attributes["BufferLine"] = static_cast<int>(params.bufferLineStyle);
    attributes["BufferWid"] = params.bufferLineWidth;
    attributes["BufferCol"] = params.bufferLineColor.name(QColor::HexArgb);  // ����͸����
    attributes["BufferDash"] = params.bufferLineDashPattern;

    // �������������ֶ�
    attributes["BufferFill"] = params.bufferFillColor.name(QColor::HexArgb);  // ����͸����
}


void ShapefileManager::AttributesToGeoParameters(const QMap<QString, QVariant>& attributes, GeoParameters& params)
{
    // ����״����ֶ�
    params.nodeLineStyle = static_cast<NodeLineStyle>(attributes["NodeLine"].toInt());
    params.pointShape = static_cast<PointShape>(attributes["PointShape"].toInt());
    params.pointColor = QColor(attributes["PointColor"].toString());

    // ��������ֶ�
    params.lineStyle = static_cast<LineStyle>(attributes["LineStyle"].toInt());
    params.lineWidth = attributes["LineWidth"].toFloat();
    params.lineColor = QColor(attributes["LineColor"].toString());
    params.lineDashPattern = attributes["LineDash"].toFloat();

    // �����ɫ�ֶ�
    params.fillColor = QColor(attributes["FillColor"].toString());

    // ��������ֶ�
    params.splineOrder = attributes["SplineOrd"].toInt();
    params.splineNodeCount = attributes["SplineCnt"].toInt();
    params.steps = attributes["Steps"].toInt();

    // ����������ֶ�
    params.bufferVisible = attributes["BufferVis"].toBool();
    params.bufferCalculationMode = static_cast<BufferCalculationMode>(attributes["BufferMode"].toInt());
    params.bufferDistance = attributes["BufferDist"].toDouble();

    // �������������ֶ�
    params.bufferHasBorder = attributes["BufferBord"].toBool();
    params.bufferLineStyle = static_cast<LineStyle>(attributes["BufferLine"].toInt());
    params.bufferLineWidth = attributes["BufferWid"].toFloat();
    params.bufferLineColor = QColor(attributes["BufferCol"].toString());
    params.bufferLineDashPattern = attributes["BufferDash"].toFloat();

    // �������������ֶ�
    params.bufferFillColor = QColor(attributes["BufferFill"].toString());
}



void ShapefileManager::saveGeo(Geo* geo)
{
    GeoType geoType = geo->getGeoType();
    QMap<QString, QVariant> attributes;

    attributes["GeoType"] = static_cast<int>(geoType);
    GeoParametersToAttributes(geo->getGeoParameters(), attributes);

    QString str;
    str = lineToWKT(geo->controlPoints);
    switch (geoType)
    {
    case TypePoint:
    case TypeSimpleLine:
        addGeometry(str, attributes);
        break;
    case TypeDoubleLine:
        attributes["Components"] = QVariant::fromValue(static_cast<DoubleLine*>(geo)->component);
        break;
    case TypeParallelLine:
        attributes["Components"] = QVariant::fromValue(static_cast<ParallelLine*>(geo)->component);
        addGeometry(str, attributes);
        break;
    case TypeTwoPointCircle:
        addGeometry(str, attributes);
        break;
    case TypeSimpleArea:
        addGeometry(str, attributes);
        break;
    case TypeComplexArea:
        attributes["Components"] = Component::serializeComponentVector(static_cast<ComplexArea*>(geo)->component);
        addGeometry(str, attributes);
        break;
    default:
        break;
    }
}

Geo* ShapefileManager::loadGeo(const QMap<QString, QVariant>& attributes, const QString& geometryWKT)
{
    // ��ȡ GeoType
    GeoType geoType = static_cast<GeoType>(attributes["GeoType"].toInt());

    // �� WKT �������Ƶ�
    Line controlPoints = wktToLine(geometryWKT);

    Geo* geo = nullptr;

    switch (geoType)
    {
    case TypePoint:
    {
        Point* point = new Point();
        point->setGeoType(geoType);
        point->controlPoints = std::move(controlPoints);
        geo = point;
    }
    break;

    case TypeSimpleLine:
    {
        SimpleLine* simpleLine = new SimpleLine();
        simpleLine->setGeoType(geoType);
        simpleLine->controlPoints = std::move(controlPoints);
        geo = simpleLine;
    }
    break;

    case TypeDoubleLine:
    {
        DoubleLine* doubleLine = new DoubleLine();
        doubleLine->setGeoType(geoType);
        doubleLine->controlPoints = std::move(controlPoints);

        if (attributes.contains("Components"))
        {
            doubleLine->component = Component::deserializeComponentVector(attributes["Components"].toString());
        }
        geo = doubleLine;
    }
    break;

    case TypeParallelLine:
    {
        ParallelLine* parallelLine = new ParallelLine();
        parallelLine->setGeoType(geoType);
        parallelLine->controlPoints = std::move(controlPoints);

        if (attributes.contains("Components"))
        {
            parallelLine->component = Component::deserializeComponentVector(attributes["Components"].toString());
        }
        geo = parallelLine;
    }
    break;

    case TypeTwoPointCircle:
    {
        TwoPointCircle* circle = new TwoPointCircle();
        circle->setGeoType(geoType);
        circle->controlPoints = std::move(controlPoints);
        geo = circle;
    }
    break;

    case TypeSimpleArea:
    {
        SimpleArea* simpleArea = new SimpleArea();
        simpleArea->setGeoType(geoType);
        simpleArea->controlPoints = std::move(controlPoints);
        geo = simpleArea;
    }
    break;

    case TypeComplexArea:
    {
        ComplexArea* complexArea = new ComplexArea();
        complexArea->setGeoType(geoType);
        complexArea->controlPoints = std::move(controlPoints);

        if (attributes.contains("Components"))
        {
            complexArea->component = Component::deserializeComponentVector(attributes["Components"].toString());
        }
        geo = complexArea;
    }
    break;

    default:
        break;
    }

    // �����Լ�����������
    if (geo)
    {
        geo->initialize();
        GeoParameters geoParams;
        AttributesToGeoParameters(attributes, geoParams);
        geo->setGeoParameters(geoParams);
        geo->markControlPointsChanged();
        geo->completeDrawing();

    }
    return geo;
}

*/