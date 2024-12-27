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

// 字符信息结构体
struct Character {
    GLuint TextureID;  // 字符纹理的 OpenGL ID
    glm::ivec2 Size;   // 字符纹理的宽高
    glm::ivec2 Bearing;// 字符的偏移量（从基线开始）
    GLuint Advance;    // 字符间距（以1/64像素为单位）
};

// 字体渲染类，封装了 FreeType 和 OpenGL 的相关逻辑
class TextRenderer {
public:
    // 构造函数，初始化字体和 OpenGL 缓冲区
    TextRenderer(const std::string& fontPath, unsigned int windowWidth, unsigned int windowHeight);

    // 析构函数，释放 OpenGL 资源
    ~TextRenderer();

    // 渲染文字
    void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color);

    // 设置着色器程序
    void setShaderProgram(GLuint program);

private:
    unsigned int width, height;             // 窗口宽度和高度
    std::map<char, Character> Characters;  // 字符映射表
    GLuint VAO, VBO;                        // OpenGL 顶点数组对象和缓冲区对象
    GLuint shaderProgram;                   // 着色器程序 ID

    // 初始化 FreeType 库并加载字体
    bool initFreeType(const std::string& fontPath);

    // 初始化 OpenGL 缓冲区
    void initBuffers();
};

#endif // TEXT_RENDERER_H
