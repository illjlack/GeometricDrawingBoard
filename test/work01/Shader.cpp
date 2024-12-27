#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/ext/vector_float2.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>

// 构造函数：从指定文件路径加载并构建着色器程序
Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. 从文件中读取顶点和片段着色器的代码
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // 确保ifstream对象可以抛出异常
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // 打开文件
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        // 读取文件内容到流中
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // 关闭文件
        vShaderFile.close();
        fShaderFile.close();

        // 转换为字符串
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    // 将读取到的字符串转换为 C 字符串，供 OpenGL 使用
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. 编译着色器
    unsigned int vertex, fragment;

    // 编译顶点着色器
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL); // 设置顶点着色器源代码
    glCompileShader(vertex); // 编译顶点着色器
    checkCompileErrors(vertex, "VERTEX"); // 检查编译是否成功

    // 编译片段着色器
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL); // 设置片段着色器源代码
    glCompileShader(fragment); // 编译片段着色器
    checkCompileErrors(fragment, "FRAGMENT"); // 检查编译是否成功

    // 链接着色器程序
    ID = glCreateProgram(); // 创建着色器程序
    glAttachShader(ID, vertex); // 将顶点着色器附加到程序
    glAttachShader(ID, fragment); // 将片段着色器附加到程序
    glLinkProgram(ID); // 链接着色器程序
    checkCompileErrors(ID, "PROGRAM"); // 检查程序链接是否成功

    // 删除已附加的着色器，因为它们已经被链接到程序中，不再需要
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

// 激活着色器程序，使其成为当前正在使用的着色器程序
void Shader::use()
{
    glUseProgram(ID);
}

// 设置 bool 类型的 uniform 变量
void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

// 设置 int 类型的 uniform 变量
void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

// 设置 float 类型的 uniform 变量
void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

// 设置 glm::vec2 类型的 uniform 变量
void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

// 设置 glm::vec2 类型的 uniform 变量，使用 x 和 y 分量
void Shader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

// 设置 glm::vec3 类型的 uniform 变量
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

// 设置 glm::vec3 类型的 uniform 变量，使用 x, y, z 分量
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

// 设置 glm::vec4 类型的 uniform 变量
void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

// 设置 glm::vec4 类型的 uniform 变量，使用 x, y, z, w 分量
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

// 设置 glm::mat2 类型的 uniform 变量
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

// 设置 glm::mat3 类型的 uniform 变量
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

// 设置 glm::mat4 类型的 uniform 变量
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

// 检查着色器编译或链接时的错误
void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") // 如果是着色器
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success); // 获取编译状态
        if (!success) // 如果编译失败
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog); // 获取错误日志
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
    else // 如果是程序链接
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success); // 获取链接状态
        if (!success) // 如果链接失败
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog); // 获取错误日志
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
}
