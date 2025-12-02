#include "ComputeShader.h"

#include <fstream>
#include <sstream>

#include <iostream>


ComputeShader::ComputeShader(std::string _path)
{
    printf("Compiling shader at %s\n", _path.c_str());
    std::ifstream shaderFile;
    shaderFile.open(_path);

    std::stringstream fileStream;
    fileStream << shaderFile.rdbuf();
    shaderFile.close();

    std::string shaderCode = fileStream.str();

    const char* l_code = shaderCode.c_str();

    GLuint compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &l_code, NULL);
    glCompileShader(compute);

    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(compute, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: Compute\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }

    m_ID = glCreateProgram();
    glAttachShader(m_ID, compute);
    glLinkProgram(m_ID);

    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(m_ID, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: Program\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }

    glDeleteShader(compute);
}

ComputeShader::~ComputeShader()
{
    // Delete shader innit
    glDeleteProgram(m_ID);
}

void ComputeShader::use()
{
    glUseProgram(m_ID);
}

void ComputeShader::SetUniform(std::string _name, bool _value)
{
    glUniform1i(glGetUniformLocation(m_ID, _name.c_str()), _value);
}

void ComputeShader::SetUniform(std::string _name, int _value)
{
    glUniform1i(glGetUniformLocation(m_ID, _name.c_str()), _value);
}

void ComputeShader::SetUniform(std::string _name, float _value)
{
    glUniform1f(glGetUniformLocation(m_ID, _name.c_str()), _value);
}

void ComputeShader::SetUniform(std::string _name, glm::vec2 _value)
{
    glUniform2fv(glGetUniformLocation(m_ID, _name.c_str()), 1, glm::value_ptr(_value));
}

void ComputeShader::SetUniform(std::string _name, glm::vec3 _value)
{
    glUniform3fv(glGetUniformLocation(m_ID, _name.c_str()), 1, glm::value_ptr(_value));
}