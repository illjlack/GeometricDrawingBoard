#include "ShapefileManager.h"
#include "ogrsf_frmts.h"
#include <QtCore/qregularexpression.h>
ShapefileManager::ShapefileManager() : dataset(nullptr)
{
    // ��ʼ�� GDAL �⣨����һ�Σ�
    GDALAllRegister();
    initializeFields();

    qRegisterMetaType<Component>("Component");
}

bool ShapefileManager::openFile(const QString& filePath)
{
    if (filePath.isEmpty())
    {
        // �ļ�·��Ϊ�գ���ʾ��ʾ��Ϣ
        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�ļ�·��Ϊ�գ��޷����ļ�"));
        return false;
    }

    // �� Shapefile �ļ�
    dataset = static_cast<GDALDataset*>(GDALOpenEx(filePath.toUtf8().data(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr));
    if (dataset == nullptr)
    {
        // �޷����ļ�����ʾ��ʾ��Ϣ
        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�޷��� Shapefile �ļ���") + filePath);
        return false;
    }

    // �ɹ����ļ�����ʾ��ʾ��Ϣ
    if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�ɹ��� Shapefile �ļ���") + filePath);

    // ��ȡ��һ��ͼ��
    OGRLayer* layer = dataset->GetLayer(0);
    if (layer == nullptr)
    {
        // �޷���ȡͼ�㣬��ʾ��ʾ��Ϣ
        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�޷���ȡͼ��"));
        GDALClose(dataset);
        dataset = nullptr;
        return false;
    }

    // ������еļ��κ���������
    geometries.clear();
    attributes.clear();

    // ����ͼ���е�Ҫ�أ����κ��������ݣ�
    OGRFeature* feature = nullptr;
    layer->ResetReading(); // ����ͼ���ȡָ��
    while ((feature = layer->GetNextFeature()) != nullptr)
    {
        // ��ȡ��������
        OGRGeometry* geometry = feature->GetGeometryRef();
        if (geometry != nullptr)
        {
            char* wkt = nullptr;
            geometry->exportToWkt(&wkt); // ����Ϊ WKT ��ʽ
            if (wkt != nullptr)
            {
                geometries.append(QString::fromUtf8(wkt));
                CPLFree(wkt); // �ͷ� GDAL ������ڴ�
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

        // ����Ҫ�ض���
        OGRFeature::DestroyFeature(feature);
    }

    return true;
}


bool ShapefileManager::saveFile(const QString& filePath)
{
    if (filePath.isEmpty())
    {
        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�ļ�·��Ϊ�գ��޷������ļ�"));
        return false;
    }

    // ���� Shapefile �ļ�
    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
    if (driver == nullptr)
    {
        // ��ʾ�޷���ȡ Shapefile ��������ʾ��Ϣ
        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�޷���ȡ Shapefile ����"));
        return false;
    }

    dataset = driver->Create(filePath.toUtf8().data(), 0, 0, 0, GDT_Unknown, nullptr);
    if (dataset == nullptr)
    {
        // ��ʾ�޷����� Shapefile �ļ�����ʾ��Ϣ
        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�޷����� Shapefile �ļ���") + filePath);
        return false;
    }

    // ����ͼ��
    OGRLayer* layer = dataset->CreateLayer("Layer1", nullptr, wkbPoint, nullptr);
    if (layer == nullptr)
    {
        // ��ʾ�޷�����ͼ�����ʾ��Ϣ
        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�޷�����ͼ��"));
        GDALClose(dataset);
        return false;
    }

    // �����ֶ�
    for (const auto& field : fields)
    {
        OGRFieldDefn fieldDefn(field.toUtf8().data(), OFTString);
        layer->CreateField(&fieldDefn);
    }

    // ��Ӽ��κ���������
    for (int i = 0; i < geometries.size(); ++i)
    {
        OGRFeature* feature = OGRFeature::CreateFeature(layer->GetLayerDefn());

        // ��Ӽ�������
        OGRGeometry* geometry = nullptr;
        const char* wkt = geometries[i].toUtf8().data();
        OGRErr err = OGRGeometryFactory::createFromWkt(wkt, nullptr, &geometry);

        if (err != OGRERR_NONE || geometry == nullptr)
        {
            // �����������ʧ�ܣ���ʾ������Ϣ
            if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�޷����� WKT �������ݣ�") + geometries[i]);
            OGRFeature::DestroyFeature(feature);
            continue;  // ����������¼
        }

        feature->SetGeometry(geometry);

       // QVector<QMap<QString, QVariant>>
        // �����������
        for (auto it = attributes[i].begin(); it != attributes[i].end(); ++it)
        {
            const QString& fieldName = it.key();
            const QVariant& fieldValue = it.value();
            feature->SetField(fieldName.toUtf8().data(), fieldValue.toString().toUtf8().data());
        }

        // ��Ҫ����ӵ�ͼ��
        layer->CreateFeature(feature);
        OGRFeature::DestroyFeature(feature);
    }

    // �ر��ļ�
    GDALClose(dataset);

    // ��ʾ�����ļ��ɹ�����ʾ��Ϣ
    if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�ļ�����ɹ���") + filePath);

    return true;
}

QVector<QString> ShapefileManager::getAllGeometries() const
{
    // �������м�������
    return geometries;
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
    addField("NodeLineStyle");
    addField("PointShape");
    addField("PointColor");

    // ��������ֶ�
    addField("LineStyle");
    addField("LineWidth");
    addField("LineColor");
    addField("LineDashPattern");

    // �����ɫ�ֶ�
    addField("FillColor");

    // ��������ֶ�
    addField("SplineOrder");
    addField("SplineNodeCount");
    addField("Steps");

    // ����������ֶ�
    addField("BufferVisible");
    addField("BufferMode");
    addField("BufferDistance");

    // �������������ֶ�
    addField("BufferHasBorder");
    addField("BufferLineStyle");
    addField("BufferLineWidth");
    addField("BufferLineColor");
    addField("BufferLineDash");

    // �������������ֶ�
    addField("BufferFillColor");
}


using Line = QVector<QPointF>;                   // ��ʾһ����
using Polygon = QVector<Line>;                 // ��ʾһ����
using Polygons = QVector<Polygon>;             // ��ʾ�漯��

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

// �� WKT ת��Ϊ Line
Line ShapefileManager::wktToLine(const QString& wkt) const
{
    Line line;
    QRegularExpression re(R"(\(([^()]+)\))"); // ƥ�������е�����
    QRegularExpressionMatch match = re.match(wkt);

    if (match.hasMatch())
    {
        QString lineStr = match.captured(1);
        QStringList pointStrs = lineStr.split(", ");
        for (const auto& pointStr : pointStrs)
        {
            QStringList coords = pointStr.split(" ");
            if (coords.size() == 2)
            {
                line.append(QPointF(coords[0].toDouble(), coords[1].toDouble()));
            }
        }
    }

    return line;
}



// �� GeoParameters ת��Ϊ QMap<QString, QVariant>
void ShapefileManager::GeoParametersToAttributes(const GeoParameters& params, QMap<QString, QVariant>& attributes)
{
    attributes["nodeLineStyle"] = static_cast<int>(params.nodeLineStyle);
    attributes["pointShape"] = static_cast<int>(params.pointShape);
    attributes["pointColor"] = params.pointColor.name();                  // QColor תΪ�ַ���
    attributes["lineStyle"] = static_cast<int>(params.lineStyle);
    attributes["lineWidth"] = params.lineWidth;
    attributes["lineColor"] = params.lineColor.name();
    attributes["lineDashPattern"] = params.lineDashPattern;
    attributes["fillColor"] = params.fillColor.name();
    attributes["splineOrder"] = params.splineOrder;
    attributes["splineNodeCount"] = params.splineNodeCount;
    attributes["steps"] = params.steps;
    attributes["bufferVisible"] = params.bufferVisible;
    attributes["bufferCalculationMode"] = static_cast<int>(params.bufferCalculationMode);
    attributes["bufferDistance"] = params.bufferDistance;
    attributes["bufferHasBorder"] = params.bufferHasBorder;
    attributes["bufferLineStyle"] = static_cast<int>(params.bufferLineStyle);
    attributes["bufferLineWidth"] = params.bufferLineWidth;
    attributes["bufferLineColor"] = params.bufferLineColor.name();
    attributes["bufferLineDashPattern"] = params.bufferLineDashPattern;
    attributes["bufferFillColor"] = params.bufferFillColor.name();
}

// �� QMap<QString, QVariant> ת��Ϊ GeoParameters
void ShapefileManager::AttributesToGeoParameters(const QMap<QString, QVariant>& attributes, GeoParameters&params)
{
    GeoParameters params;
    params.nodeLineStyle = static_cast<NodeLineStyle>(attributes["nodeLineStyle"].toInt());
    params.pointShape = static_cast<PointShape>(attributes["pointShape"].toInt());
    params.pointColor = QColor(attributes["pointColor"].toString());
    params.lineStyle = static_cast<LineStyle>(attributes["lineStyle"].toInt());
    params.lineWidth = attributes["lineWidth"].toFloat();
    params.lineColor = QColor(attributes["lineColor"].toString());
    params.lineDashPattern = attributes["lineDashPattern"].toFloat();
    params.fillColor = QColor(attributes["fillColor"].toString());
    params.splineOrder = attributes["splineOrder"].toInt();
    params.splineNodeCount = attributes["splineNodeCount"].toInt();
    params.steps = attributes["steps"].toInt();
    params.bufferVisible = attributes["bufferVisible"].toBool();
    params.bufferCalculationMode = static_cast<BufferCalculationMode>(attributes["bufferCalculationMode"].toInt());
    params.bufferDistance = attributes["bufferDistance"].toDouble();
    params.bufferHasBorder = attributes["bufferHasBorder"].toBool();
    params.bufferLineStyle = static_cast<LineStyle>(attributes["bufferLineStyle"].toInt());
    params.bufferLineWidth = attributes["bufferLineWidth"].toFloat();
    params.bufferLineColor = QColor(attributes["bufferLineColor"].toString());
    params.bufferLineDashPattern = attributes["bufferLineDashPattern"].toFloat();
    params.bufferFillColor = QColor(attributes["bufferFillColor"].toString());
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
        attributes["Components"] = QVariant::fromValue(static_cast<ComplexArea*>(geo)->component);
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
            doubleLine->component = attributes["Components"].value<QVector<Component>>();
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
            parallelLine->component = attributes["Components"].value<QVector<Component>>();
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
            complexArea->component = attributes["Components"].value<QVector<Component>>();
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
        GeoParameters geoParams;
        AttributesToGeoParameters(attributes, geoParams);
        geo->setGeoParameters(geoParams);
    }

    return geo;
}
