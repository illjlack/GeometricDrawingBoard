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

    Key_PgFillColor,          // �������ɫ
    Key_PgLineWidth,          // ��ı߿��
    Key_PgLineColor,          // �߿���ɫ
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
    DrawPolygon,

    EndDrawMode
};

enum GeoType
{
    BeginGeoType = EndDrawMode,

    Undefined,             // δ����
    TypePoint,                 // ��
    TypePolyline,              // ����
    TypeSpline,                // ������
    TypeArcThreePoints,        // ����Բ��
    TypeArcTwoPoints,          // ����Բ��
    TypeStreamline,             // ����

    EndGeoType
};

enum GeoDrawState
{
    BeginGeoDrawState = EndGeoType,

    Complete,   // ��ɻ���
    Drawing,     // ���ڻ���

    EndGeoDrawState
};

enum LineStyle {
    BeginStyle = EndGeoDrawState,

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
