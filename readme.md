### !!!bug

1. 在着色器绑定外去设置矩阵是无效的！！

2. 奇怪的需要makeCurrent()

   ```cpp
   void OpenGLWidget::pushObject(Object* obj)
   {
       makeCurrent();
       obj->setShader(shaderProgram);
       obj->initialize();  // 确保此时上下文有效
       vec.push_back(obj);
       doneCurrent();
   }
   ```

   ```cpp
   void initialize()
       {
           // 检查当前 OpenGL 上下文是否有效
           QOpenGLContext* currentContext = QOpenGLContext::currentContext();
           if (!currentContext) {
               qWarning() << "OpenGL context is not available!";
               return;
           }
           // 获取 OpenGL 函数接口
           QOpenGLFunctions* functions = currentContext->functions();
           if (!functions) {
               qWarning() << "Failed to get OpenGL functions!";
               return;
           }
       ...
   }
   ```

   明明上下文Context都检查了，没报错，但是少了makeCurrent();就绘制无效了。

   （QOpenGLWidget派生类自定义的函数要自己加makeCurrent()，然后当时一直奇怪为什么非要放在paintGL()里有效）

   猜测可能与FBO有关，因为makeCurrent()里除了上下文就是fbo了。

3.qt里，槽用lambda表达式的时候，捕获外部指针不要引用，，，用复制



### QOpenGLWidget

#### **OpenGL 生命周期相关函数**

这些函数允许你在适当的时间点初始化、调整大小和绘制 OpenGL 内容：

- **`initializeGL()`**
  用于初始化 OpenGL 状态和资源（例如加载着色器、初始化缓冲区等）。通常在窗口小部件第一次显示时调用。
- **`resizeGL(int w, int h)`**
  当窗口小部件大小发生变化时调用，用于调整 OpenGL 的视口或其他依赖大小的状态。
- **`paintGL()`**
  在绘制 OpenGL 内容时调用。用户应在此函数中实现具体的绘制逻辑。



#### **OpenGL 上下文管理**

这些函数用于控制和获取 OpenGL 上下文：

- **`context() const`**
  返回当前窗口小部件关联的 OpenGL 上下文指针（`QOpenGLContext`）。可以通过该上下文访问 GPU 资源或与其他上下文共享资源。
- **`makeCurrent()`**
  将当前窗口小部件的 OpenGL 上下文设置为当前上下文，之后的 OpenGL 操作都会基于此上下文。
- **`doneCurrent()`**
  释放当前的 OpenGL 上下文，使其不再是当前上下文。



如果您在 **`QOpenGLWidget` 的范围之外** 使用 OpenGL，就需要自己管理上下文

在 OpenGL 中，上下文 (Context) 是一个重要的概念。它是一个运行环境，保存了所有 OpenGL 状态和资源的集合，包括顶点缓冲区对象、纹理、帧缓冲区等。OpenGL 上下文提供了一个接口，供应用程序与 GPU 交互。

因为 OpenGL 是一个状态机，其状态存储在上下文中。如果在没有激活上下文的情况下调用 OpenGL 函数，这些函数将无法工作，甚至可能导致程序崩溃。

```cpp
QOpenGLContext *ctx = myOpenGLWidget->context();
if (ctx) {
    ctx->makeCurrent(myOpenGLWidget); // 设置上下文为当前上下文
    // 在此处调用 OpenGL 函数进行绘制操作
    ctx->doneCurrent(); // 释放当前上下文
}
```









- **模型矩阵**：用于物体的**平移**、**旋转**、**缩放**，将物体从局部坐标系转换到世界坐标系。

- **视图矩阵**：用于设置相机的位置和方向，决定了场景如何从相机的视角展示。

- **投影矩阵**：定义了如何将三维场景映射到二维屏幕上，控制物体的透视效果和屏幕显示的范围。

  

### 1. 模型矩阵 (Model Matrix)

- **功能**：控制物体在世界坐标系中的位置、旋转和缩放。

- **描述**：模型矩阵将物体从局部空间（物体的原始坐标系统）转换到世界空间。通过这个矩阵，我们可以操作物体的**位置**（平移）、**旋转**和**缩放**。

- 常见操作

  ：

  - **平移**（Translation）：将物体沿某个方向移动。
  - **旋转**（Rotation）：将物体绕某个轴旋转。
  - **缩放**（Scaling）：改变物体的大小。

模型矩阵通常由这几种操作的矩阵相乘得到。例如，若要对物体进行平移、旋转和缩放，则会按顺序组合这些变换矩阵：

```cpp
glm::mat4 model = glm::mat4(1.0f);  // 初始化为单位矩阵

// 进行平移、旋转、缩放等操作
model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));  // 平移
model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // 旋转
model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));  // 缩放
```

模型矩阵在渲染每个物体时都会单独计算和应用。

### 2. 视图矩阵 (View Matrix)

- **功能**：控制场景如何从相机的视角呈现。

- **描述**：视图矩阵将世界坐标系中的物体变换到相机空间中，从而决定了**摄像机的位置和方向**。它描述了相机的“观察”位置，控制相机如何“看”世界。

- 常见操作

  ：

  - **相机位置**（Camera Position）：确定相机在世界坐标系中的位置。
  - **目标位置**（LookAt）：确定相机要看向的点（通常是场景的中心或某个物体）。
  - **上方向**（Up Direction）：控制相机的“上”方向，通常是垂直方向（Y轴）。

视图矩阵通常通过 `glm::lookAt` 函数来创建，该函数使用相机的位置、目标位置和上方向来计算视图矩阵：

```cpp
glm::mat4 view = glm::lookAt(
    glm::vec3(0.0f, 0.0f, 3.0f),  // 相机位置
    glm::vec3(0.0f, 0.0f, 0.0f),  // 观察目标
    glm::vec3(0.0f, 1.0f, 0.0f)   // 上方向（通常为 Y 轴）
);
```

这个矩阵决定了相机如何观察场景，从而影响最终画面中的视角。

### 3. 投影矩阵 (Projection Matrix)

- **功能**：控制如何将三维场景投影到二维屏幕上，定义视锥体的形状。
- 描述：投影矩阵决定了场景中物体如何在二维屏幕上显示，特别是物体的大小和透视效果。
- 投影矩阵有两种常见类型：
  - **透视投影**（Perspective Projection）：模拟人眼看到的效果，物体远离相机时看起来较小，产生透视效果。
  - **正交投影**（Orthographic Projection）：不产生透视效果，物体无论离相机多远，都保持相同大小。适用于需要精确比例的场景，如2D游戏或建筑可视化。

#### 透视投影矩阵：

透视投影矩阵通过参数设置视场角、近平面和远平面的距离、宽高比等来创建，常见的函数是 `glm::perspective`：

```cpp
glm::mat4 projection = glm::perspective(
    glm::radians(45.0f),        // 视场角（FOV）
    float(width) / float(height), // 宽高比
    0.1f,                       // 近平面
    100.0f                      // 远平面
);
```

#### 正交投影矩阵：

正交投影矩阵的函数是 `glm::ortho`，它用于创建正交投影效果：

```cpp
glm::mat4 projection = glm::ortho(
    -1.0f, 1.0f,   // 左右裁剪面
    -1.0f, 1.0f,   // 上下裁剪面
    0.1f, 100.0f   // 近平面和远平面
);
```



