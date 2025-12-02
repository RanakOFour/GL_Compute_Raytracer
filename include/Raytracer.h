#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "GCP_GFX_Framework.h"
#include "GLM/ext.hpp"

#include "Camera.h"
#include "BVH.h"
#include "Datastructs/Triangle.h"
#include "Datastructs/Material.h"
#include "Datastructs/Texture.h"
#include "Datastructs/Light.h"
#include "ComputeShader.h"

#include <vector>

class Camera;
class Raytracer : public GCP_Framework
{
    private:
    BVH m_BVH;

    std::vector<Triangle>* m_tris;
    std::vector<Material>* m_mats;
    std::vector<Texture>* m_textures;

    std::vector<Light> m_lights;

    Camera m_camera;

    ComputeShader m_IntersectionComp;
    ComputeShader m_ShadowComp;
    ComputeShader m_ShadingComp;

    GLuint m_positionB;
    GLuint m_normalsB;
    GLuint m_textureInfoB;
    GLuint m_matAlbedoB;
    GLuint m_matPropsB;
    GLuint m_shadowB;

    GLuint m_triangleSSBO;
    GLuint m_textureSSBO;
    GLuint m_materialSSBO;

    bool m_setup;

    public:
    Raytracer(glm::ivec2 _screenSize);
    ~Raytracer();

    void Trace();

    void SetTris(std::vector<Triangle>* _tris);
    void SetMaterials(std::vector<Material>* _mat);
    void SetTextures(std::vector<Texture>* _tex);
    
    void AddLight(Light _light);
    Light* GetLight(int index);

    Camera* GetCamera(); 
};

#endif