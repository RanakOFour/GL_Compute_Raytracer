#include "TextureCollection.h"

TextureCollection::TextureCollection()
: m_maxTextures(32)
, m_textures()
, m_texturesBufferID(-1)
{

}

TextureCollection::~TextureCollection()
{
    if(m_texturesBufferID > -1)
    {
        glDeleteBuffers(1, &m_texturesBufferID);
    }
}

void TextureCollection::AddTexture(Texture& _tex)
{
    
}