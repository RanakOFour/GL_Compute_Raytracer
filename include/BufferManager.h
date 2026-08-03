#ifndef _BUFFERMANAGER_H_
#define _BUFFERMANAGER_H_

#include "TextureBuffer.h"
#include <memory>
#include <vector>
#include <map>

class BufferManager
{
private:
    std::vector<std::shared_ptr<TextureBuffer>> m_buffers;
    std::map<int, std::shared_ptr<TextureBuffer> m_mappingToBuffer;

public:
    BufferManager();
    ~BufferManager();

    std::weak_ptr<TextureBuffer> CreateBuffer(int _type);
    std::weak_ptr<TextureBuffer> GetBufferAt(int _location);

    void MapBuffer(int _mapLoc, GLuint _bufferId);
    void UnmapBuffer(GLuint _bufferId);
};

#endif
