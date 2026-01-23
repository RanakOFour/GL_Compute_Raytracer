#include "ComputeShader.h"

#include <fstream>
#include <sstream>

#include <iostream>

void PrintActiveUniforms(GLuint _program)
{
    GLint l_count = 0;
    glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &l_count);
    char nameBuf[256];
    for (GLint i = 0; i < l_count; ++i) 
    {
        GLsizei len = 0;
        GLint size = 0;
        GLenum type = 0;
        glGetActiveUniform(_program, (GLuint)i, sizeof(nameBuf), &len, &size, &type, nameBuf);
        GLint loc = glGetUniformLocation(_program, nameBuf);
        printf("active uniform %d: %s size=%d type=0x%x loc=%d\n", i, nameBuf, size, type, loc);
    }
}

ComputeShader::ComputeShader(std::string _path)
{
    printf("Compiling shader at %s\n", _path.c_str());
    std::ifstream l_shaderFile;
    l_shaderFile.open(_path);

    std::stringstream fileStream;
    fileStream << l_shaderFile.rdbuf();
    l_shaderFile.close();

    std::string shaderCode = fileStream.str();

    const char* l_code = shaderCode.c_str();

    GLuint compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &l_code, NULL);
    glCompileShader(compute);

    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
    if(success != GL_TRUE)
    {
        glGetShaderInfoLog(compute, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: Compute\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }

    m_ID = glCreateProgram();
    glAttachShader(m_ID, compute);
    glLinkProgram(m_ID);

    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if(success != GL_TRUE)
    {
        glGetProgramInfoLog(m_ID, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: Program\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }

    glDeleteShader(compute);

    PrintActiveUniforms(m_ID);
}

ComputeShader::~ComputeShader()
{
    // Only delete if we own a valid program (not moved-from)
    if (m_ID != 0)
    {
        glDeleteProgram(m_ID);
    }
}

// Move constructor - transfer ownership
ComputeShader::ComputeShader(ComputeShader&& other) noexcept
    : m_ID(other.m_ID)
{
    other.m_ID = 0;  // Mark source as moved-from
}

// Move assignment - transfer ownership
ComputeShader& ComputeShader::operator=(ComputeShader&& other) noexcept
{
    if (this != &other)
    {
        // Delete our current program if we have one
        if (m_ID != 0)
        {
            glDeleteProgram(m_ID);
        }
        // Take ownership of other's program
        m_ID = other.m_ID;
        other.m_ID = 0;
    }
    return *this;
}

void ComputeShader::use()
{
    glUseProgram(m_ID);
}

void ComputeShader::SetUniform(std::string _name, bool _value)
{
    GLint l_location = glGetUniformLocation(m_ID, _name.c_str());
    
    if(l_location == -1)
    {
        printf("%s location: %i\n", _name.c_str(), l_location);
        return;
    }

    glUniform1i(l_location, _value);
}

void ComputeShader::SetUniform(std::string _name, int _value)
{
    GLint l_location = glGetUniformLocation(m_ID, _name.c_str());
    
    if(l_location == -1)
    {
        printf("%s location: %i\n", _name.c_str(), l_location);
        return;
    }

    glUniform1i(l_location, _value);
}

void ComputeShader::SetUniform(std::string _name, float _value)
{
    GLint l_location = glGetUniformLocation(m_ID, _name.c_str());
    
    if(l_location == -1)
    {
        printf("%s location: %i\n", _name.c_str(), l_location);
        return;
    }

    glUniform1f(l_location, _value);
}

void ComputeShader::SetUniform(std::string _name, glm::vec2 _value)
{
    GLint l_location = glGetUniformLocation(m_ID, _name.c_str());
    
    if(l_location == -1)
    {
        printf("%s location: %i\n", _name.c_str(), l_location);
        return;
    }

    glUniform2fv(l_location, 1, glm::value_ptr(_value));
}

void ComputeShader::SetUniform(std::string _name, glm::vec3 _value)
{
    GLint l_location = glGetUniformLocation(m_ID, _name.c_str());

    if(l_location == -1)
    {
        printf("%s location: %i\n", _name.c_str(), l_location);
        return;
    }

    glUniform3fv(l_location, 1, glm::value_ptr(_value));
}


void ComputeShader::SetUniform(std::string _name, glm::uvec4 _value)
{
    GLint l_location = glGetUniformLocation(m_ID, _name.c_str());

    if(l_location == -1)
    {
        printf("%s location: %i\n", _name.c_str(), l_location);
        return;
    }

    glUniform4uiv(l_location, 1, glm::value_ptr(_value));
}