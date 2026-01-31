#include "Datastructs/TextureSSBO.h"

TextureSSBO::TextureSSBO()
: ShaderStorageBuffer()
, m_textures()
{
    
}

TextureSSBO::~TextureSSBO()
{
    
}

int TextureSSBO::AddTexture(std::string _path)
{
    m_textures.push_back(Texture(_path));
    AddData(m_textures.back().GetTexHandle());
    printf("Added texture %u to TextureCollection\n", m_textures.back().GetID());

    return m_textures.size() - 1;
}

void TextureSSBO::RemoveTexture(int _id)
{
    m_textures.erase(m_textures.begin() + _id);
    RemoveData(_id);
}

Texture* TextureSSBO::GetTexture(int _id)
{
    return &m_textures[_id];
}

std::vector<Texture>& TextureSSBO::GetTextures()
{
    return m_textures;
}