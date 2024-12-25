#pragma once

#include <glad/glad.h>  // ���� glad ��ͷ�ļ������� OpenGL ��������
#include <string>       // ���� string �⣬���ڴ����ַ���
#include <glm/fwd.hpp>

// Shader �ࣺ���ڹ��� OpenGL ��ɫ������
class Shader
{
public:
    unsigned int ID; // ��ɫ������� ID�������� OpenGL ��������ɫ������

    // ���캯������ָ�����ļ�·�����ز�������ɫ������
    Shader(const char* vertexPath, const char* fragmentPath);

    // ������ɫ���������õ�ǰ����Ϊʹ�õ���ɫ��
    void use();

    // ���� uniform ������ʵ�ú������������ݴ��ݵ���ɫ��
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
    // �����ɫ�����������ʱ�Ĵ���
    void checkCompileErrors(unsigned int shader, std::string type); // �����������ʱ�Ƿ���ִ���
};
