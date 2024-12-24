#version 330 core
out vec4 FragColor;

//in vec4 vertexColor; // 从顶点着色器传来的输入变量（名称相同、类型相同）
uniform vec4 ourColor; // 在OpenGL程序代码中设定这个变量

void main()
{
 //FragColor = vertexColor;
    FragColor = ourColor;
}