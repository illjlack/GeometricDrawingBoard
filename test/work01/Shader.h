#pragma once

#include <glad/glad.h>  // 导入 glad 库头文件，管理 OpenGL 函数加载
#include <string>       // 导入 string 库，用于处理字符串
#include <glm/fwd.hpp>

// Shader 类：用于管理 OpenGL 着色器程序
class Shader
{
public:
    unsigned int ID; // 着色器程序的 ID，用于在 OpenGL 中引用着色器程序

    // 构造函数：从指定的文件路径加载并构建着色器程序
    Shader(const char* vertexPath, const char* fragmentPath);

    // 激活着色器程序，设置当前程序为使用的着色器
    void use();

    // 设置 uniform 变量的实用函数，允许将数据传递到着色器
    void setBool(const std::string& name, bool value) const;  
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    // 检查着色器编译或链接时的错误
    void checkCompileErrors(unsigned int shader, std::string type); // 检查编译或链接时是否出现错误
};
