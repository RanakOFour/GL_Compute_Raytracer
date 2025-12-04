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
, m_gBuffers{0, 0, 0, 0, 0, 0}
, m_triangleSSBO(-1)
, m_textureSSBO(-1)
, m_materialSSBO(-1)
, m_setup(false)
, m_shadows(true)
, m_shading(true)
{
    printf("Binding bufferTex\n");
    m_mainBuffer->BindGLImage();

    printf("Creating texture buffers\n");
    //Create texture buffers on GPU
    glGenTextures(4, &m_gBuffers[0]);

    for(int i = 0; i < 3; i++)
    {       
        printf("Filling in buffer %i\n", i);
        glBindTexture(GL_TEXTURE_2D, m_gBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_screenSize.x, m_screenSize.y, 0, GL_RGBA, GL_FLOAT, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindImageTexture(i + 1, m_gBuffers[i], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    }


    printf("Shadow buffer\n");
    glBindTexture(GL_TEXTURE_2D, m_gBuffers[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_screenSize.x, m_screenSize.y, 0, GL_RED, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindImageTexture(4, m_gBuffers[3], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    

    // Everything is bound here once as the bindings do not change
}

Raytracer::~Raytracer()
{
    glDeleteTextures(6, &m_gBuffers[0]);
    glDeleteBuffers(1, &m_triangleSSBO);
    glDeleteBuffers(1, &m_materialSSBO);
}

void Raytracer::Trace()
{
    if(!m_setup)
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_triangleSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, m_BVH.GetNodeSSBO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, m_BVH.GetIndexSSBO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, m_materialSSBO);
        m_setup = true;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_materialSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Material) * m_mats->size(), &(m_mats->at(0)));
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glm::vec2 l_workGroups(ceil(m_screenSize.x / 8), ceil(m_screenSize.y / 4));

    m_IntersectionComp.use();
    m_camera.UpdateShader(m_IntersectionComp);
    m_IntersectionComp.SetUniform("u_resolution", glm::vec2(m_screenSize.x, m_screenSize.y));

    // Cap light count at 10
    int l_lightCount = m_lights.size() > 10 ? 10 : m_lights.size();
    m_IntersectionComp.SetUniform("u_numLights", l_lightCount);
    for(int i = 0; i < l_lightCount; i++)
    {
        m_IntersectionComp.SetUniform("u_lights[" + std::to_string(i) + "].position", m_lights[i].position);
	    m_IntersectionComp.SetUniform("u_lights[" + std::to_string(i) + "].color", m_lights[i].colour);
    }

    
	glDispatchCompute(l_workGroups.x, l_workGroups.y, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

    if(m_shadows)
    {
        m_ShadowComp.use();

        m_camera.UpdateShader(m_ShadowComp);

        // Cap light count at 10
        int l_lightCount = m_lights.size() > 10 ? 10 : m_lights.size();
        m_ShadowComp.SetUniform("u_numLights", l_lightCount);
        for(int i = 0; i < l_lightCount; i++)
        {
            m_ShadowComp.SetUniform("u_lights[" + std::to_string(i) + "].position", m_lights[i].position);
            m_ShadowComp.SetUniform("u_lights[" + std::to_string(i) + "].color", m_lights[i].colour);
        }

	    glDispatchCompute(l_workGroups.x, l_workGroups.y, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
    }

    if(m_shading)
    {
        m_ShadingComp.use();
        m_camera.UpdateShader(m_ShadingComp);
        m_ShadingComp.SetUniform("u_numLights", l_lightCount);

        for(int i = 0; i < l_lightCount; i++)
        {
            m_ShadingComp.SetUniform("u_lights[" + std::to_string(i) + "].position", m_lights[i].position);
            m_ShadingComp.SetUniform("u_lights[" + std::to_string(i) + "].color", m_lights[i].colour);
        }

        for(int i = 0; i < m_textures->size(); i++)
        {
            m_ShadingComp.SetUniform("u_materialTextures[" + std::to_string(i) + "]", 9 + i);
        }

	    glDispatchCompute(l_workGroups.x, l_workGroups.y, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
    }
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
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Material) * m_mats->size(), &(m_mats->at(0)), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, m_materialSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Raytracer::SetTextures(std::vector<Texture>* _tex)
{
    m_textures = _tex;
    
    for(int i = 0; i < m_textures->size(); i++)
    {
        glActiveTexture(GL_TEXTURE9 + i);
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

Material* Raytracer::GetMaterial(int _index)
{
    return &(m_mats->at(_index));
}

Camera* Raytracer::GetCamera()
{
    return &m_camera;
}