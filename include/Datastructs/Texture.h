/**
 * @file Texture.h
 * @brief Texture class for image loading and GPU texture management
 * 
 * This file provides the Texture class which handles loading images from
 * disk and managing OpenGL texture objects for use in the raytracer.
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "GL/glew.h"
#include "GLM/ext.hpp"

#include <string>
#include <vector>

/**
 * @class Texture
 * @brief Manages image data and OpenGL texture objects
 * 
 * Handles loading textures from file, storing pixel data on CPU,
 * and managing the corresponding OpenGL texture object on GPU.
 */
class Texture
{
protected:
    /** @brief Flag indicating if CPU data needs to be uploaded to GPU */
    bool m_dirty;
    
    /** @brief Raw pixel data stored on CPU (RGBA, 8 bits per channel) */
    std::vector<unsigned char> m_data;
    
    /** @brief Texture dimensions (width, height) */
    glm::ivec2 m_size;
    
    /** @brief OpenGL texture object ID */
    GLuint m_id;
    GLuint64 m_handle;

public:
    /**
     * @brief Load a texture from file
     * @param _path Filepath to the image file
     */
    Texture(const std::string& _path);
    
    /**
     * @brief Create an empty texture with specified size
     * @param _size Texture dimensions (width, height)
     */
    Texture(glm::ivec2 _size);
    
    /** @brief Destructor - releases OpenGL texture */
    ~Texture();

    /**
     * @brief Set the texture size (reallocates data)
     * @param _size New dimensions (width, height)
     */
    void Size(glm::ivec2 _size);
    
    /**
     * @brief Get the texture size
     * @return Texture dimensions (width, height)
     */
    const glm::ivec2 Size();

    /**
     * @brief Load texture data from an image file
     * @param _path Filepath to the image
     */
    virtual void Load(const std::string& _path);

    /**
     * @brief Set a pixel's color value
     * @param _position Pixel coordinates
     * @param _color RGBA color to set
     */
    void Pixel(glm::ivec2 _position, glm::vec4& _color);
    
    /**
     * @brief Get a pixel's color value
     * @param _position Pixel coordinates
     * @return RGBA color at the position
     */
    const glm::vec4 Pixel(glm::ivec2 _position);

    /**
     * @brief Get the OpenGL texture ID
     * @return Texture object ID (uploads data if dirty)
     */
    GLuint GetID();

    GLuint64 GetTexHandle();
};

#endif