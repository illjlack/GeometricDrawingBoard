//#include "ShapefileManager.h"
//#include "comm.h"
//
//ShapefileManager::ShapefileManager() : dataset(nullptr)
//{
//    // 初始化 GDAL 库（仅需一次）
//    GDALAllRegister();
//}
//
//bool ShapefileManager::openFile(const QString& filePath)
//{
//    if (filePath.isEmpty())
//    {
//        // 显示文件路径为空的提示信息
//        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("文件路径为空，无法打开文件"));
//        return false;
//    }
//
//    // 打开 Shapefile 文件
//    dataset = (GDALDataset*)GDALOpenEx(filePath.toUtf8().data(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr);
//    if (dataset == nullptr)
//    {
//        // 显示无法打开文件的提示信息
//        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("无法打开 Shapefile 文件：") + filePath);
//        return false;
//    }
//
//    // 成功打开文件，显示提示信息
//    if (GlobalStatusBar) GlobalStatusBar->showMessage(L("成功打开 Shapefile 文件：") + filePath);
//
//    // 获取图层（假设第一个图层）
//    OGRLayer* layer = dataset->GetLayer(0);
//    if (layer == nullptr)
//    {
//        // 显示无法获取图层的提示信息
//        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("无法获取图层"));
//        GDALClose(dataset);
//        return false;
//    }
//
//    // 清空现有的几何和属性数据
//    geometries.clear();
//    attributes.clear();
//
//    // 遍历图层中的要素（几何数据）
//    OGRFeature* feature;
//    layer->ResetReading();
//    while ((feature = layer->GetNextFeature()) != nullptr)
//    {
//        // 获取几何数据
//        OGRGeometry* geometry = feature->GetGeometryRef();
//        if (geometry != nullptr)
//        {
//            //QString geometryStr = QString::fromUtf8(geometry->exportToWkt());
//            //geometries.append(geometryStr);
//        }
//
//        // 获取属性数据
//        std::unordered_map<QString, QString> attr;
//        for (int i = 0; i < feature->GetFieldCount(); ++i)
//        {
//            QString fieldName = QString::fromUtf8(feature->GetFieldDefnRef(i)->GetNameRef());
//            QString fieldValue = QString::fromUtf8(feature->GetFieldAsString(i));
//            attr[fieldName] = fieldValue;
//        }
//        attributes.append(attr);
//
//        OGRFeature::DestroyFeature(feature);
//    }
//
//    return true;
//}
//
//bool ShapefileManager::saveFile(const QString& filePath)
//{
//    if (filePath.isEmpty())
//    {
//        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("文件路径为空，无法保存文件"));
//        return false;
//    }
//
//    // 创建 Shapefile 文件
//    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
//    if (driver == nullptr)
//    {
//        // 显示无法获取 Shapefile 驱动的提示信息
//        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("无法获取 Shapefile 驱动"));
//        return false;
//    }
//
//    dataset = driver->Create(filePath.toUtf8().data(), 0, 0, 0, GDT_Unknown, nullptr);
//    if (dataset == nullptr)
//    {
//        // 显示无法创建 Shapefile 文件的提示信息
//        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("无法创建 Shapefile 文件：") + filePath);
//        return false;
//    }
//
//    // 创建图层
//    OGRLayer* layer = dataset->CreateLayer("Layer1", nullptr, wkbPoint, nullptr);
//    if (layer == nullptr)
//    {
//        // 显示无法创建图层的提示信息
//        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("无法创建图层"));
//        GDALClose(dataset);
//        return false;
//    }
//
//    // 创建字段
//    for (const auto& field : fields)
//    {
//        OGRFieldDefn fieldDefn(field.toUtf8().data(), OFTString);
//        layer->CreateField(&fieldDefn);
//    }
//
//    // 添加几何和属性数据
//    for (int i = 0; i < geometries.size(); ++i)
//    {
//        OGRFeature* feature = OGRFeature::CreateFeature(layer->GetLayerDefn());
//
//        // 添加几何数据
//        OGRGeometry* geometry;
//        OGRGeometryFactory::createGeometryFromWkt(geometries[i].toUtf8().data(), &geometry);
//        feature->SetGeometry(geometry);
//
//        // 添加属性数据
//        for (const auto& [fieldName, fieldValue] : attributes[i])
//        {
//            feature->SetField(fieldName.toUtf8().data(), fieldValue.toUtf8().data());
//        }
//
//        // 将要素添加到图层
//        layer->CreateFeature(feature);
//        OGRFeature::DestroyFeature(feature);
//    }
//
//    // 关闭文件
//    GDALClose(dataset);
//
//    // 显示保存文件成功的提示信息
//    if (GlobalStatusBar) GlobalStatusBar->showMessage(L("文件保存成功：") + filePath);
//
//    return true;
//}
//
//void ShapefileManager::addGeometry(const QString& geometry, const std::unordered_map<QString, QString>& attr)
//{
//    // 添加几何数据和属性数据
//    geometries.append(geometry);
//    attributes.append(attr);
//}
//
//QStringList ShapefileManager::getAllGeometries() const
//{
//    // 返回所有几何数据
//    return geometries;
//}
//
//void ShapefileManager::addField(const QString& fieldName)
//{
//    // 添加字段名
//    fields.append(fieldName);
//}
//
//QStringList ShapefileManager::getFields() const
//{
//    // 返回所有字段名
//    return fields;
//}
