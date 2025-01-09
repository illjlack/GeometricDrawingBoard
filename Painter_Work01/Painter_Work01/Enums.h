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

    Key_LineStyle,          // 线的样式(实线、虚线)
    Key_LineWidth,          // 线宽
    Key_LineColor,          // 线的颜色
    Key_LineDashPattern,    // 虚线段长
    Key_NodeLineStyle,      // 节点线形
    Key_FillColor,          // 面填充颜色

    Key_SplineOrder,          // 样条阶数
    Key_SplineNodeCount,      // 样条节点个数
    Key_Steps,                // 计算曲线的点的密度

    EndDrawSettingKey
};


// 绘制模式
enum DrawMode 
{
    BeginDrawMode = DrawSettingKey::EndDrawSettingKey,

    None,
    DrawPoint,
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
    StyleArc,               // 圆弧（实际也是三点）
    StyleTwoPointCircle,    // 两点
    StyleStreamline,        // 流线
    
    EndNodeLineStyle,
};

enum GeoType
{
    BeginGeoType = EndDrawMode,

    Undefined,                  // 未定义
    TypePoint,                  // 点
    TypeSimpleLine,             // 简单线
    TypeDoubleLine,             // 双线
    TypeParallelLine,           // 平行线
    TypeTwoPointCircle,         // 两点画圆
    TypeSimpleArea,             // 简单面
    TypeComplexArea,            // 复杂面

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

    Solid,      // 实线
    Dashed,     // 虚线

    EndStyle
};

enum PointShape {
    BeginPointShape = EndStyle,

    Square,     // 方形
    Circle,     // 圆形

    EndPointShape
};
