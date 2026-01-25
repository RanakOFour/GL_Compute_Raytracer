#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "GLM/ext.hpp"

#include "Camera.h"
#include "BVH.h"
#include "Datastructs/Triangle.h"
#include "Datastructs/Material.h"
#include "Datastructs/Texture.h"
#include "Datastructs/Light.h"
#include "ComputeShader.h"
#include "BufferIndexes.h"

#include <vector>

class GUI;
class Camera;
class Raytracer
{
    friend GUI;
    private:
    BVH m_BVH;

    // I want to (in the future) be able to hook up the Raytracer to something else
    // so just plugging in the vectors with pointers made the most sense
    std::vector<Triangle>* m_tris;
    std::vector<Material>* m_mats;
    std::vector<Texture>* m_textures;

    std::vector<Light> m_lights;

    Camera m_camera;

    ComputeShader m_ObjIntersectComp;
    ComputeShader m_LightIntersectComp;
    ComputeShader m_ShadowComp;
    ComputeShader m_PBRShadeComp;

    GLuint m_gBuffers[GBUFFERCOUNT];

    GLuint m_triangleSSBO;
    GLuint m_materialSSBO;

    GLuint m_mainTextureLoc;

    glm::ivec2 m_renderSize;

    bool m_setup;

    bool m_lightVision;
    bool m_shadows;
    bool m_shading;

    int m_frameCount;
    int m_sampleCount;

    public:
    Raytracer(glm::ivec2 _screenSize, GLuint _mainTextureLoc);
    ~Raytracer();

    void Trace(float _time);

    void SetTris(std::vector<Triangle>* _tris);
    void SetMaterials(std::vector<Material>* _mat);
    void SetTextures(std::vector<Texture>* _tex);
    
    void AddLight(Light _light);
    Light* GetLight(int index);

    Material* GetMaterial(int _index);

    Camera* GetCamera();

    bool LightVision() { return m_lightVision; };
    void LightVision(bool _l) { m_lightVision = _l; };

    bool Shading() { return m_shading;};
    void Shading(bool _s) { m_shading = _s;};

    bool Shadows() { return m_shadows; };
    void Shadows(bool _s) { m_shadows = _s; };

    int Samples() { return m_sampleCount; };
    void Samples(int _s) { m_sampleCount = _s; };
};

#endif