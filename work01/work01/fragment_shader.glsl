/*
#version 330 core
out vec4 FragColor;

//in vec4 vertexColor; // �Ӷ�����ɫ�����������������������ͬ��������ͬ��
uniform vec4 ourColor; // ��OpenGL����������趨�������

void main()
{
 //FragColor = vertexColor;
    FragColor = ourColor;
}

*/

#version 330 core
out vec4 FragColor;  // �����Ƭ����ɫ

in vec2 TexCoord;  // �Ӷ�����ɫ������������������

uniform sampler2D text;  // ��������
uniform vec3 textColor;  // �ı���ɫ

void main()
{
    vec4 sampled = texture(text, TexCoord);  // ������������
    if (sampled.a < 0.1)  // ���͸���Ƚϵͣ�����Ϊ͸��
        discard;
    FragColor = vec4(textColor, 1.0) * sampled;  // ����������ɫ
}
