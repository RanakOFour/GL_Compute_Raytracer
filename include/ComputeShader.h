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
    void SetUniform(std::string _name, glm::uvec4 _value);
    ///@}

    GLuint GetID() { return m_ID; };

    private:
    /*
    *   ShaderID of the compiled program on the GPU
    */
    GLuint m_ID;
};

/*
*   A struct used to contain information for the UI
*   The raytracer did not like this, so it is unused
*/
class ComputeInformation
{
    private:
    std::string m_name;
    ComputeShader* m_shader;
    bool m_enabled;

    public:
    ComputeInformation(std::string _name, ComputeShader* _shader)
    : m_name(_name)
    , m_shader(_shader)
    , m_enabled(true)
    {

    };

    ComputeInformation(std::string _name, std::string _shaderPath)
    : m_name(_name)
    , m_enabled(true)
    {
        m_shader = new ComputeShader(_shaderPath);
    };

    ~ComputeInformation()
    {
        delete(m_shader);
    };

    inline std::string Name() { return m_name; };
    inline void Name(std::string _n) { m_name = _n; }

    inline void Use() { m_shader->use(); };
    inline ComputeShader* Shader() { return m_shader; };

    inline bool Enabled() { return m_enabled; };
    inline void Enabled(bool _e) { m_enabled = _e; };
};


#endif