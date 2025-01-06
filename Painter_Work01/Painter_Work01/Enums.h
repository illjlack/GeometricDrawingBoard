#pragma once

// �����ö����̫���ˣ����Զ�ͳһ�����
// 
// Ϊ�˷������ö�ʹ�������������Ƶġ�


enum DrawSettingKey 
{
    BeginDrawSettingKey = 0,

    Key_DrawMode,   //ѡ����Ƶ����ͣ���,����...��
    Key_LineWidth,
    Key_LineColor,
    Key_FillColor,
    Key_Opacity,

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

