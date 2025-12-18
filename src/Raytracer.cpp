#include "Raytracer.h"
#include "Framebuffer.h"

#include <chrono>

Raytracer::Raytracer(glm::ivec2 _screenSize)
: GCP_Framework(_screenSize)
, m_BVH()
, m_tris(nullptr)
, m_mats(nullptr)
, m_textures(nullptr)
, m_lights()
, m_camera(_screenSize)
, m_ObjIntersectComp("./resources/shaders/RTPipeline/Intersections/Intersections.comp")
, m_LightIntersectComp("./resources/shaders/RTPipeline/Intersections/LightDetection.comp")
, m_ShadowComp("./resources/shaders/RTPipeline/Shadows/MCStratified.comp")
, m_PBRShadeComp("./resources/shaders/RTPipeline/Shading/PBRShading.comp")
, m_gBuffers()
, m_triangleSSBO(-1)
, m_materialSSBO(-1)
, m_setup(false)
, m_shadows(true)
, m_shading(true)
, m_lightVision(true)
, m_frameCount(0)
, m_sampleCount(1)
{
    m_mainBuffer->BindGLImage();
    //Create texture buffers on GPU
    glGenTextures(GBUFFERCOUNT, &m_gBuffers[0]);

    for (int i = 0; i < GBUFFERCOUNT; i++)
    {
        glBindTexture(GL_TEXTURE_2D, m_gBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_screenSize.x, m_screenSize.y, 0, GL_RGBA, GL_FLOAT, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindImageTexture(i + 1, m_gBuffers[i], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    // Everything is bound here once as the bindings do not change
}

Raytracer::~Raytracer()
{
    glDeleteTextures(5, &m_gBuffers[0]);
    glDeleteBuffers(1, &m_triangleSSBO);
    glDeleteBuffers(1, &m_materialSSBO);

    // The pointers don't need to be deleted because their lifetimes are handled by other objects
}

void Raytracer::Trace(float _deltaTime)
{
    if (!m_setup)
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, TRIANGLE_DATA, m_triangleSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BVH_NODES, m_BVH.GetNodeSSBO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BVH_INDICES, m_BVH.GetIndexSSBO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, MATERIALS, m_materialSSBO);
        m_setup = true;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_materialSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Material) * m_mats->size(), &(m_mats->at(0)));
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glm::vec2 l_workGroups(ceil(m_screenSize.x / 8), ceil(m_screenSize.y / 4));
    m_ObjIntersectComp.use();

    m_camera.UpdateShader(m_ObjIntersectComp);

    m_ObjIntersectComp.SetUniform("u_resolution", glm::vec2(m_screenSize.x, m_screenSize.y));
    m_ObjIntersectComp.SetUniform("u_aspect", (float)m_screenSize.x / float(m_screenSize.y));

    glDispatchCompute(l_workGroups.x, l_workGroups.y, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // m_ObjIntersectComp.SetUniform("u_lastFrameCamera.position", m_camera.Position());
    // m_ObjIntersectComp.SetUniform("u_lastFrameCamera.forward", m_camera.Forward());
    // m_ObjIntersectComp.SetUniform("u_lastFrameCamera.right", m_camera.Right());
    // m_ObjIntersectComp.SetUniform("u_lastFrameCamera.up", m_camera.Up());
    // m_ObjIntersectComp.SetUniform("u_lastFrameCamera.fov", m_camera.fov());

    int l_lightCount = m_lights.size() > 10 ? 10 : m_lights.size();
    
    if (m_shadows)
    {
        m_ShadowComp.use();

        //m_camera.UpdateShader(m_ShadowComp);

        m_ShadowComp.SetUniform("u_lightCount", l_lightCount);
        m_ShadowComp.SetUniform("u_sampleCount", m_sampleCount);
        m_ShadowComp.SetUniform("u_frameCount", m_frameCount);
        m_ShadowComp.SetUniform("u_resolution", m_screenSize);

        for (int i = 0; i < l_lightCount; i++)
        {
            m_ShadowComp.SetUniform("u_lights[" + std::to_string(i) + "].position", m_lights[i].position);
            m_ShadowComp.SetUniform("u_lights[" + std::to_string(i) + "].colour", m_lights[i].colour);
            m_ShadowComp.SetUniform("u_lights[" + std::to_string(i) + "].intensity", m_lights[i].intensity);
            m_ShadowComp.SetUniform("u_lights[" + std::to_string(i) + "].radius", m_lights[i].radius);
            m_ShadowComp.SetUniform("u_lights[" + std::to_string(i) + "].cornerA", m_lights[i].cornerA);
            m_ShadowComp.SetUniform("u_lights[" + std::to_string(i) + "].cornerB", m_lights[i].cornerB);
        }

        for (int i = 0; i < m_textures->size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + BufferIndices::TEXTURES + i);
            m_PBRShadeComp.SetUniform("u_materialTextures[" + std::to_string(i) + "]", BufferIndices::TEXTURES + i);
        }


        glDispatchCompute(l_workGroups.x, l_workGroups.y, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        m_ShadowComp.SetUniform("u_lastFrameCamera.position", m_camera.Position());
        m_ShadowComp.SetUniform("u_lastFrameCamera.forward", m_camera.Forward());
        m_ShadowComp.SetUniform("u_lastFrameCamera.right", m_camera.Right());
        m_ShadowComp.SetUniform("u_lastFrameCamera.up", m_camera.Up());
        m_ShadowComp.SetUniform("u_lastFrameCamera.fov", m_camera.fov());
    }

    if (m_shading)
    {
        m_PBRShadeComp.use();

        m_PBRShadeComp.SetUniform("u_cameraPos", m_camera.Position());
        m_PBRShadeComp.SetUniform("u_lightCount", l_lightCount);
        for (int i = 0; i < l_lightCount; i++)
        {
            m_PBRShadeComp.SetUniform("u_lights[" + std::to_string(i) + "].position", m_lights[i].position);
            m_PBRShadeComp.SetUniform("u_lights[" + std::to_string(i) + "].colour", m_lights[i].colour);
            m_PBRShadeComp.SetUniform("u_lights[" + std::to_string(i) + "].intensity", m_lights[i].intensity);
            m_PBRShadeComp.SetUniform("u_lights[" + std::to_string(i) + "].radius", m_lights[i].radius);
            m_PBRShadeComp.SetUniform("u_lights[" + std::to_string(i) + "].cornerA", m_lights[i].cornerA);
            m_PBRShadeComp.SetUniform("u_lights[" + std::to_string(i) + "].cornerB", m_lights[i].cornerB);
        }

        for (int i = 0; i < m_textures->size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + BufferIndices::TEXTURES + i);
            m_PBRShadeComp.SetUniform("u_materialTextures[" + std::to_string(i) + "]", BufferIndices::TEXTURES + i);
        }

        glDispatchCompute(l_workGroups.x, l_workGroups.y, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    if (m_lightVision)
    {
        m_LightIntersectComp.use();

        m_camera.UpdateShader(m_LightIntersectComp);

        m_LightIntersectComp.SetUniform("u_resolution", glm::vec2(m_screenSize.x, m_screenSize.y));
        m_LightIntersectComp.SetUniform("u_lightCount", l_lightCount);

        for (int i = 0; i < l_lightCount; i++)
        {
            m_LightIntersectComp.SetUniform("u_lights[" + std::to_string(i) + "].position", m_lights[i].position);
            m_LightIntersectComp.SetUniform("u_lights[" + std::to_string(i) + "].colour", m_lights[i].colour);
            m_LightIntersectComp.SetUniform("u_lights[" + std::to_string(i) + "].radius", m_lights[i].radius);
            m_LightIntersectComp.SetUniform("u_lights[" + std::to_string(i) + "].cornerA", m_lights[i].cornerA);
            m_LightIntersectComp.SetUniform("u_lights[" + std::to_string(i) + "].cornerB", m_lights[i].cornerB);
        }

        glDispatchCompute(l_workGroups.x, l_workGroups.y, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    m_frameCount++;
}

void Raytracer::SetTris(std::vector<Triangle>* _tris)
{
    m_tris = _tris;
    Triangle t = m_tris->at(0);

    m_BVH.BuildBHV(_tris);

    if (m_triangleSSBO == -1)
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

    if (m_materialSSBO == -1)
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

    for (int i = 0; i < m_textures->size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + BufferIndices::TEXTURES + i);
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