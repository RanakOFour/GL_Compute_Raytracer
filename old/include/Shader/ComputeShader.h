/**
 * @file ComputeShader.h
 * @brief OpenGL Compute Shader wrapper class
 * 
 * This file provides the ComputeShader class which wraps OpenGL compute
 * shader compilation, linking, and uniform management functionality.
 */

#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include "GL/glew.h"
#include "GLM/ext.hpp"

#include <string>

/**
 * @class ComputeShader
 * @brief Wrapper class for OpenGL Compute shaders
 * 
 * Handles loading, compiling, and managing compute shaders including
 * setting uniform values of various types.
 */
class ComputeShader
{
private:
    /** @brief OpenGL program ID of the compiled shader */
    GLuint m_ID;
    
public:
    /**
     * @brief Load and compile a compute shader from file
     * @param _path Filepath to the compute shader source file
     * 
     * Program does not crash if an invalid path is given; instead,
     * m_ID will be set to 0 indicating shader creation failed.
     */
    ComputeShader(std::string _path);
    
    /** @brief Destructor - releases OpenGL shader program */
    ~ComputeShader();

    /**
     * @brief Move constructor
     * @param other ComputeShader to move from
     * 
     * Required because the destructor deletes the GL program.
     */
    ComputeShader(ComputeShader&& other) noexcept;
    
    /**
     * @brief Move assignment operator
     * @param other ComputeShader to move from
     * @return Reference to this shader
     */
    ComputeShader& operator=(ComputeShader&& other) noexcept;

    /** @brief Deleted copy constructor to prevent accidental program deletion */
    ComputeShader(const ComputeShader&) = delete;
    
    /** @brief Deleted copy assignment to prevent accidental program deletion */
    ComputeShader& operator=(const ComputeShader&) = delete;

    /**
     * @brief Activate this shader for use in subsequent OpenGL operations
     */
    void use();

    /**
     * @name Uniform Setters
     * @brief Methods to set shader uniform values
     * @{
     */
    
    /**
     * @brief Set a boolean uniform
     * @param _name Uniform name in the shader
     * @param _value Boolean value to set
     */
    void SetUniform(std::string _name, bool _value);
    
    /**
     * @brief Set an integer uniform
     * @param _name Uniform name in the shader
     * @param _value Integer value to set
     */
    void SetUniform(std::string _name, int _value);
    
    /**
     * @brief Set a float uniform
     * @param _name Uniform name in the shader
     * @param _value Float value to set
     */
    void SetUniform(std::string _name, float _value);
    
    /**
     * @brief Set a vec2 uniform
     * @param _name Uniform name in the shader
     * @param _value 2D vector value to set
     */
    void SetUniform(std::string _name, glm::vec2 _value);
    
    /**
     * @brief Set a vec3 uniform
     * @param _name Uniform name in the shader
     * @param _value 3D vector value to set
     */
    void SetUniform(std::string _name, glm::vec3 _value);
    
    /**
     * @brief Set a vec4 uniform
     * @param _name Uniform name in the shader
     * @param _value 4D vector value to set
     */
    void SetUniform(std::string _name, glm::vec4 _value);
    
    /**
     * @brief Set a uvec4 uniform
     * @param _name Uniform name in the shader
     * @param _value Unsigned 4D vector value to set
     */
    void SetUniform(std::string _name, glm::uvec4 _value);
    /** @} */

    /**
     * @brief Get the OpenGL program ID
     * @return The shader program ID (0 if shader creation failed)
     */
    GLuint GetID() { return m_ID; };
};

#endif
