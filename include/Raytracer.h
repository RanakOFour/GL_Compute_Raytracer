#ifndef _RAYTRACER_H_
#define _RAYTRACER_H_

#include "BufferManager.h"
#include "ComputeShader.h"

#include <vector>
#include <memory>

class Scene;
class Raytracer
{
private:
    BufferManager m_bufferManager;
    std::vector<ComputeShader> m_shaders;
    std::shared_ptr<Scene> m_currentScene;

public:
    Raytracer();
    ~Raytracer();

    void Trace();

    void SetScene(std::shared_ptr<Scene> _scene);
    std::shared_ptr<Scene> GetScene();

    void AddShader(ComputeShader _shader);

    BufferManager& GetBufferManager() { return m_bufferManager; };
}

#endif
