#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <iostream>

// �ַ���Ϣ�ṹ��
struct Character {
    GLuint TextureID;  // �ַ������ OpenGL ID
    glm::ivec2 Size;   // �ַ�����Ŀ��
    glm::ivec2 Bearing;// �ַ���ƫ�������ӻ��߿�ʼ��
    GLuint Advance;    // �ַ���ࣨ��1/64����Ϊ��λ��
};

// ������Ⱦ�࣬��װ�� FreeType �� OpenGL ������߼�
class TextRenderer {
public:
    // ���캯������ʼ������� OpenGL ������
    TextRenderer(const std::string& fontPath, unsigned int windowWidth, unsigned int windowHeight);

    // �����������ͷ� OpenGL ��Դ
    ~TextRenderer();

    // ��Ⱦ����
    void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color);

    // ������ɫ������
    void setShaderProgram(GLuint program);

private:
    unsigned int width, height;             // ���ڿ�Ⱥ͸߶�
    std::map<char, Character> Characters;  // �ַ�ӳ���
    GLuint VAO, VBO;                        // OpenGL �����������ͻ���������
    GLuint shaderProgram;                   // ��ɫ������ ID

    // ��ʼ�� FreeType �Ⲣ��������
    bool initFreeType(const std::string& fontPath);

    // ��ʼ�� OpenGL ������
    void initBuffers();
};

#endif // TEXT_RENDERER_H
