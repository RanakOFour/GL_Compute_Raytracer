#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include "GL/glew.h"
#include "GLM/ext.hpp"

#include <string>

/*
*   Wrapper for OpenGL Compute shaders
*/
class ComputeShader
{
    public:
    /*
    *   Loads a new Compute shader from the given filepath.
    *   Program does not crash if a wrong path is given.
    */
    ComputeShader(std::string _path);
    ~ComputeShader();

    /*
    *   Sets the shader to be used by OpenGl
    */
    void use();

    /// @{
    /** Sets the value of the uniform in the shader program. */ 
    void SetUniform(std::string _name, bool _value);
    void SetUniform(std::string _name, int _value);
    void SetUniform(std::string _name, float _value);
    void SetUniform(std::string _name, glm::vec2 _value);
    void SetUniform(std::string _name, glm::vec3 _value);
    ///@}

    private:
    /*
    *   ShaderID of the compiled program on the GPU
    */
    GLint m_ID;
};

#endif