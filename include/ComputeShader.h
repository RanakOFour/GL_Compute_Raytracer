#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include "GL/glew.h"
#include "GLM/ext.hpp"

#include <string>

class ComputeShader
{
    public:
    ComputeShader(std::string _path);
    ~ComputeShader();

    void use();
    void SetUniform(std::string _name, int _value);
    void SetUniform(std::string _name, float _value);
    void SetUniform(std::string _name, glm::vec2 _value);
    void SetUniform(std::string _name, glm::vec3 _value);

    private:
    GLint m_ID;

};

#endif