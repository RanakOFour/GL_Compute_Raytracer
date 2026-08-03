#ifndef _TEXTURE_BUFFER_H_
#define _TEXTURE_BUFFER_H_

#include "GL/glew.h"
#include "GLM/ext.hpp"

class TextureBuffer
{
private:
    GLuint m_id;

    /// The dimensions of the texture
    glm::vec2 m_size;

    /// The OpenGL code for the type of colour data stored in the buffer e.g. GL_RGBA32F, GL_RG8
    int m_bufferType;

public:
    TextureBuffer(int _bufferType);
    ~TextureBuffer();

    template<typename T>
    void Set(T _data) requires typeid(T) == typeid(float) 
                               || typeid(T) == typeid(glm::vec2) 
                               || typeid(T) == typeid(glm::vec3) 
                               || typeid(T) == typeid(glm::vec4)

    GLuint GetID();
};

#endif
