### 模块

#### mainWindow

窗口

选择绘制方式、颜色、大小

#### Canvas

画布

处理绘制、点击事件，管理几何图形

#### Geo

几何图形类

管理自己的点、颜色、怎么绘制

处理点击之类的事件来更新图形

#### mathUtil

用来计算获得需要的图形的轮廓等

#### DrawSettings

绘制设置

主要是mainWindow的关于绘制的设置不方便传递。

直接全局保存获得。

（对于某个图形的怎么调整？？？）

（可以试试，把选中图形放到一起，获得选中时设置）





### 缓冲区分析

1.直接画很多圆和矩形无边框覆盖成缓冲区

复杂度*=画圆,且不能画出外轮廓。



2.画出外轮廓填充。

![image-20250106091527252](./readme.assets/image-20250106091527252.png)



一般情况，计算锐角里的一个交点，钝角中的圆弧











```cpp
屎山代码就是这么堆的,我悟了
```



感觉又要改：

是选择：简单线、双线、平行线、两点画圆、简单面、复杂面

节点线型：折线、样条线、三点圆弧、流线

线型：实线、虚线

```cpp
enum class DrawMode
{
    SimpleLine,     // 简单线
    DoubleLine,     // 双线
    ParallelLine,   // 平行线
    TwoPointCircle, // 两点画圆
    SimpleArea,     // 简单面
    ComplexArea     // 复杂面
};
enum class NodeLineStyle 
{
    Polyline,       // 折线
    Spline,         // 样条线
    ThreePointArc,  // 三点圆弧
    Arc,			// 普通圆弧
    Streamline      // 流线
};
```

