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

#include "Raytracer/Raytracer.h"
#include "Shader/ShaderInfo.h"

#include "Datastructs/ShaderStorageBuffer.h"
#include "Datastructs/Light.h"
#include "Datastructs/Material.h"

#include "Window.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"

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
    std::weak_ptr<Raytracer> m_rt;

    std::weak_ptr<Window> m_window;

    /** @brief Pointer to the light SSBO */
    std::weak_ptr<ShaderStorageBuffer<Light>> m_lightSSBO;

    /** @brief Pointer to the material SSBO */
    std::weak_ptr<ShaderStorageBuffer<Material>> m_materialSSBO;
    
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
     * @param _window Pointer to the window instance to control
     * @param _lightSSBO Pointer to the light SSBO
     * @param _materialSSBO Pointer to the material SSBO
     */
    GUI(std::weak_ptr<Raytracer> _rt, 
        std::weak_ptr<Window> _window,
        std::weak_ptr<ShaderStorageBuffer<Light>> _lightSSBO,
        std::weak_ptr<ShaderStorageBuffer<Material>> _materialSSBO);

    /** @brief Destructor */
    ~GUI();

    /**
     * @brief Main UI render function - displays all UI windows
     * @param _deltaTime Time elapsed since last frame (for display purposes)
     */
    void ShowUI(float _deltaTime);

    /**
     * @brief Display the camera information window
     * 
     * Shows current camera position, direction vectors, and FOV.
     * All values are read-only display.
     */
    void ShowCameraInfoUI();

    /**
     * @brief Display the scene settings window with Lights and Materials tabs
     * 
     * Provides a tabbed interface for editing light properties (position, color,
     * intensity, radius) and material properties (color, roughness, metallic, etc.)
     */
    void ShowSceneSettingsUI();

    /**
     * @brief Display the visibility settings window with tabs for prefix and property control
     * 
     * Tab 1: Manage hidden uniform prefixes (add/remove prefixes that hide uniforms from GUI)
     * Tab 2: Per-property visibility toggle for individual shader properties
     */
    void ShowVisibilitySettingsUI();

    /**
     * @brief Display the shader management window
     * 
     * Allows loading new shaders from file paths and removing existing shaders.
     * Shows list of currently loaded shaders with remove buttons.
     */
    void ShowShaderManagementUI();

    /**
     * @brief Display the enabled shaders window
     * 
     * Shows checkboxes to enable/disable each shader and displays
     * editable properties for each enabled shader's visible uniforms.
     */
    void ShowEnabledShadersUI();

    void ShowWindowSettingsUI();
};

#endif
