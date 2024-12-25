#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/ext/vector_float2.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>

// ���캯������ָ���ļ�·�����ز�������ɫ������
Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. ���ļ��ж�ȡ�����Ƭ����ɫ���Ĵ���
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // ȷ��ifstream��������׳��쳣
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // ���ļ�
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        // ��ȡ�ļ����ݵ�����
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // �ر��ļ�
        vShaderFile.close();
        fShaderFile.close();

        // ת��Ϊ�ַ���
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    // ����ȡ�����ַ���ת��Ϊ C �ַ������� OpenGL ʹ��
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. ������ɫ��
    unsigned int vertex, fragment;

    // ���붥����ɫ��
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL); // ���ö�����ɫ��Դ����
    glCompileShader(vertex); // ���붥����ɫ��
    checkCompileErrors(vertex, "VERTEX"); // �������Ƿ�ɹ�

    // ����Ƭ����ɫ��
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL); // ����Ƭ����ɫ��Դ����
    glCompileShader(fragment); // ����Ƭ����ɫ��
    checkCompileErrors(fragment, "FRAGMENT"); // �������Ƿ�ɹ�

    // ������ɫ������
    ID = glCreateProgram(); // ������ɫ������
    glAttachShader(ID, vertex); // ��������ɫ�����ӵ�����
    glAttachShader(ID, fragment); // ��Ƭ����ɫ�����ӵ�����
    glLinkProgram(ID); // ������ɫ������
    checkCompileErrors(ID, "PROGRAM"); // �����������Ƿ�ɹ�

    // ɾ���Ѹ��ӵ���ɫ������Ϊ�����Ѿ������ӵ������У�������Ҫ
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

// ������ɫ������ʹ���Ϊ��ǰ����ʹ�õ���ɫ������
void Shader::use()
{
    glUseProgram(ID);
}

// ���� bool ���͵� uniform ����
void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

// ���� int ���͵� uniform ����
void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

// ���� float ���͵� uniform ����
void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

// ���� glm::vec2 ���͵� uniform ����
void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

// ���� glm::vec2 ���͵� uniform ������ʹ�� x �� y ����
void Shader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

// ���� glm::vec3 ���͵� uniform ����
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

// ���� glm::vec3 ���͵� uniform ������ʹ�� x, y, z ����
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

// ���� glm::vec4 ���͵� uniform ����
void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

// ���� glm::vec4 ���͵� uniform ������ʹ�� x, y, z, w ����
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

// ���� glm::mat2 ���͵� uniform ����
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

// ���� glm::mat3 ���͵� uniform ����
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

// ���� glm::mat4 ���͵� uniform ����
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

// �����ɫ�����������ʱ�Ĵ���
void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") // �������ɫ��
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success); // ��ȡ����״̬
        if (!success) // �������ʧ��
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog); // ��ȡ������־
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
    else // ����ǳ�������
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success); // ��ȡ����״̬
        if (!success) // �������ʧ��
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog); // ��ȡ������־
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
}
