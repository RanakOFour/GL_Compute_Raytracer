/**
 * @file GUI.cpp
 * @brief Implementation of the GUI class
 */

#include "GUI.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include <memory>

GUI::GUI(std::weak_ptr<Raytracer> _rt, 
         std::weak_ptr<Window> _window,
         std::weak_ptr<ShaderStorageBuffer<Light>> _lightSSBO,
         std::weak_ptr<ShaderStorageBuffer<Material>> _materialSSBO) 
: m_rt(_rt)
, m_window(_window)
, m_lightSSBO(_lightSSBO)
, m_materialSSBO(_materialSSBO)
, m_selectedLight(0)
, m_selectedMaterial(0)
, m_selectedShaderForProps(0)
, m_newPrefixBuffer{0}
, m_newShaderPathBuffer{0}
, m_newShaderNameBuffer{0}
{
}

GUI::~GUI()
{
}

void GUI::ShowUI(float _deltaTime)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if(ImGui::Begin("Settings"))
    {
        if(ImGui::BeginTabBar("##categories"))
        {
            ShowCameraInfo();
            ShowSceneSettings();
            ShowShaderManagement();
            ShowShaderPropertySettings();
        }

        ImGui::EndTabBar();
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::ShowCameraInfo()
{
    if (!ImGui::BeginTabItem("Camera Info"))
        return;

    std::shared_ptr<Window> l_windowPtr = m_window.lock();
    glm::ivec2* renderSize = l_windowPtr->RenderSize();

    if(ImGui::InputInt2("Render Resolution", &renderSize->x))
    {
        if(renderSize->x < 1) renderSize->x = 1;
        if(renderSize->y < 1) renderSize->y = 1;
        l_windowPtr->ChangeRenderSize();
        
        std::shared_ptr<Raytracer> l_rtPtr = m_rt.lock();
        l_rtPtr->BuildRenderDataBuffers();
    }

    Camera& cam = m_rt.lock()->GetCamera();
    glm::vec3 pos = cam.Position();
    glm::vec3 fwd = cam.Forward();
    glm::vec3 right = cam.Right();
    glm::vec3 up = cam.Up();
    float fov = cam.fov();

    ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
    ImGui::Text("Orientation: (%.3f, %.3f, %.3f, %.3f)", cam.Rotation().x, cam.Rotation().y, cam.Rotation().z, cam.Rotation().w);
    ImGui::Text("Delta Time: %.3f ms @ %.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::EndTabItem();
}

void GUI::ShowSceneSettings()
{
    if (!ImGui::BeginTabItem("Objects"))
        return;

    std::shared_ptr<ShaderStorageBuffer<Light>> l_lightSSBOPtr = m_lightSSBO.lock();
    std::shared_ptr<ShaderStorageBuffer<Material>> l_materialSSBOPtr = m_materialSSBO.lock();

    if (ImGui::BeginTabBar("SceneSettingsTabBar"))
    {
        // Lights Tab
        if (ImGui::BeginTabItem("Lights"))
        {
            if (!l_lightSSBOPtr)
            {
                ImGui::Text("Light SSBO not available.");
                ImGui::EndTabItem();
            }
            else
            {
            std::vector<Light>* lights = l_lightSSBOPtr->GetData();
            int lightCount = static_cast<int>(lights->size());

            if (lightCount > 0)
            {
                if (m_selectedLight >= lightCount)
                    m_selectedLight = lightCount - 1;

                // Light selector
                std::vector<const char*> l_lightNames;
                for (int i = 0; i < lightCount; ++i)
                {
                    std::string name = "Light " + std::to_string(i);
                    l_lightNames.push_back(name.c_str());
                }

                ImGui::Combo("Select Light", &m_selectedLight, l_lightNames.data(), lightCount);
                ImGui::Separator();

                Light& light = (*lights)[m_selectedLight];
                bool lightChanged = false;
                lightChanged |= ImGui::DragFloat3("Position", &light.position.x, 0.1f);
                lightChanged |= ImGui::ColorEdit3("Color", &light.colour.x);
                lightChanged |= ImGui::DragFloat("Intensity", &light.intensity, 0.01f, 0.0f, 100.0f);
                lightChanged |= ImGui::DragFloat("Radius", &light.radius, 0.01f, 0.0f, 10.0f);
                
                if (lightChanged)
                {
                    l_lightSSBOPtr->SetDirty(m_selectedLight);
                }
            }
            else
            {
                ImGui::Text("No lights in the scene.");
            }

            ImGui::EndTabItem();
            }
        }

        // Materials Tab
        if (ImGui::BeginTabItem("Materials"))
        {
            if (!l_materialSSBOPtr)
            {
                ImGui::Text("Material SSBO not available.");
                ImGui::EndTabItem();
            }
            else
            {
            std::vector<Material>* materials = l_materialSSBOPtr->GetData();
            int materialCount = static_cast<int>(materials->size());

            if (materialCount > 0)
            {
                if (m_selectedMaterial >= materialCount)
                    m_selectedMaterial = materialCount - 1;

                // Material selector
                std::vector<const char*> materialNames;
                for (int i = 0; i < materialCount; ++i)
                {
                    std::string name = "Material " + std::to_string(i);
                    materialNames.push_back(name.c_str());
                }

                ImGui::Combo("Select Material", &m_selectedMaterial, materialNames.data(), materialCount);
                ImGui::Separator();

                Material& mat = (*materials)[m_selectedMaterial];
                bool materialChanged = false;

                materialChanged |= ImGui::ColorEdit3("Albedo", &mat.albedo.x);
                materialChanged |= ImGui::DragFloat("Roughness", &mat.roughness, 0.01f, 0.0f, 1.0f);
                materialChanged |= ImGui::DragFloat("Metallic", &mat.metallic, 0.01f, 0.0f, 1.0f);
                materialChanged |= ImGui::DragFloat("Ambient Occlusion", &mat.ambientOcclusion, 0.01f, 0.0f, 1.0f);

                if (materialChanged)
                {
                    l_materialSSBOPtr->SetDirty(m_selectedMaterial);
                }
            }
            else
            {
                ImGui::Text("No materials in the scene.");
            }

            ImGui::EndTabItem();
            }
        }

        ImGui::EndTabBar();
    }

    ImGui::EndTabItem();
}

void GUI::ShowShaderPropertySettings()
{
    if (!ImGui::BeginTabItem("Properties"))
        return;

    if (ImGui::BeginTabBar("VisibilityTabBar"))
    {
        // Hidden Prefixes Tab
        if (ImGui::BeginTabItem("Hidden Prefixes"))
        {
            auto hiddenMgr = HiddenUniformManager::Get();
            const auto& prefixes = hiddenMgr->GetPrefixes();

            ImGui::Text("Uniforms matching these prefixes are hidden from GUI:");
            ImGui::Separator();

            // List current prefixes with remove buttons
            std::string prefixToRemove;
            for (const auto& prefix : prefixes)
            {
                ImGui::PushID(prefix.c_str());
                if (ImGui::Button("X"))
                {
                    prefixToRemove = prefix;
                }
                ImGui::SameLine();
                ImGui::Text("%s", prefix.c_str());
                ImGui::PopID();
            }

            if (!prefixToRemove.empty())
            {
                hiddenMgr->RemovePrefix(prefixToRemove);
                ShaderInfoCollection::RefreshAllVisibilityStatic();
            }

            ImGui::Separator();

            // Add new prefix
            ImGui::InputText("New Prefix", m_newPrefixBuffer, sizeof(m_newPrefixBuffer));
            ImGui::SameLine();
            if (ImGui::Button("Add"))
            {
                if (strlen(m_newPrefixBuffer) > 0)
                {
                    hiddenMgr->AddPrefix(m_newPrefixBuffer);
                    ShaderInfoCollection::RefreshAllVisibilityStatic();
                    memset(m_newPrefixBuffer, 0, sizeof(m_newPrefixBuffer));
                }
            }

            ImGui::Separator();
            if (ImGui::Button("Reset to Defaults"))
            {
                hiddenMgr->ResetToDefaults();
                ShaderInfoCollection::RefreshAllVisibilityStatic();
            }

            ImGui::EndTabItem();
        }

        // Property Visibility Tab
        if (ImGui::BeginTabItem("Property Visibility"))
        {
            std::shared_ptr<ShaderInfoCollection> collection = ShaderInfoCollection::Get();
            if (collection)
            {
                std::vector<std::shared_ptr<ShaderInfo>>& shaders = collection->GetShaders();

                if (!shaders.empty())
                {
                    // Shader selector
                    std::vector<const char*> shaderNames;
                    for (auto& shader : shaders)
                    {
                        shaderNames.push_back(shader->Name().c_str());
                    }

                    if (m_selectedShaderForProps >= static_cast<int>(shaders.size()))
                        m_selectedShaderForProps = 0;

                    ImGui::Combo("Select Shader", &m_selectedShaderForProps, shaderNames.data(), static_cast<int>(shaderNames.size()));
                    ImGui::Separator();

                    // List properties with visibility toggles
                    ShaderInfo& selectedShader = *shaders[m_selectedShaderForProps];
                    auto& properties = selectedShader.GetProperties();

                    ImGui::Text("Toggle visibility for each property:");
                    for (ShaderProperty& l_prop : properties)
                    {
                        bool visible = l_prop.visible;
                        if (ImGui::Checkbox(l_prop.name.c_str(), &visible))
                        {
                            l_prop.SetVisible(visible);
                        }
                        ImGui::SameLine();
                        ImGui::TextDisabled("(%s)", l_prop.Type().c_str());
                    }
                }
                else
                {
                    ImGui::Text("No shaders loaded.");
                }
            }
            else
            {
                ImGui::Text("ShaderInfoCollection not initialized.");
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::EndTabItem();
}

void GUI::ShowShaderManagement()
{
    if (!ImGui::BeginTabItem("Shaders"))
        return;

    auto collection = ShaderInfoCollection::Get();
    if (!collection)
    {
        ImGui::Text("ShaderInfoCollection not initialized.");
        ImGui::EndTabItem();
        return;
    }

    // Load new shader section
    ImGui::Text("Load New Shader:");
    ImGui::InputText("File Path", m_newShaderPathBuffer, sizeof(m_newShaderPathBuffer));
    ImGui::InputText("Display Name", m_newShaderNameBuffer, sizeof(m_newShaderNameBuffer));

    if (ImGui::Button("Load Shader"))
    {
        if (strlen(m_newShaderPathBuffer) > 0 && strlen(m_newShaderNameBuffer) > 0)
        {
            auto l_loaded = collection->Load(m_newShaderPathBuffer, m_newShaderNameBuffer).lock();
            if (l_loaded)
            {
                memset(m_newShaderPathBuffer, 0, sizeof(m_newShaderPathBuffer));
                memset(m_newShaderNameBuffer, 0, sizeof(m_newShaderNameBuffer));
            }
        }
    }

    ImGui::Separator();
    ImGui::Text("Loaded Shaders:");

    auto& shaders = collection->GetShaders();
    std::string shaderToRemove;

    for (size_t i = 0; i < shaders.size(); ++i)
    {
        ImGui::PushID(static_cast<int>(i));
        
        if (ImGui::Button("X"))
        {
            shaderToRemove = shaders[i]->Name();
        }
        ImGui::SameLine();
        
        bool enabled = shaders[i]->Enabled();
        if (ImGui::Checkbox("##enabled", &enabled))
        {
            shaders[i]->Enabled(enabled);
        }
        ImGui::SameLine();
        
        ImGui::Text("%s", shaders[i]->Name().c_str());
        
        ImGui::PopID();
    }

    if (!shaderToRemove.empty())
    {
        collection->RemoveShader(shaderToRemove);
    }

    ImGui::EndTabItem();
}

void GUI::ShowEnabledShaders()
{
    if (!ImGui::BeginTabItem("Shaders"))
        return;

    std::shared_ptr<ShaderInfoCollection> l_collection = ShaderInfoCollection::Get();
    if (!l_collection)
    {
        ImGui::Text("ShaderInfoCollection not initialized.");
        ImGui::EndTabItem();
        return;
    }

    for (std::shared_ptr<ShaderInfo>& l_shaderInfo : l_collection->GetShaders())
    {
        bool l_enabled = l_shaderInfo->Enabled();
        if (ImGui::Checkbox(l_shaderInfo->Name().c_str(), &l_enabled))
        {
            l_shaderInfo->Enabled(l_enabled);
        }

        if (l_enabled)
        {
            ImGui::Indent();
            
            std::vector<ShaderProperty*> l_visibleProps = l_shaderInfo->GetVisibleProperties();
            for (ShaderProperty* l_prop : l_visibleProps)
            {
                ImGui::PushID(l_prop->name.c_str());
                
                switch (l_prop->type)
                {
                    case ShaderProperty::INT:
                        ImGui::DragInt(l_prop->name.c_str(), &l_prop->value.i);
                        break;
                    case ShaderProperty::FLOAT:
                        ImGui::DragFloat(l_prop->name.c_str(), &l_prop->value.f, 0.01f);
                        break;
                    case ShaderProperty::BOOL:
                        ImGui::Checkbox(l_prop->name.c_str(), &l_prop->value.b);
                        break;
                    case ShaderProperty::VEC2:
                        ImGui::DragFloat2(l_prop->name.c_str(), &l_prop->value.vec2[0], 0.01f);
                        break;
                    case ShaderProperty::VEC3:
                        ImGui::DragFloat3(l_prop->name.c_str(), &l_prop->value.vec3[0], 0.01f);
                        break;
                    case ShaderProperty::VEC4:
                        ImGui::DragFloat4(l_prop->name.c_str(), &l_prop->value.vec4[0], 0.01f);
                        break;
                }
                
                ImGui::PopID();
            }
            
            ImGui::Unindent();
        }
    }

    ImGui::EndTabItem();
}