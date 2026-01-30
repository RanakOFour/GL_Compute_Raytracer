#include "TextureCollection.h"

TextureCollection::TextureCollection()
: ShaderStorageBuffer()
, m_textures()
{
    
}

TextureCollection::~TextureCollection()
{
    
}

int TextureCollection::AddTexture(std::string _path)
{
    m_textures.push_back(Texture(_path));
    AddData(m_textures.back().GetTexHandle());
    printf("Added texture %u to TextureCollection\n", m_textures.back().GetID());

    return m_textures.size();
}

void TextureCollection::RemoveTexture(int _id)
{
    m_textures.erase(m_textures.begin() + _id);
    RemoveData(_id);
}

Texture* TextureCollection::GetTexture(int _id)
{
    return &m_textures[_id];
}

std::vector<Texture>& TextureCollection::GetTextures()
{
    return m_textures;
}