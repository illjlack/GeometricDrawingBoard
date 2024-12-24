/*
#version 330 core
layout (location = 0) in vec3 aPos; // λ�ñ���������λ��ֵΪ0

out vec4 vertexColor;

void main()
{
    gl_Position = vec4(aPos, 1.0); // ��һ��vec3��Ϊvec4�Ĺ������Ĳ���
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // �������������Ϊ����ɫ
}
*/

#version 330 core
layout (location = 0) in vec4 vertex;  // ����Ķ���λ�ú���������

out vec2 TexCoord;  // �������������

void main()
{
    gl_Position = vec4(vertex.x, vertex.y, 0.0, 1.0);  // ���ö���λ��
    TexCoord = vertex.zw;  // ���������괫�ݵ�Ƭ����ɫ��
}
