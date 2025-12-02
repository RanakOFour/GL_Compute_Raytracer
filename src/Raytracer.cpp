#include "Raytracer.h"
#include "Framebuffer.h"

Raytracer::Raytracer(glm::ivec2 _screenSize)
: GCP_Framework(_screenSize)
, m_BVH()
, m_tris(nullptr)
, m_mats(nullptr)
, m_textures(nullptr)
, m_lights()
, m_camera(_screenSize)
, m_IntersectionComp("./resources/shaders/RTPipeline/RTIntersections.comp")
, m_ShadowComp("./resources/shaders/RTPipeline/RTShadows.comp")
, m_ShadingComp("./resources/shaders/RTPipeline/RTShading.comp")
, m_positionB(-1)
, m_normalsB(-1)
, m_textureInfoB(-1)
, m_matAlbedoB(-1)
, m_matPropsB(-1)
, m_shadowB(-1)
, m_triangleSSBO(-1)
, m_textureSSBO(-1)
, m_materialSSBO(-1)
, m_setup(false)
{
    int* _maxImages;
    glGetIntegerv(GL_MAX_COMPUTE_IMAGE_UNIFORMS, _maxImages);

    printf("Max images: %i\n", *_maxImages);
    printf("Binding bufferTex\n");
    m_mainBuffer->BindGLImage();

    printf("Creating texture buffers\n");
    //Create texture buffers on GPU
    glGenTextures(6, &m_positionB);

    for(int i = 0; i < 5; i++)
    {       
        printf("Filling in buffer %i\n", i);
        glBindTexture(GL_TEXTURE_2D, *((&m_positionB) + i));
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_screenSize.x, m_screenSize.y, 0, GL_RGBA, GL_FLOAT, 0);
        glBindImageTexture(i + 1, *((&m_positionB) + i), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    }


    printf("Shadow buffer\n");
    glBindTexture(GL_TEXTURE_2D, m_shadowB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_screenSize.x, m_screenSize.y, 0, GL_RGBA, GL_FLOAT, 0);
    glBindImageTexture(9, m_shadowB, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    

    // Everything is bound here once as the bindings do not change
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_triangleSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, m_BVH.GetNodeSSBO());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, m_BVH.GetIndexSSBO());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, m_materialSSBO);

    m_ShadowComp.use();
    m_ShadowComp.SetUniform("u_resolution", glm::vec2(m_screenSize.x, m_screenSize.y));
    glUseProgram(0);
}

Raytracer::~Raytracer()
{
    glDeleteTextures(6, &m_positionB);
    glDeleteBuffers(2, &m_triangleSSBO);
}

void Raytracer::Trace()
{
    m_IntersectionComp.use();
    m_camera.UpdateShader(m_IntersectionComp);

    // We rebind the 0 buffer as it gets reassigned in the shading comp
    glBindImageTexture(0, m_positionB, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    // Cap light count at 10
    int lightCount = m_lights.size() > 10 ? 10 : m_lights.size();
    for(int i = 0; i < lightCount; i++)
    {
        m_IntersectionComp.SetUniform("u_lights[" + std::to_string(i) + "].position", m_lights[i].position);
	    m_IntersectionComp.SetUniform("u_lights[" + std::to_string(i) + "].color", m_lights[i].colour);
    }

    
	glDispatchCompute(m_screenSize.x / 4, m_screenSize.y / 4, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

    m_ShadowComp.use();
    m_ShadowComp.SetUniform("u_cameraPosition", m_camera.Position());

    glDispatchCompute(m_screenSize.x / 4, m_screenSize.y / 4, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_ShadingComp.use();

    m_ShadingComp.SetUniform("u_cameraPosition", m_camera.Position());
    for(int i = 0; i < lightCount; i++)
    {
        m_ShadingComp.SetUniform("u_lights[" + std::to_string(i) + "].position", m_lights[i].position);
	    m_ShadingComp.SetUniform("u_lights[" + std::to_string(i) + "].color", m_lights[i].colour);
    }

    glDispatchCompute(m_screenSize.x / 4, m_screenSize.y / 4, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Raytracer::SetTris(std::vector<Triangle>* _tris)
{
    m_tris = _tris;
    m_BVH.BuildBHV(_tris);

    if(!m_triangleSSBO)
    {
        glGenBuffers(1, &m_triangleSSBO);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_triangleSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Triangle) * m_tris->size(), &(m_tris->at(0)), GL_DYNAMIC_READ);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Raytracer::SetMaterials(std::vector<Material>* _mat)
{
    m_mats = _mat;

    if(!m_materialSSBO)
    {
        glGenBuffers(1, &m_materialSSBO);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_materialSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Material) * m_mats->size(), &(m_mats->at(0)), GL_DYNAMIC_READ);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Raytracer::SetTextures(std::vector<Texture>* _tex)
{
    m_textures = _tex;
    
    for(int i = 0; i < m_textures->size(); i++)
    {
        glActiveTexture(GL_TEXTURE10 + i);
        glBindTexture(GL_TEXTURE_2D, m_textures->at(i).GetID());
    }
}

void Raytracer::AddLight(Light _light)
{
    m_lights.push_back(_light);
}

Light* Raytracer::GetLight(int _index)
{
    return &(m_lights[_index]);
}

Camera* Raytracer::GetCamera()
{
    return &m_camera;
}