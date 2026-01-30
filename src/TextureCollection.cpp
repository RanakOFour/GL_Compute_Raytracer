#include "TextureCollection.h"

TextureCollection::TextureCollection()
: ShaderStorageBuffer()
, m_textures()
{
    
}

TextureCollection::~TextureCollection()
{
    
}

void TextureCollection::UpdateGPUData()
{
    if (m_data.empty())
            return;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboID);

    // size_t requiredSize = sizeof(GLuint64) * m_data.size();

    // // If buffer needs to grow or was never allocated, reallocate entire buffer
    // if (requiredSize > m_allocatedSize)
    // {
    //     glBufferData(GL_SHADER_STORAGE_BUFFER, requiredSize, m_data.data(), GL_DYNAMIC_STORAGE_BIT);
    //     m_allocatedSize = requiredSize;
        
    //     // Mark all as clean since we just uploaded everything
    //     for (size_t i = 0; i < m_dirtyFlags.size(); i++)
    //     {
    //         m_dirtyFlags[i] = false;
    //     }
    // }
    // else
    // {
    //     // Only update dirty elements
    //     for (size_t i = 0; i < m_data.size(); i++)
    //     {
    //         if (m_dirtyFlags[i])
    //         {
    //             glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint64) * i, sizeof(GLuint64), &(m_data[i]));
    //             m_dirtyFlags[i] = false;
    //         }
    //     }
    // }

    glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint64) * m_data.size(), m_data.data(), GL_DYNAMIC_STORAGE_BIT);

    for(int i = 0; i < m_data.size(); i++)
    {
        glMakeTextureHandleResidentARB(m_data[i]);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void TextureCollection::AddTexture(Texture& _tex)
{
    m_textures.push_back(_tex);
    AddData(_tex.GetTexHandle());
}

void TextureCollection::RemoveTexture(int _id)
{
    m_textures.erase(m_textures.begin() + _id);
    RemoveData(_id);
}

Texture& TextureCollection::GetTexture(int _id)
{
    return m_textures[_id];
}

std::vector<Texture>& TextureCollection::GetTextures()
{
    return m_textures;
}