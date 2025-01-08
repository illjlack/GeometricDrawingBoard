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
    Key_NodeLineStyle,      // 节点线形

    Key_SplineOrder,          // 样条阶数
    Key_SplineNodeCount,      // 样条节点个数

    Key_PgFillColor,          // 面填充颜色
    Key_PgLineWidth,          // 面的边框宽
    Key_PgLineColor,          // 边框颜色
    Key_PgLineMode,           // 边框类型
    Key_PgLineStyle,          // 线的样式
    Key_PgLineDashPattern,    // 边框虚线段长
    Key_PgNodeLineStyle,      // 边框节点线形

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
    DrawPolygon,                // 简单面
    DrawComplexPolygon,         // 复杂面

    DrawSimpleLine,     // 简单线
    DrawDoubleLine,     // 双线
    DrawParallelLine,   // 平行线
    DrawTwoPointCircle, // 两点画圆
    DrawSimpleArea,     // 简单面
    DrawComplexArea,     // 复杂面

    EndDrawMode
};

enum NodeLineStyle
{
    BeginNodeLineStyle = EndDrawMode,

    NoStyle,                //未定义
    StylePolyline,          // 折线
    StyleSpline,            // 样条线
    StyleThreePointArc,     // 三点圆弧
    StyleArc,               // 圆弧
    StyleStreamline,        // 流线
    
    EndNodeLineStyle,
};

enum GeoType
{
    BeginGeoType = EndDrawMode,

    Undefined,                  // 未定义
    TypePoint,                  // 点
    TypePolyline,               // 折线
    TypeSpline,                 // 样条线
    TypeArcThreePoints,         // 三点圆弧
    TypeArcTwoPoints,           // 两点圆弧
    TypeStreamline,             // 流线
    TypePolygon,                // 简单面
    TypeComplexPolygon,         // 复杂面

    TypeSimpleLine,     // 简单线
    TypeDoubleLine,     // 双线
    TypeParallelLine,   // 平行线
    TypeTwoPointCircle, // 两点画圆
    TypeSimpleArea,     // 简单面
    TypeComplexArea,     // 复杂面

    EndGeoType
};

enum GeoState
{
    BeginGeoState = EndGeoType,

    GeoStateInitialized = 1 << 0,
    GeoStateComplete = 1 << 1,       // 完成绘制（不加入零时控制点，跟踪鼠标）
    GeoStateInvalid  = 1 << 2,       // 作废
    GeoStateSelected = 1 << 3,       // 选中（显示控制点）

    EndGeoState = BeginGeoState + 1
};

enum LineStyle {
    BeginStyle = EndGeoState,

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
