#include "ShapefileManager.h"
#include "ogrsf_frmts.h"
#include <QtCore/qregularexpression.h>
ShapefileManager::ShapefileManager() : dataset(nullptr)
{
    // 初始化 GDAL 库（仅需一次）
    GDALAllRegister();
    initializeFields();

    qRegisterMetaType<Component>("Component");
}

bool ShapefileManager::openFile(const QString& filePath)
{
    if (filePath.isEmpty())
    {
        // 文件路径为空，显示提示信息
        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("文件路径为空，无法打开文件"));
        return false;
    }

    // 打开 Shapefile 文件
    dataset = static_cast<GDALDataset*>(GDALOpenEx(filePath.toUtf8().data(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr));
    if (dataset == nullptr)
    {
        // 无法打开文件，显示提示信息
        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("无法打开 Shapefile 文件：") + filePath);
        return false;
    }

    // 成功打开文件，显示提示信息
    if (GlobalStatusBar) GlobalStatusBar->showMessage(L("成功打开 Shapefile 文件：") + filePath);

    // 获取第一个图层
    OGRLayer* layer = dataset->GetLayer(0);
    if (layer == nullptr)
    {
        // 无法获取图层，显示提示信息
        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("无法获取图层"));
        GDALClose(dataset);
        dataset = nullptr;
        return false;
    }

    // 清空现有的几何和属性数据
    geometries.clear();
    attributes.clear();

    // 遍历图层中的要素（几何和属性数据）
    OGRFeature* feature = nullptr;
    layer->ResetReading(); // 重置图层读取指针
    while ((feature = layer->GetNextFeature()) != nullptr)
    {
        // 获取几何数据
        OGRGeometry* geometry = feature->GetGeometryRef();
        if (geometry != nullptr)
        {
            char* wkt = nullptr;
            geometry->exportToWkt(&wkt); // 导出为 WKT 格式
            if (wkt != nullptr)
            {
                geometries.append(QString::fromUtf8(wkt));
                CPLFree(wkt); // 释放 GDAL 分配的内存
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

        // 销毁要素对象
        OGRFeature::DestroyFeature(feature);
    }

    return true;
}


bool ShapefileManager::saveFile(const QString& filePath)
{
    if (filePath.isEmpty())
    {
        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("文件路径为空，无法保存文件"));
        return false;
    }

    // 创建 Shapefile 文件
    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
    if (driver == nullptr)
    {
        // 显示无法获取 Shapefile 驱动的提示信息
        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("无法获取 Shapefile 驱动"));
        return false;
    }

    dataset = driver->Create(filePath.toUtf8().data(), 0, 0, 0, GDT_Unknown, nullptr);
    if (dataset == nullptr)
    {
        // 显示无法创建 Shapefile 文件的提示信息
        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("无法创建 Shapefile 文件：") + filePath);
        return false;
    }

    // 创建图层
    OGRLayer* layer = dataset->CreateLayer("Layer1", nullptr, wkbPoint, nullptr);
    if (layer == nullptr)
    {
        // 显示无法创建图层的提示信息
        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("无法创建图层"));
        GDALClose(dataset);
        return false;
    }

    // 创建字段
    for (const auto& field : fields)
    {
        OGRFieldDefn fieldDefn(field.toUtf8().data(), OFTString);
        layer->CreateField(&fieldDefn);
    }

    // 添加几何和属性数据
    for (int i = 0; i < geometries.size(); ++i)
    {
        OGRFeature* feature = OGRFeature::CreateFeature(layer->GetLayerDefn());

        // 添加几何数据
        OGRGeometry* geometry = nullptr;
        const char* wkt = geometries[i].toUtf8().data();
        OGRErr err = OGRGeometryFactory::createFromWkt(wkt, nullptr, &geometry);

        if (err != OGRERR_NONE || geometry == nullptr)
        {
            // 如果创建几何失败，显示错误信息
            if (GlobalStatusBar) GlobalStatusBar->showMessage(L("无法解析 WKT 几何数据：") + geometries[i]);
            OGRFeature::DestroyFeature(feature);
            continue;  // 跳过此条记录
        }

        feature->SetGeometry(geometry);

       // QVector<QMap<QString, QVariant>>
        // 添加属性数据
        for (auto it = attributes[i].begin(); it != attributes[i].end(); ++it)
        {
            const QString& fieldName = it.key();
            const QVariant& fieldValue = it.value();
            feature->SetField(fieldName.toUtf8().data(), fieldValue.toString().toUtf8().data());
        }

        // 将要素添加到图层
        layer->CreateFeature(feature);
        OGRFeature::DestroyFeature(feature);
    }

    // 关闭文件
    GDALClose(dataset);

    // 显示保存文件成功的提示信息
    if (GlobalStatusBar) GlobalStatusBar->showMessage(L("文件保存成功：") + filePath);

    return true;
}

QVector<QString> ShapefileManager::getAllGeometries() const
{
    // 返回所有几何数据
    return geometries;
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
    addField("NodeLineStyle");
    addField("PointShape");
    addField("PointColor");

    // 线形相关字段
    addField("LineStyle");
    addField("LineWidth");
    addField("LineColor");
    addField("LineDashPattern");

    // 填充颜色字段
    addField("FillColor");

    // 样条相关字段
    addField("SplineOrder");
    addField("SplineNodeCount");
    addField("Steps");

    // 缓冲区相关字段
    addField("BufferVisible");
    addField("BufferMode");
    addField("BufferDistance");

    // 缓冲区线属性字段
    addField("BufferHasBorder");
    addField("BufferLineStyle");
    addField("BufferLineWidth");
    addField("BufferLineColor");
    addField("BufferLineDash");

    // 缓冲区面属性字段
    addField("BufferFillColor");
}


using Line = QVector<QPointF>;                   // 表示一条线
using Polygon = QVector<Line>;                 // 表示一个面
using Polygons = QVector<Polygon>;             // 表示面集合

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

// 从 WKT 转换为 Line
Line ShapefileManager::wktToLine(const QString& wkt) const
{
    Line line;
    QRegularExpression re(R"(\(([^()]+)\))"); // 匹配括号中的坐标
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



// 将 GeoParameters 转换为 QMap<QString, QVariant>
void ShapefileManager::GeoParametersToAttributes(const GeoParameters& params, QMap<QString, QVariant>& attributes)
{
    attributes["nodeLineStyle"] = static_cast<int>(params.nodeLineStyle);
    attributes["pointShape"] = static_cast<int>(params.pointShape);
    attributes["pointColor"] = params.pointColor.name();                  // QColor 转为字符串
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

// 从 QMap<QString, QVariant> 转换为 GeoParameters
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

    // 从属性加载其他参数
    if (geo)
    {
        GeoParameters geoParams;
        AttributesToGeoParameters(attributes, geoParams);
        geo->setGeoParameters(geoParams);
    }

    return geo;
}
