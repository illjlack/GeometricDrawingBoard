/*
#version 330 core
out vec4 FragColor;

//in vec4 vertexColor; // 从顶点着色器传来的输入变量（名称相同、类型相同）
uniform vec4 ourColor; // 在OpenGL程序代码中设定这个变量

void main()
{
 //FragColor = vertexColor;
    FragColor = ourColor;
}

*/

#version 330 core
out vec4 FragColor;  // 输出的片段颜色

in vec2 TexCoord;  // 从顶点着色器传递来的纹理坐标

uniform sampler2D text;  // 字体纹理
uniform vec3 textColor;  // 文本颜色

void main()
{
    vec4 sampled = texture(text, TexCoord);  // 采样字体纹理
    if (sampled.a < 0.1)  // 如果透明度较低，设置为透明
        discard;
    FragColor = vec4(textColor, 1.0) * sampled;  // 设置最终颜色
}
