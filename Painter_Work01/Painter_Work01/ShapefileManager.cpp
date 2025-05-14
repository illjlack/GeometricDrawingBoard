/*
#include "ShapefileManager.h"
#include "ogrsf_frmts.h"
#include <QtCore/qregularexpression.h>
#include <QDir>

ShapefileManager::ShapefileManager() : dataset(nullptr)
{
    // 初始化 GDAL 库（仅需一次）
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
        Log << L("文件路径为空，无法打开文件");
        return false;
    }

    QFileInfo fileInfo(shpFilePath);
    if (!fileInfo.exists() || fileInfo.suffix().toLower() != "shp")
    {
        Log << L("指定的文件不存在或不是一个 Shapefile 文件：") << shpFilePath;
        return false;
    }

    // 打开 Shapefile 文件
    dataset = static_cast<GDALDataset*>(GDALOpenEx(shpFilePath.toUtf8().data(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr));
    if (dataset == nullptr)
    {
        Log << L("无法打开 Shapefile 文件：") << shpFilePath;
        return false;
    }

    Log << L("成功打开 Shapefile 文件：") << shpFilePath;

    // 获取第一个有效的图层
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
        Log << L("无法获取图层");
        GDALClose(dataset);
        dataset = nullptr;
        return false;
    }

    // 清空之前的数据
    geometries.clear();
    attributes.clear();

    // 遍历图层中的要素
    OGRFeature* feature = nullptr;
    layer->ResetReading();

    if (layer->GetFeatureCount() == 0) {
        Log << L("图层中没有要素");
        GDALClose(dataset);  // 确保在这里关闭文件
        dataset = nullptr;
        return false;
    }

    while ((feature = layer->GetNextFeature()) != nullptr)
    {
        // 获取几何数据并转换为 WKT 格式
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
                Log << L("无法转换几何数据为 WKT 格式");
            }
        }

        // 获取属性数据
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

        // 销毁当前要素
        OGRFeature::DestroyFeature(feature);
    }

    GDALClose(dataset);  // 确保文件在函数结束时被关闭
    dataset = nullptr;

    return true;
}



bool ShapefileManager::saveFile(const QString& folderPath)
{
    if (folderPath.isEmpty())
    {
        Log << L("文件夹路径为空，无法保存文件");
        return false;
    }

    QDir folder(folderPath);
    if (!folder.exists() && !folder.mkpath("."))
    {
        Log << L("无法创建文件夹：") << folderPath;
        return false;
    }

    QString shpFilePath = folder.filePath("main.shp");

    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
    if (driver == nullptr)
    {
        Log << L("无法获取 Shapefile 驱动");
        return false;
    }

    // 删除已有文件（如果存在）
    if (QFile::exists(shpFilePath))
    {
        if (driver->Delete(shpFilePath.toUtf8().data()) != CE_None)
        {
            Log << L("无法删除已有 Shapefile 文件：") << shpFilePath;
            return false;
        }
    }

    dataset = driver->Create(shpFilePath.toUtf8().data(), 0, 0, 0, GDT_Unknown, nullptr);
    if (dataset == nullptr)
    {
        Log << L("无法创建 Shapefile 文件：") << shpFilePath;
        return false;
    }

    OGRLayer* layer = dataset->CreateLayer("Layer1", nullptr, wkbLineString, nullptr);
    if (layer == nullptr)
    {
        Log << L("无法创建图层");
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
        // 创建无投影的坐标系
        OGRSpatialReference spatialRef;
        spatialRef.SetLocalCS("Non-Geographic");

        // 解析 WKT 数据时指定坐标系
        OGRErr err = OGRGeometryFactory::createFromWkt(wkt, &spatialRef, &geometry);

        // 解析失败
        if (err != OGRERR_NONE || geometry == nullptr)
        {
            // 根据错误代码构建错误信息
            QString errMsg = QString("OGRErr code: %1").arg(err);

            // 记录日志，增加更多详细信息
            Log << L("无法解析 WKT 几何数据：") << geometries[i]
                << L(" 错误码：") << errMsg;

            // 销毁 feature 并跳过当前循环
            OGRFeature::DestroyFeature(feature);
            continue;
        }

        // 设置几何数据到 feature
        feature->SetGeometry(geometry);

        // 设置字段
        for (auto it = attributes[i].begin(); it != attributes[i].end(); ++it)
        {
            feature->SetField(it.key().toUtf8().data(), it.value().toString().toUtf8().data());
        }

        // 创建 feature 到图层
        OGRErr featureErr = layer->CreateFeature(feature);
        if (featureErr != OGRERR_NONE)
        {
            Log << L("无法创建要素，错误码：") << featureErr;
            OGRFeature::DestroyFeature(feature);
            continue;
        }
        OGRFeature::DestroyFeature(feature);
    }

    GDALClose(dataset);

    Log << L("文件保存成功：") << shpFilePath;

    return true;
}


void ShapefileManager::addField(const QString& fieldName)
{
    // 添加字段名
    fields.append(fieldName);
}

QVector<QString> ShapefileManager::getFields() const
{
    // 返回所有字段名
    return fields;
}

void ShapefileManager::initializeFields()
{
    addField("GeoType");
    addField("Components");

    // 点形状相关字段
    addField("NodeLine");
    addField("PointShape");
    addField("PointColor");

    // 线形相关字段
    addField("LineStyle");
    addField("LineWidth");
    addField("LineColor");
    addField("LineDash");

    // 填充颜色字段
    addField("FillColor");

    // 样条相关字段
    addField("SplineOrd");
    addField("SplineCnt");
    addField("Steps");

    // 缓冲区相关字段
    addField("BufferVis");
    addField("BufferMode");
    addField("BufferDist");

    // 缓冲区线属性字段
    addField("BufferBord");
    addField("BufferLine");
    addField("BufferWid");
    addField("BufferCol");
    addField("BufferDash");

    // 缓冲区面属性字段
    addField("BufferFill");
}


using Line = QVector<QPointF>;                      // 表示一条线
using Polygon = QVector<Line>;                      // 表示一个面
using Polygons = QVector<Polygon>;                  // 表示面集合

void ShapefileManager::clearGeometry()
{
    geometries.clear();
    attributes.clear();
}

void ShapefileManager::addGeometry(const QString& geometry, const QMap<QString, QVariant>& attr)
{
    // 添加几何数据和属性数据
    geometries.append(geometry);
    attributes.append(attr);
}

// 将 Line 转换为 WKT
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

    // 匹配括号中的坐标部分
    QRegularExpression re(R"(\(([^()]+)\))");
    QRegularExpressionMatch match = re.match(wkt.trimmed());

    if (match.hasMatch())
    {
        QString lineStr = match.captured(1).trimmed();
        // 使用正则表达式移除可能的额外空格
        QStringList pointStrs = lineStr.split(QRegularExpression(R"(\s*,\s*)")); // 允许前后有空格
        for (const auto& pointStr : pointStrs)
        {
            QStringList coords = pointStr.split(QRegularExpression(R"(\s+)")); // 坐标之间用空格分隔
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
                    Log << L("坐标转换失败：") << pointStr;
                }
            }
            else
            {
                Log << L("坐标格式错误：") << pointStr;
            }
        }
    }
    else
    {
        Log << L("无法匹配 WKT 数据：") << wkt;
    }

    return line;
}


void ShapefileManager::GeoParametersToAttributes(const GeoParameters& params, QMap<QString, QVariant>& attributes)
{
    // 点形状相关字段
    attributes["NodeLine"] = static_cast<int>(params.nodeLineStyle);
    attributes["PointShape"] = static_cast<int>(params.pointShape);
    attributes["PointColor"] = params.pointColor.name(QColor::HexArgb); // 包括透明度

    // 线形相关字段
    attributes["LineStyle"] = static_cast<int>(params.lineStyle);
    attributes["LineWidth"] = params.lineWidth;
    attributes["LineColor"] = params.lineColor.name(QColor::HexArgb);  // 包括透明度
    attributes["LineDash"] = params.lineDashPattern;

    // 填充颜色字段
    attributes["FillColor"] = params.fillColor.name(QColor::HexArgb);  // 包括透明度

    // 样条相关字段
    attributes["SplineOrd"] = params.splineOrder;
    attributes["SplineCnt"] = params.splineNodeCount;
    attributes["Steps"] = params.steps;

    // 缓冲区相关字段
    attributes["BufferVis"] = params.bufferVisible;
    attributes["BufferMode"] = static_cast<int>(params.bufferCalculationMode);
    attributes["BufferDist"] = params.bufferDistance;

    // 缓冲区线属性字段
    attributes["BufferBord"] = params.bufferHasBorder;
    attributes["BufferLine"] = static_cast<int>(params.bufferLineStyle);
    attributes["BufferWid"] = params.bufferLineWidth;
    attributes["BufferCol"] = params.bufferLineColor.name(QColor::HexArgb);  // 包括透明度
    attributes["BufferDash"] = params.bufferLineDashPattern;

    // 缓冲区面属性字段
    attributes["BufferFill"] = params.bufferFillColor.name(QColor::HexArgb);  // 包括透明度
}


void ShapefileManager::AttributesToGeoParameters(const QMap<QString, QVariant>& attributes, GeoParameters& params)
{
    // 点形状相关字段
    params.nodeLineStyle = static_cast<NodeLineStyle>(attributes["NodeLine"].toInt());
    params.pointShape = static_cast<PointShape>(attributes["PointShape"].toInt());
    params.pointColor = QColor(attributes["PointColor"].toString());

    // 线形相关字段
    params.lineStyle = static_cast<LineStyle>(attributes["LineStyle"].toInt());
    params.lineWidth = attributes["LineWidth"].toFloat();
    params.lineColor = QColor(attributes["LineColor"].toString());
    params.lineDashPattern = attributes["LineDash"].toFloat();

    // 填充颜色字段
    params.fillColor = QColor(attributes["FillColor"].toString());

    // 样条相关字段
    params.splineOrder = attributes["SplineOrd"].toInt();
    params.splineNodeCount = attributes["SplineCnt"].toInt();
    params.steps = attributes["Steps"].toInt();

    // 缓冲区相关字段
    params.bufferVisible = attributes["BufferVis"].toBool();
    params.bufferCalculationMode = static_cast<BufferCalculationMode>(attributes["BufferMode"].toInt());
    params.bufferDistance = attributes["BufferDist"].toDouble();

    // 缓冲区线属性字段
    params.bufferHasBorder = attributes["BufferBord"].toBool();
    params.bufferLineStyle = static_cast<LineStyle>(attributes["BufferLine"].toInt());
    params.bufferLineWidth = attributes["BufferWid"].toFloat();
    params.bufferLineColor = QColor(attributes["BufferCol"].toString());
    params.bufferLineDashPattern = attributes["BufferDash"].toFloat();

    // 缓冲区面属性字段
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
    // 获取 GeoType
    GeoType geoType = static_cast<GeoType>(attributes["GeoType"].toInt());

    // 从 WKT 解析控制点
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

    // 从属性加载其他参数
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