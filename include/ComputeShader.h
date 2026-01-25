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
    private:
    /*
    *   ShaderID of the compiled program on the GPU
    */
    GLuint m_ID;
    
    public:
    /*
    *   Loads a new Compute shader from the given filepath.
    *   Program does not crash if a wrong path is given.
    */
    ComputeShader(std::string _path);
    ~ComputeShader();

    // Move semantics (required because destructor deletes GL program)
    ComputeShader(ComputeShader&& other) noexcept;
    ComputeShader& operator=(ComputeShader&& other) noexcept;

    // Delete copy operations to prevent accidental program deletion
    ComputeShader(const ComputeShader&) = delete;
    ComputeShader& operator=(const ComputeShader&) = delete;

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
    void SetUniform(std::string _name, glm::uvec4 _value);
    ///@}

    GLuint GetID() { return m_ID; };
};

#endif