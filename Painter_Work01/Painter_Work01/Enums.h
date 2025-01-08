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

    Key_LineStyle,          // �ߵ���ʽ
    Key_LineWidth,          // �߿�
    Key_LineColor,          // �ߵ���ɫ
    Key_LineDashPattern,    // ���߶γ�

    Key_SplineOrder,          // ��������
    Key_SplineNodeCount,      // �����ڵ����

    Key_PgFillColor,          // �������ɫ
    Key_PgLineWidth,          // ��ı߿��
    Key_PgLineColor,          // �߿���ɫ
    Key_PgLineMode,           // �߿�����
    Key_PgLineStyle,          // �ߵ���ʽ
    Key_PgLineDashPattern,    // �߿����߶γ�

    EndDrawSettingKey
};


// ����ģʽ
enum DrawMode 
{
    BeginDrawMode = DrawSettingKey::EndDrawSettingKey,

    None,
    DrawPoint,
    DrawPolyline,              // ����
    DrawSpline,                // ������
    DrawArcThreePoints,        // ����Բ��
    DrawArcTwoPoints,          // ����Բ��
    DrawStreamline,             // ����
    DrawPolygon,                // ����
    DrawComplexPolygon,         // ������

    EndDrawMode
};

enum GeoType
{
    BeginGeoType = EndDrawMode,

    Undefined,                  // δ����
    TypePoint,                  // ��
    TypePolyline,               // ����
    TypeSpline,                 // ������
    TypeArcThreePoints,         // ����Բ��
    TypeArcTwoPoints,           // ����Բ��
    TypeStreamline,             // ����
    TypePolygon,                // ����
    TypeComplexPolygon,         // ������

    EndGeoType
};

enum GeoState
{
    BeginGeoState = EndGeoType,

    GeoStateComplete = 1 << 0,       // ��ɻ��ƣ���������ʱ���Ƶ㣬������꣩
    GeoStateInvalid  = 1 << 2,       // ����
    GeoStateSelected = 1 << 3,       // ѡ�У���ʾ���Ƶ㣩

    EndGeoState = BeginGeoState + 1
};

enum LineStyle {
    BeginStyle = EndGeoState,

    Solid,     // ʵ��
    Dashed,     // ����

    EndStyle
};

enum PointShape {
    BeginPointShape = EndStyle,

    Square,     // ����
    Circle,     // Բ��

    EndPointShape
};
