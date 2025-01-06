#pragma once

// 用类的枚举量太杂了，所以都统一放这里。
// 
// 为了方便设置都使用无作用域限制的。


enum DrawSettingKey 
{
    BeginDrawSettingKey = 0,

    Key_DrawMode,   // 选择绘制的类型（点,折线...）

    Key_PointShape, // 点的形状
    Key_PointColor, // 点的颜色

    Key_LineStyle,          // 线的样式
    Key_LineWidth,          // 线宽
    Key_LineColor,          // 线的颜色
    Key_LineDashPattern,    // 虚线段长

    Key_PgFillColor,          // 面填充颜色
    Key_PgLineWidth,          // 面的边框宽
    Key_PgLineColor,          // 边框颜色
    Key_PgLineStyle,          // 线的样式
    Key_PgLineDashPattern,    // 边框虚线段长

    EndDrawSettingKey
};


// 绘制模式
enum DrawMode 
{
    BeginDrawMode = DrawSettingKey::EndDrawSettingKey,

    None,
    DrawPoint,
    DrawPolyline,              // 折线
    DrawSpline,                // 样条线
    DrawArcThreePoints,        // 三点圆弧
    DrawArcTwoPoints,          // 两点圆弧
    DrawStreamline,             // 流线
    DrawPolygon,

    EndDrawMode
};

enum GeoType
{
    BeginGeoType = EndDrawMode,

    Undefined,             // 未定义
    TypePoint,                 // 点
    TypePolyline,              // 折线
    TypeSpline,                // 样条线
    TypeArcThreePoints,        // 三点圆弧
    TypeArcTwoPoints,          // 两点圆弧
    TypeStreamline,             // 流线

    EndGeoType
};

enum GeoDrawState
{
    BeginGeoDrawState = EndGeoType,

    Complete,   // 完成绘制
    Drawing,     // 正在绘制

    EndGeoDrawState
};

enum LineStyle {
    BeginStyle = EndGeoDrawState,

    Solid,     // 实线
    Dashed,     // 虚线

    EndStyle
};

enum PointShape {
    BeginPointShape = EndStyle,

    Square,     // 方形
    Circle,     // 圆形

    EndPointShape
};
