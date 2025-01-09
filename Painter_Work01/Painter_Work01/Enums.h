#pragma once

// �����ö����̫���ˣ����Զ�ͳһ�����
// 
// Ϊ�˷������ö�ʹ�������������Ƶġ�


enum DrawSettingKey 
{
    BeginDrawSettingKey = 0,

    Key_DrawMode,   // ѡ����Ƶ����ͣ���,����...��

    Key_PointShape, // �����״
    Key_PointColor, // �����ɫ

    Key_LineStyle,          // �ߵ���ʽ(ʵ�ߡ�����)
    Key_LineWidth,          // �߿�
    Key_LineColor,          // �ߵ���ɫ
    Key_LineDashPattern,    // ���߶γ�
    Key_NodeLineStyle,      // �ڵ�����
    Key_FillColor,          // �������ɫ

    Key_SplineOrder,          // ��������
    Key_SplineNodeCount,      // �����ڵ����
    Key_Steps,                // �������ߵĵ���ܶ�

    EndDrawSettingKey
};


// ����ģʽ
enum DrawMode 
{
    BeginDrawMode = DrawSettingKey::EndDrawSettingKey,

    None,
    DrawPoint,
    DrawSimpleLine,     // ����
    DrawDoubleLine,     // ˫��
    DrawParallelLine,   // ƽ����
    DrawTwoPointCircle, // ���㻭Բ
    DrawSimpleArea,     // ����
    DrawComplexArea,     // ������

    EndDrawMode
};

enum NodeLineStyle
{
    BeginNodeLineStyle = EndDrawMode,

    NoStyle,                //δ����
    StylePolyline,          // ����
    StyleSpline,            // ������
    StyleThreePointArc,     // ����Բ��
    StyleArc,               // Բ����ʵ��Ҳ�����㣩
    StyleTwoPointCircle,    // ����
    StyleStreamline,        // ����
    
    EndNodeLineStyle,
};

enum GeoType
{
    BeginGeoType = EndDrawMode,

    Undefined,                  // δ����
    TypePoint,                  // ��
    TypeSimpleLine,             // ����
    TypeDoubleLine,             // ˫��
    TypeParallelLine,           // ƽ����
    TypeTwoPointCircle,         // ���㻭Բ
    TypeSimpleArea,             // ����
    TypeComplexArea,            // ������

    EndGeoType
};

enum GeoState
{
    BeginGeoState = EndGeoType,

    GeoStateInitialized = 1 << 0,
    GeoStateComplete = 1 << 1,       // ��ɻ��ƣ���������ʱ���Ƶ㣬������꣩
    GeoStateInvalid  = 1 << 2,       // ����
    GeoStateSelected = 1 << 3,       // ѡ�У���ʾ���Ƶ㣩

    EndGeoState = BeginGeoState + 1
};

enum LineStyle {
    BeginStyle = EndGeoState,

    Solid,      // ʵ��
    Dashed,     // ����

    EndStyle
};

enum PointShape {
    BeginPointShape = EndStyle,

    Square,     // ����
    Circle,     // Բ��

    EndPointShape
};
