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





object派生出

点、线、面

点保存节点位置

线保存节点位置

面保存节点位置（可能点击不好判断）



