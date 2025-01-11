



### 绘制平滑的缓冲区

使用平行线画的话。自相交的问题不好解决。

所以使用栅格做法。

输入是矢量图（线）（有序的点，差不多是折线），先栅格化（Bresenham 直线算法），点的个数m。

求出图的范围后,枚举每个场景中的每个点，到线的距离。所以映射到的场景的精度越高，越耗时间。

在范围内的标记。

然后找到边界的点（有标记的格子周围有没标记的，就是边界）。此时是无序的点。因为边界是不相交闭合的，可以使用搜索来确定顺序。



因为精度限制，算出来的线上会有很多“凸起”，是因为误差。

样条不能平滑，因为这些误差也会让样条凸起。

所以先使用`Douglas-Peucker` 算法（通过忽略偏差范围内点简化为线段）使误差被忽略。

然后用样条平滑。



为了让精度更高，可以使用分块，先求出边界附近的格子，然后不断提高精度。

枚举线上每一点的做法也很粗糙。也可以分块。





算法流程：

1. 计算矢量图的范围

2. 设置一个映射的比例, 用来映射矢量图到格栅图

   ```cpp
   struct GridMap
   {
       double scale;                            // 缩放比例
       QPointF offset;                          // 偏移量（网格原点对应的原来坐标）
   };
   ```

3. ~~映射矢量图到栅格里，Bresenham 直线算法连上相邻两个点~~    (我敲，是不是可以直接计算与矢量线段的距离)
4. 遍历栅格，求出边界上的点。
5. 不断提高映射比例，只计算边界上的点表示的块



```cpp
// 将原始坐标映射到网格坐标
void mapToGrid(const GridMap& gridMap, const QPointF& worldPos, QPoint& gridPos)
{
    gridPos.setX((worldPos.x() - gridMap.offset.x()) / gridMap.scale);
    gridPos.setY((worldPos.y() - gridMap.offset.y()) / gridMap.scale);
}

// 从网格坐标恢复到原始坐标
void restoreFromGrid(const GridMap& gridMap, const QPoint& gridPos, QPointF& worldPos)
{
    worldPos.setX(gridPos.x() * gridMap.scale + gridMap.offset.x());
    worldPos.setY(gridPos.y() * gridMap.scale + gridMap.offset.y());
}


// 计算点到线段的垂直距离
double perpendicularDistance(const QPointF& point, const QPointF& start, const QPointF& end) 
{
    double x1 = start.x(), y1 = start.y();
    double x2 = end.x(), y2 = end.y();
    double x0 = point.x(), y0 = point.y();

    // 计算线段的长度
    double dx = x2 - x1;
    double dy = y2 - y1;
    double segmentLength = std::sqrt(dx * dx + dy * dy);

    // 如果线段的长度为零，返回点到起点的距离
    if (segmentLength == 0) 
    {
        return std::sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));
    }

    // 计算投影点的比例t
    double t = ((x0 - x1) * dx + (y0 - y1) * dy) / (segmentLength * segmentLength);
    t = std::max(0.0, std::min(1.0, t));  // 限制t在0到1之间

    // 计算投影点的坐标
    double projection_x = x1 + t * dx;
    double projection_y = y1 + t * dy;

    // 计算并返回点到投影点的距离
    return std::sqrt((x0 - projection_x) * (x0 - projection_x) + (y0 - projection_y) * (y0 - projection_y));
}
```









