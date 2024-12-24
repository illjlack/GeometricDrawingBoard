/*
#version 330 core
layout (location = 0) in vec3 aPos; // 位置变量的属性位置值为0

out vec4 vertexColor;

void main()
{
    gl_Position = vec4(aPos, 1.0); // 把一个vec3作为vec4的构造器的参数
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // 把输出变量设置为暗红色
}
*/

#version 330 core
layout (location = 0) in vec4 vertex;  // 传入的顶点位置和纹理坐标

out vec2 TexCoord;  // 输出的纹理坐标

void main()
{
    gl_Position = vec4(vertex.x, vertex.y, 0.0, 1.0);  // 设置顶点位置
    TexCoord = vertex.zw;  // 将纹理坐标传递到片段着色器
}
