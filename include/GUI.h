/**
 * @file GUI.h
 * @brief ImGui-based graphical user interface for the raytracer
 * 
 * This file provides the GUI class for rendering ImGui interfaces including
 * camera information, scene settings (lights and materials), shader management,
 * and uniform visibility controls.
 */

#ifndef GUI_H
#define GUI_H

#include "Raytracer.h"
#include "ShaderInfo.h"

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_sdl.h"
#include "IMGUI/imgui_impl_opengl3.h"

#include <string>
#include <vector>

/**
 * @class GUI
 * @brief Manages the ImGui-based user interface for the raytracer
 * 
 * Provides windows for displaying camera information, editing scene settings
 * (lights and materials), managing shaders, and controlling uniform visibility.
 */
class GUI
{
private:
    /** @brief Pointer to the raytracer instance */
    Raytracer* m_rt;
    
    /** @brief Currently selected light index in the UI */
    int m_selectedLight;
    
    /** @brief Currently selected material index in the UI */
    int m_selectedMaterial;
    
    /** @brief Currently selected shader index for property editing */
    int m_selectedShaderForProps;
    
    /** @brief Buffer for entering new hidden prefix text */
    char m_newPrefixBuffer[128];
    
    /** @brief Buffer for entering new shader file path */
    char m_newShaderPathBuffer[256];
    
    /** @brief Buffer for entering new shader display name */
    char m_newShaderNameBuffer[128];

public:
    /**
     * @brief Construct a new GUI object
     * @param _rt Pointer to the raytracer instance to control
     */
    GUI(Raytracer* _rt) 
        : m_rt(_rt)
        , m_selectedLight(0)
        , m_selectedMaterial(0)
        , m_selectedShaderForProps(0)
    {
        memset(m_newPrefixBuffer, 0, sizeof(m_newPrefixBuffer));
        memset(m_newShaderPathBuffer, 0, sizeof(m_newShaderPathBuffer));
        memset(m_newShaderNameBuffer, 0, sizeof(m_newShaderNameBuffer));
    }

    /** @brief Destructor */
    ~GUI() {}

    /**
     * @brief Main UI render function - displays all UI windows
     * @param _deltaTime Time elapsed since last frame (for display purposes)
     */
    void ShowUI(float _deltaTime)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ShowCameraInfoUI();
        ShowSceneSettingsUI();
        ShowEnabledShadersUI();
        ShowVisibilitySettingsUI();
        ShowShaderManagementUI();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    /**
     * @brief Display the camera information window
     * 
     * Shows current camera position, direction vectors, and FOV.
     * All values are read-only display.
     */
    void ShowCameraInfoUI()
    {
        ImGui::Begin("Camera Info");
        Camera& cam = m_rt->GetCamera();
        glm::vec3 pos = cam.Position();
        glm::vec3 fwd = cam.Forward();
        glm::vec3 right = cam.Right();
        glm::vec3 up = cam.Up();
        float fov = cam.fov();

        ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
        ImGui::Text("Forward:  (%.2f, %.2f, %.2f)", fwd.x, fwd.y, fwd.z);
        ImGui::Text("Right:    (%.2f, %.2f, %.2f)", right.x, right.y, right.z);
        ImGui::Text("Up:       (%.2f, %.2f, %.2f)", up.x, up.y, up.z);
        ImGui::Text("FOV:      %.2f", fov);
        ImGui::End();
    }

