#ifndef TEXTURECOLLECTION_H
#define TEXTURECOLLECTION_H

#include "GL/glew.h"
#include "Datastructs/Texture.h"
#include <vector>

class TextureCollection
{
    private:
    unsigned int m_maxTextures;
    std::vector<Texture> m_textures;

    GLuint m_texturesBufferID;

    public:
    TextureCollection();
    ~TextureCollection();

    void AddTexture(Texture& _tex);
    void RemoveTexture(int _id);

    Texture& GetTexture(int _id);
    std::vector<Texture>& GetTextures();
};

#endif