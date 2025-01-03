

### **1. 框架结构**

大致可以分为以下几个模块：

1. **主窗口** (`MainWindow`): 包含菜单栏、工具栏、状态栏等全局控件。
2. **绘图区域** (`Canvas`): 一个自定义控件，用于处理绘图和用户交互。
3. **图形管理** (`Shapes`): 定义不同类型的图形（如矩形、圆形、线条等）的类。
4. **工具模块**: 包含绘图工具（画笔、颜色选择、图形选择等）。
5. **数据管理模块**: 保存绘图数据，支持撤销、重做和文件保存。

------

### **2. 主窗口设计 (`MainWindow`)**

主窗口是整个程序的入口，主要功能包括：

- 菜单栏：文件操作（新建、打开、保存等）。
- 工具栏：绘图工具（选择矩形、圆形、直线、橡皮擦等）。
- 状态栏：显示鼠标坐标、选中图形的信息等。

#### 主窗口类结构：

```cpp
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createMenuBar();   // 创建菜单栏
    void createToolBar();   // 创建工具栏
    void createStatusBar(); // 创建状态栏

    Canvas *canvas;         // 自定义绘图区域
};
```

------

### **3. 绘图区域设计 (`Canvas`)**

`Canvas` 是用户实际绘图的地方，需要继承 `QWidget` 并重写以下方法：

- `paintEvent(QPaintEvent *event)`: 使用 `QPainter` 绘制图形。
- `mousePressEvent(QMouseEvent *event)`: 处理鼠标按下事件。
- `mouseMoveEvent(QMouseEvent *event)`: 处理鼠标移动事件。
- `mouseReleaseEvent(QMouseEvent *event)`: 处理鼠标释放事件。

#### 示例代码：

```cpp
class Canvas : public QWidget {
    Q_OBJECT

public:
    explicit Canvas(QWidget *parent = nullptr);

    void setCurrentShape(int shapeType); // 设置当前绘图形状
    void setPen(const QPen &pen);        // 设置画笔
    void setBrush(const QBrush &brush); // 设置画刷

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QVector<QSharedPointer<Shape>> shapes; // 保存所有图形
    QSharedPointer<Shape> currentShape;   // 当前正在绘制的图形
    QPen pen;
    QBrush brush;
};
```

------

### **4. 图形管理模块 (`Shapes`)**

不同图形可以抽象为一个基类，并为每种图形定义子类。

```cpp
class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw(QPainter &painter) = 0; // 绘制方法
    virtual void setStartPoint(const QPoint &point) = 0;
    virtual void setEndPoint(const QPoint &point) = 0;
};

class Rectangle : public Shape {
public:
    void draw(QPainter &painter) override;
    void setStartPoint(const QPoint &point) override;
    void setEndPoint(const QPoint &point) override;

private:
    QRect rect;
};
```

------

### **5. 工具模块**

工具模块主要用于选择画笔、画刷和图形类型：

- 画笔设置工具：颜色、宽度、样式。
- 图形选择工具：矩形、圆形、线条、自由绘制等。

#### 示例：

```cpp
class ToolBar : public QToolBar {
    Q_OBJECT

public:
    explicit ToolBar(QWidget *parent = nullptr);

signals:
    void shapeSelected(int shapeType);
    void penChanged(const QPen &pen);
    void brushChanged(const QBrush &brush);
};
```

------

### **6. 数据管理模块**

支持撤销、重做和文件保存功能：

- **撤销和重做**：通过栈（`QStack`）保存绘图操作的历史记录。
- **文件保存和加载**：使用 `QImage` 或 `QPixmap` 保存到文件，支持常见图片格式（如 PNG、JPEG）。

```cpp
class DataManager {
public:
    void saveToFile(const QString &filePath, const QVector<QSharedPointer<Shape>> &shapes);
    QVector<QSharedPointer<Shape>> loadFromFile(const QString &filePath);

    void undo();
    void redo();

private:
    QStack<QVector<QSharedPointer<Shape>>> undoStack;
    QStack<QVector<QSharedPointer<Shape>>> redoStack;
};
```

------

### **7. 主程序入口**

通过 `QApplication` 启动主窗口：

```cpp
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
```