    /**
     * @brief Display the scene settings window with Lights and Materials tabs
     * 
     * Provides a tabbed interface for editing light properties (position, color,
     * intensity, radius) and material properties (color, roughness, metallic, etc.)
     */
    void ShowSceneSettingsUI()
    {
        ImGui::Begin("Scene Settings");

        if (ImGui::BeginTabBar("SceneSettingsTabBar"))
        {
            // Lights Tab
            if (ImGui::BeginTabItem("Lights"))
            {
                std::vector<Light>& lights = m_rt->GetLights();
                int lightCount = static_cast<int>(lights.size());

                if (lightCount > 0)
                {
                    if (m_selectedLight >= lightCount)
                        m_selectedLight = lightCount - 1;

                    // Light selector
                    std::vector<const char*> lightNames;
                    for (int i = 0; i < lightCount; ++i)
                    {
                        static char nameBuf[32];
                        snprintf(nameBuf, sizeof(nameBuf), "Light %d", i);
                        lightNames.push_back(nameBuf);
                    }

                    ImGui::Combo("Select Light", &m_selectedLight, lightNames.data(), lightCount);
                    ImGui::Separator();

                    Light& light = lights[m_selectedLight];
                    ImGui::DragFloat3("Position", &light.position.x, 0.1f);
                    ImGui::ColorEdit3("Color", &light.color.x);
                    ImGui::DragFloat("Intensity", &light.intensity, 0.01f, 0.0f, 100.0f);
                    ImGui::DragFloat("Radius", &light.radius, 0.01f, 0.0f, 10.0f);

                    m_rt->SyncLightToShader(m_selectedLight);
                }
                else
                {
                    ImGui::Text("No lights in the scene.");
                }

                ImGui::EndTabItem();
            }

            // Materials Tab
            if (ImGui::BeginTabItem("Materials"))
            {
                std::vector<Material>& materials = m_rt->GetMaterials();
                int materialCount = static_cast<int>(materials.size());

                if (materialCount > 0)
                {
                    if (m_selectedMaterial >= materialCount)
                        m_selectedMaterial = materialCount - 1;

                    // Material selector
                    std::vector<const char*> materialNames;
                    for (int i = 0; i < materialCount; ++i)
                    {
                        static char nameBuf[32];
                        snprintf(nameBuf, sizeof(nameBuf), "Material %d", i);
                        materialNames.push_back(nameBuf);
                    }

                    ImGui::Combo("Select Material", &m_selectedMaterial, materialNames.data(), materialCount);
                    ImGui::Separator();

                    Material& mat = materials[m_selectedMaterial];
                    ImGui::ColorEdit3("Albedo", &mat.albedo.x);
                    ImGui::DragFloat("Roughness", &mat.roughness, 0.01f, 0.0f, 1.0f);
                    ImGui::DragFloat("Metallic", &mat.metallic, 0.01f, 0.0f, 1.0f);
                    ImGui::DragFloat("Ambient Occlusion", &mat.ambientOcclusion, 0.01f, 0.0f, 1.0f);

                    m_rt->SyncMaterialToShader(m_selectedMaterial);
                }
                else
                {
                    ImGui::Text("No materials in the scene.");
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    /**
     * @brief Display the visibility settings window with tabs for prefix and property control
     * 
     * Tab 1: Manage hidden uniform prefixes (add/remove prefixes that hide uniforms from GUI)
     * Tab 2: Per-property visibility toggle for individual shader properties
     */
    void ShowVisibilitySettingsUI()
    {
        ImGui::Begin("Visibility Settings");

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
                auto collection = ShaderInfoCollection::Get();
                if (collection)
                {
                    auto& shaders = collection->GetShaders();

                    if (!shaders.empty())
                    {
                        // Shader selector
                        std::vector<const char*> shaderNames;
                        for (auto& shader : shaders)
                        {
                            shaderNames.push_back(shader.Name().c_str());
                        }

                        if (m_selectedShaderForProps >= static_cast<int>(shaders.size()))
                            m_selectedShaderForProps = 0;

                        ImGui::Combo("Select Shader", &m_selectedShaderForProps, shaderNames.data(), static_cast<int>(shaderNames.size()));
                        ImGui::Separator();

                        // List properties with visibility toggles
                        ShaderInfo& selectedShader = shaders[m_selectedShaderForProps];
                        auto& properties = selectedShader.GetProperties();

                        ImGui::Text("Toggle visibility for each property:");
                        for (auto& prop : properties)
                        {
                            bool visible = prop.visible;
                            if (ImGui::Checkbox(prop.name.c_str(), &visible))
                            {
                                prop.SetVisible(visible);
                            }
                            ImGui::SameLine();
                            ImGui::TextDisabled("(%s)", prop.Type().c_str());
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

        ImGui::End();
    }

    /**
     * @brief Display the shader management window
     * 
     * Allows loading new shaders from file paths and removing existing shaders.
     * Shows list of currently loaded shaders with remove buttons.
     */
    void ShowShaderManagementUI()
    {
        ImGui::Begin("Shader Management");

        auto collection = ShaderInfoCollection::Get();
        if (!collection)
        {
            ImGui::Text("ShaderInfoCollection not initialized.");
            ImGui::End();
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
                ShaderInfo* loaded = collection->LoadShader(m_newShaderPathBuffer, m_newShaderNameBuffer);
                if (loaded)
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
                shaderToRemove = shaders[i].Name();
            }
            ImGui::SameLine();
            
            bool enabled = shaders[i].Enabled();
            if (ImGui::Checkbox("##enabled", &enabled))
            {
                shaders[i].Enabled(enabled);
            }
            ImGui::SameLine();
            
            ImGui::Text("%s", shaders[i].Name().c_str());
            
            ImGui::PopID();
        }

        if (!shaderToRemove.empty())
        {
            collection->RemoveShader(shaderToRemove);
        }

        ImGui::End();
    }

    /**
     * @brief Display the enabled shaders window
     * 
     * Shows checkboxes to enable/disable each shader and displays
     * editable properties for each enabled shader's visible uniforms.
     */
    void ShowEnabledShadersUI()
    {
        ImGui::Begin("Enabled Shaders");

        auto collection = ShaderInfoCollection::Get();
        if (!collection)
        {
            ImGui::Text("ShaderInfoCollection not initialized.");
            ImGui::End();
            return;
        }

        for (ShaderInfo& shaderInfo : collection->GetShaders())
        {
            bool enabled = shaderInfo.Enabled();
            if (ImGui::Checkbox(shaderInfo.Name().c_str(), &enabled))
            {
                shaderInfo.Enabled(enabled);
            }

            if (enabled)
            {
                ImGui::Indent();
                
                auto visibleProps = shaderInfo.GetVisibleProperties();
                for (ShaderProperty* prop : visibleProps)
                {
                    ImGui::PushID(prop->name.c_str());
                    
                    switch (prop->type)
                    {
                        case ShaderProperty::INT:
                            ImGui::DragInt(prop->name.c_str(), &prop->value.i);
                            break;
                        case ShaderProperty::FLOAT:
                            ImGui::DragFloat(prop->name.c_str(), &prop->value.f, 0.01f);
                            break;
                        case ShaderProperty::BOOL:
                            ImGui::Checkbox(prop->name.c_str(), &prop->value.b);
                            break;
                        case ShaderProperty::VEC2:
                            ImGui::DragFloat2(prop->name.c_str(), prop->value.vec2, 0.01f);
                            break;
                        case ShaderProperty::VEC3:
                            ImGui::DragFloat3(prop->name.c_str(), prop->value.vec3, 0.01f);
                            break;
                        case ShaderProperty::VEC4:
                            ImGui::DragFloat4(prop->name.c_str(), prop->value.vec4, 0.01f);
                            break;
                    }
                    
                    ImGui::PopID();
                }
                
                ImGui::Unindent();
            }
        }

        ImGui::End();
    }
};

#endif
