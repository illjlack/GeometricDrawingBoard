//#include "ShapefileManager.h"
//#include "comm.h"
//
//ShapefileManager::ShapefileManager() : dataset(nullptr)
//{
//    // ��ʼ�� GDAL �⣨����һ�Σ�
//    GDALAllRegister();
//}
//
//bool ShapefileManager::openFile(const QString& filePath)
//{
//    if (filePath.isEmpty())
//    {
//        // ��ʾ�ļ�·��Ϊ�յ���ʾ��Ϣ
//        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�ļ�·��Ϊ�գ��޷����ļ�"));
//        return false;
//    }
//
//    // �� Shapefile �ļ�
//    dataset = (GDALDataset*)GDALOpenEx(filePath.toUtf8().data(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr);
//    if (dataset == nullptr)
//    {
//        // ��ʾ�޷����ļ�����ʾ��Ϣ
//        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�޷��� Shapefile �ļ���") + filePath);
//        return false;
//    }
//
//    // �ɹ����ļ�����ʾ��ʾ��Ϣ
//    if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�ɹ��� Shapefile �ļ���") + filePath);
//
//    // ��ȡͼ�㣨�����һ��ͼ�㣩
//    OGRLayer* layer = dataset->GetLayer(0);
//    if (layer == nullptr)
//    {
//        // ��ʾ�޷���ȡͼ�����ʾ��Ϣ
//        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�޷���ȡͼ��"));
//        GDALClose(dataset);
//        return false;
//    }
//
//    // ������еļ��κ���������
//    geometries.clear();
//    attributes.clear();
//
//    // ����ͼ���е�Ҫ�أ��������ݣ�
//    OGRFeature* feature;
//    layer->ResetReading();
//    while ((feature = layer->GetNextFeature()) != nullptr)
//    {
//        // ��ȡ��������
//        OGRGeometry* geometry = feature->GetGeometryRef();
//        if (geometry != nullptr)
//        {
//            //QString geometryStr = QString::fromUtf8(geometry->exportToWkt());
//            //geometries.append(geometryStr);
//        }
//
//        // ��ȡ��������
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
//        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�ļ�·��Ϊ�գ��޷������ļ�"));
//        return false;
//    }
//
//    // ���� Shapefile �ļ�
//    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
//    if (driver == nullptr)
//    {
//        // ��ʾ�޷���ȡ Shapefile ��������ʾ��Ϣ
//        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�޷���ȡ Shapefile ����"));
//        return false;
//    }
//
//    dataset = driver->Create(filePath.toUtf8().data(), 0, 0, 0, GDT_Unknown, nullptr);
//    if (dataset == nullptr)
//    {
//        // ��ʾ�޷����� Shapefile �ļ�����ʾ��Ϣ
//        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�޷����� Shapefile �ļ���") + filePath);
//        return false;
//    }
//
//    // ����ͼ��
//    OGRLayer* layer = dataset->CreateLayer("Layer1", nullptr, wkbPoint, nullptr);
//    if (layer == nullptr)
//    {
//        // ��ʾ�޷�����ͼ�����ʾ��Ϣ
//        if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�޷�����ͼ��"));
//        GDALClose(dataset);
//        return false;
//    }
//
//    // �����ֶ�
//    for (const auto& field : fields)
//    {
//        OGRFieldDefn fieldDefn(field.toUtf8().data(), OFTString);
//        layer->CreateField(&fieldDefn);
//    }
//
//    // ��Ӽ��κ���������
//    for (int i = 0; i < geometries.size(); ++i)
//    {
//        OGRFeature* feature = OGRFeature::CreateFeature(layer->GetLayerDefn());
//
//        // ��Ӽ�������
//        OGRGeometry* geometry;
//        OGRGeometryFactory::createGeometryFromWkt(geometries[i].toUtf8().data(), &geometry);
//        feature->SetGeometry(geometry);
//
//        // �����������
//        for (const auto& [fieldName, fieldValue] : attributes[i])
//        {
//            feature->SetField(fieldName.toUtf8().data(), fieldValue.toUtf8().data());
//        }
//
//        // ��Ҫ����ӵ�ͼ��
//        layer->CreateFeature(feature);
//        OGRFeature::DestroyFeature(feature);
//    }
//
//    // �ر��ļ�
//    GDALClose(dataset);
//
//    // ��ʾ�����ļ��ɹ�����ʾ��Ϣ
//    if (GlobalStatusBar) GlobalStatusBar->showMessage(L("�ļ�����ɹ���") + filePath);
//
//    return true;
//}
//
//void ShapefileManager::addGeometry(const QString& geometry, const std::unordered_map<QString, QString>& attr)
//{
//    // ��Ӽ������ݺ���������
//    geometries.append(geometry);
//    attributes.append(attr);
//}
//
//QStringList ShapefileManager::getAllGeometries() const
//{
//    // �������м�������
//    return geometries;
//}
//
//void ShapefileManager::addField(const QString& fieldName)
//{
//    // ����ֶ���
//    fields.append(fieldName);
//}
//
//QStringList ShapefileManager::getFields() const
//{
//    // ���������ֶ���
//    return fields;
//}
