#include "Raytracer.h"
#include "ShaderInfo.h"

#include <chrono>

void Raytracer::BuildRenderDataBuffers()
{
    glBindImageTexture(0, m_mainTextureLoc, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    //Create texture buffers on GPU
    glGenTextures(GBUFFERCOUNT, &m_gBuffers[0]);

    for (int i = 0; i < GBUFFERCOUNT; i++)
    {
        glBindTexture(GL_TEXTURE_2D, m_gBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_renderSize.x, m_renderSize.y, 0, GL_RGBA, GL_FLOAT, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindImageTexture(i + 1, m_gBuffers[i], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

Raytracer::Raytracer(glm::ivec2 _renderSize, GLuint _mainTextureLoc)
: m_mainTextureLoc(_mainTextureLoc)
, m_renderSize(_renderSize)
, m_BVH()
, m_tris(nullptr)
, m_mats(nullptr)
, m_textures(nullptr)
, m_lights()
, m_camera(_renderSize)
, m_gBuffers(GBUFFERCOUNT)
, m_triangleSSBO(-1)
, m_materialSSBO(-1)
, m_setup(false)
, m_frameCount(0)
{
    BuildRenderDataBuffers();

    ComputeShader* l_objIntersectComp = new ComputeShader("resources/shaders/RTPipeline/Intersections/Intersections.comp");
    ComputeShader* l_shadowComp = new ComputeShader("resources/shaders/RTPipeline/Shadows/MonteCarloShadow.comp");
    ComputeShader* l_pbrShadeComp = new ComputeShader("resources/shaders/RTPipeline/Shading/PBRShading.comp");
    ComputeShader* l_lightIntersectComp = new ComputeShader("resources/shaders/RTPipeline/Intersections/LightDetection.comp");

    ShaderInfoCollection::Init();
    ShaderInfoCollection::LogShader(l_objIntersectComp, "Object Intersection");
    ShaderInfoCollection::LogShader(l_shadowComp, "Shadows");
    ShaderInfoCollection::LogShader(l_pbrShadeComp, "PBR Shading");
    ShaderInfoCollection::LogShader(l_lightIntersectComp, "Light Detection");

    m_shaders = &ShaderInfoCollection::Init()->GetShaders();

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

    glm::vec2 l_workGroups(ceil(m_renderSize.x / 8), ceil(m_renderSize.y / 4));
    ShaderInfo* l_currentShader;

    for(int i = 0; i < m_shaders->size(); i++)
    {
        l_currentShader = &m_shaders->at(i);
        if(l_currentShader->Enabled() == false)
            continue;

        printf("Executing shader: %s\n", l_currentShader->Name().c_str());
        
        ComputeShader* shader = l_currentShader->Shader();
        shader->use();

        // Set camera uniforms (current and last frame)
        m_camera.UpdateShader(*shader);

        // Set resolution and aspect ratio
        shader->SetUniform("u_resolution", glm::vec2(m_renderSize));
        shader->SetUniform("u_aspect", (float)m_renderSize.x / (float)m_renderSize.y);

        // Set frame count
        shader->SetUniform("u_frameCount", m_frameCount);

        // Set light uniforms
        shader->SetUniform("u_lightCount", (int)m_lights.size());
        for (int j = 0; j < m_lights.size() && j < 10; j++)
        {
            std::string prefix = "u_lights[" + std::to_string(j) + "].";
            shader->SetUniform(prefix + "position", m_lights[j].position);
            shader->SetUniform(prefix + "colour", m_lights[j].color);
            shader->SetUniform(prefix + "intensity", m_lights[j].intensity);
            shader->SetUniform(prefix + "radius", m_lights[j].radius);
            shader->SetUniform(prefix + "cornerA", m_lights[j].cornerA);
            shader->SetUniform(prefix + "cornerB", m_lights[j].cornerB);
        }

        // Set camera position (for shaders that use u_cameraPos instead of u_camera.position)
        shader->SetUniform("u_cameraPos", m_camera.Position());

        // Set user-editable properties (non-read-only ones)
        l_currentShader->UpdateShader();

        glDispatchCompute(l_workGroups.x, l_workGroups.y, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    // Store current camera state for next frame's u_lastFrameCamera
    m_camera.StoreLastFrameState();

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