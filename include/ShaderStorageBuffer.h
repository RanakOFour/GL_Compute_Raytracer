#ifndef SHADERSTORAGE_H
#define SHADERSTORAGE_H

#include "GL/glew.h"

#include <vector>

class IShaderStorageBuffer
{
    public:
    virtual ~IShaderStorageBuffer() = default;
    virtual void UpdateGPUData() = 0;
    virtual void SetDirty(int index) = 0;
    virtual GLuint GetSSBOID() const = 0;
    virtual int BindLocation() const = 0;
};

template<typename T>
class ShaderStorageBuffer : public IShaderStorageBuffer
{
    private:
    int m_bindLocation;
    GLuint m_ssboID;
    std::vector<bool> m_dirtyFlags;
    std::vector<T> m_data;
    size_t m_allocatedSize;

    public:
    ShaderStorageBuffer()
    : m_ssboID(0)
    , m_bindLocation(-1)
    , m_data()
    , m_dirtyFlags()
    , m_allocatedSize(0)
    {
        glGenBuffers(1, &m_ssboID);
    };

    ShaderStorageBuffer(GLuint _ssboID, int _bindLocation)
    : m_ssboID(_ssboID)
    , m_bindLocation(_bindLocation)
    , m_data()
    , m_dirtyFlags()
    , m_allocatedSize(0)
    {

    };

    ~ShaderStorageBuffer()
    {
        if (m_ssboID != 0)
        {
            glDeleteBuffers(1, &m_ssboID);
        }
    };

    void AddData(const T& data)
    {
        m_data.push_back(data);
        m_dirtyFlags.push_back(true);
    };

    void RemoveData(int index)
    {
        if (index >= 0 && index < m_data.size())
        {
            m_data.erase(m_data.begin() + index);
            m_dirtyFlags.erase(m_dirtyFlags.begin() + index);
        }
    };

    void UpdateGPUData()
    {
        if (m_data.empty())
            return;

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboID);

        size_t requiredSize = sizeof(T) * m_data.size();

        // If buffer needs to grow or was never allocated, reallocate entire buffer
        if (requiredSize > m_allocatedSize)
        {
            glBufferData(GL_SHADER_STORAGE_BUFFER, requiredSize, m_data.data(), GL_DYNAMIC_DRAW);
            m_allocatedSize = requiredSize;
            
            // Mark all as clean since we just uploaded everything
            for (size_t i = 0; i < m_dirtyFlags.size(); i++)
            {
                m_dirtyFlags[i] = false;
            }
        }
        else
        {
            // Only update dirty elements
            for (size_t i = 0; i < m_data.size(); i++)
            {
                if (m_dirtyFlags[i])
                {
                    glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(T) * i, sizeof(T), &(m_data[i]));
                    m_dirtyFlags[i] = false;
                }
            }
        }

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    };

    void SetDirty(int index)
    {
        if (index >= 0 && index < m_dirtyFlags.size())
        {
            m_dirtyFlags[index] = true;
        }
    }

    void BindLocation(int bindLocation)
    {
        m_bindLocation = bindLocation;
    }

    int BindLocation() const { return m_bindLocation; }

    GLuint GetSSBOID() const { return m_ssboID; }

    std::vector<T>* GetData() { return &m_data; }
};

#endif