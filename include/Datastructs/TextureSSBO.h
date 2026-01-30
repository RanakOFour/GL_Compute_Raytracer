#ifndef TEXTURECOLLECTION_H
#define TEXTURECOLLECTION_H

#include "GL/glew.h"
#include "Datastructs/Texture.h"
#include "Datastructs/ShaderStorageBuffer.h"

#include <memory>
#include <vector>

class TextureSSBO : public ShaderStorageBuffer<GLuint64>
{
    private:
    std::vector<Texture> m_textures;

    public:
    TextureSSBO();
    ~TextureSSBO();

    int AddTexture(std::string _path);
    void RemoveTexture(int _id);

    Texture* GetTexture(int _id);
    std::vector<Texture>& GetTextures();
};

#endif