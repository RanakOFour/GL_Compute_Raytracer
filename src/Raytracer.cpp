#include "Raytracer/Raytracer.h"
#include "Raytracer/BufferBindPoints.h"
#include "Shader/ShaderInfo.h"
#include "Window.h"

#include <chrono>

void Raytracer::BuildRenderDataBuffers()
{
    std::shared_ptr<Window> l_windowPtr = m_windowPtr.lock();
    glm::ivec2* l_renderSize = l_windowPtr->RenderSize();

    glBindImageTexture(0, l_windowPtr->GetScreenTexture(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    //Create texture buffers on GPU
    glGenTextures(GBUFFERCOUNT, &m_gBuffers[0]);

    for (int i = 0; i < GBUFFERCOUNT; i++)
    {
        glBindTexture(GL_TEXTURE_2D, m_gBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, l_renderSize->x, l_renderSize->y, 0, GL_RGBA, GL_FLOAT, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindImageTexture(i + 1, m_gBuffers[i], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

Raytracer::Raytracer(std::weak_ptr<Window> _windowPtr)
: m_windowPtr(_windowPtr)
, m_BVH()
, m_camera(*_windowPtr.lock()->RenderSize())
, m_gBuffers(GBUFFERCOUNT)
, m_ssbos()
, m_setup(false)
, m_frameCount(0)
{
    BuildRenderDataBuffers();

    ShaderInfoCollection::Init();
    ShaderInfoCollection::Load("resources/shaders/RTPipeline/Intersections/Intersections.comp", "Object Intersection");
    ShaderInfoCollection::Load("resources/shaders/RTPipeline/Shadows/MCStratified.comp", "Shadow Pass");
    ShaderInfoCollection::Load("resources/shaders/RTPipeline/Shading/PBRShading.comp", "PBR Pass");

    m_shaders = &ShaderInfoCollection::Get()->GetShaders();

    // Everything is bound here once as the bindings do not change
}

Raytracer::~Raytracer()
{
    glDeleteTextures(GBUFFERCOUNT, &m_gBuffers[0]);
}

void Raytracer::RebuildGBuffers()
{
    BuildRenderDataBuffers();
}

void Raytracer::Trace(float _deltaTime)
{
    // Always update GPU data first (uploads any new/changed data)
    for(int i = 0; i < m_ssbos.size(); i++)
    {
        m_ssbos[i]->UpdateGPUData();
    }

    // Bind buffers only once after they have data
    if (!m_setup)
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BVH_NODES, m_BVH.GetNodeSSBO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BVH_INDICES, m_BVH.GetIndexSSBO());
        for(int i = 0; i < m_ssbos.size(); i++)
        {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_ssbos[i]->BindLocation(), m_ssbos[i]->GetSSBOID());
        }
        m_setup = true;
    }

    // Ensure buffer updates are visible to shaders
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    auto l_time1 = std::chrono::high_resolution_clock::now();

    std::shared_ptr<Window> l_windowPtr = m_windowPtr.lock();
    glm::ivec2* l_renderSize = l_windowPtr->RenderSize();

    glm::vec2 l_workGroups(ceil(l_renderSize->x / 8), ceil(l_renderSize->y / 4));
    ShaderInfo* l_currentShader;
    bool l_cameraUpdated = false;
    bool l_lastFrameCameraUpdated = false;

    auto l_time2 = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds l_timeElapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(l_time2 - l_time1);

    printf("DT for Setup: %.3f ms\n", l_timeElapsed.count() * 0.001f);

    float l_totalTime = l_timeElapsed.count() * 0.001f;

    for(int i = 0; i < m_shaders->size(); i++)
    {
        l_time1 = std::chrono::high_resolution_clock::now();

        l_currentShader = m_shaders->at(i).get();
        if(l_currentShader->Enabled() == false)
            continue;

        printf("Executing shader: %s\n", l_currentShader->Name().c_str());

        ComputeShader* l_shader = l_currentShader->Shader();
        l_shader->use();

        // Set property values (if needed)
        std::vector<ShaderProperty>& l_properties = l_currentShader->GetProperties();
        for (int p = 0; p < l_properties.size(); p++)
        {
            if (l_properties[p].name == "u_deltaTime")
            {
                l_properties[p].value.f = _deltaTime;
                l_shader->SetUniform("u_deltaTime", l_properties[p].value.f);
            }
            else if(l_properties[p].name == "u_time")
            {
                l_properties[p].value.f = _deltaTime * m_frameCount;
                l_shader->SetUniform("u_time", l_properties[p].value.f);
            }
            else if(l_properties[p].name == "u_resolution")
            {
                l_properties[p].value.vec2 = *l_renderSize;
                l_shader->SetUniform("u_resolution", l_properties[p].value.vec2);
            }
            else if(l_properties[p].name == "u_aspect")
            {
                l_properties[p].value.f = (float)l_renderSize->x / (float)l_renderSize->y;
                l_shader->SetUniform("u_aspect", l_properties[p].value.f);
            }
            else if(l_properties[p].name == "u_frameCount")
            {
                l_properties[p].value.i = m_frameCount;
                l_shader->SetUniform("u_frameCount", l_properties[p].value.i);
            }
            else if(!l_cameraUpdated && l_properties[p].name.substr(0, 8) == "u_camera")
            {
                l_cameraUpdated = true;
                m_camera.ExportState(l_currentShader);
            }
            else if(!l_lastFrameCameraUpdated && l_properties[p].name.substr(0, 17) == "u_lastFrameCamera")
            {
                l_lastFrameCameraUpdated = true;
                m_camera.ExportLastFrameState(l_currentShader);
            }
        }

        // Set user-editable properties (non-read-only ones)
        l_currentShader->UpdateShader();

        glDispatchCompute(l_workGroups.x, l_workGroups.y, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        l_cameraUpdated = false;
        l_lastFrameCameraUpdated = false;

        l_time2 = std::chrono::high_resolution_clock::now();

		l_timeElapsed =
			std::chrono::duration_cast<std::chrono::microseconds>(l_time2 - l_time1);

		printf("DT for shader: %.3f ms\n", l_timeElapsed.count() * 0.001f);
        l_totalTime += l_timeElapsed.count() * 0.001f;
    }

    // Store current camera state for next frame's u_lastFrameCamera
    m_camera.StoreLastFrameState();

    m_frameCount++;

    printf("Total frame DT: %.3f ms\n", l_totalTime);
}

void Raytracer::AddSSBO(std::shared_ptr<IShaderStorageBuffer> _ssbo)
{
    m_ssbos.push_back(_ssbo);
}

void Raytracer::BuildBVH(std::vector<Triangle>* _tris)
{
    m_BVH.BuildBHV(_tris);
}

void Raytracer::BuildBVH(int l_ssboID)
{
    std::shared_ptr<ShaderStorageBuffer<Triangle>> l_triangleSSBO = std::dynamic_pointer_cast<ShaderStorageBuffer<Triangle>>(m_ssbos[l_ssboID]);
    std::vector<Triangle>* l_tris = l_triangleSSBO->GetData();

    m_BVH.BuildBHV(l_tris);
}