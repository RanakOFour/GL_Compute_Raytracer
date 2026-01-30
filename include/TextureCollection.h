#ifndef TEXTURECOLLECTION_H
#define TEXTURECOLLECTION_H

#include "GL/glew.h"
#include "Datastructs/Texture.h"
#include "ShaderStorageBuffer.h"

#include <memory>
#include <vector>

class TextureCollection : public ShaderStorageBuffer<GLuint64>
{
    private:
    std::vector<Texture> m_textures;

    public:
    TextureCollection();
    ~TextureCollection();

    int AddTexture(std::string _path);
    void RemoveTexture(int _id);

    Texture* GetTexture(int _id);
    std::vector<Texture>& GetTextures();
};

#endif