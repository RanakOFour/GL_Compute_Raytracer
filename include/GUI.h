#ifndef IMGUI_H
#define IMGUI_H

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "Raytracer.h"


/*
*  @brief
*  Contains the ImGui calls for controlling the raytracer
*/
class GUI
{
    private:
    Raytracer* m_rt;

    int m_selectedLight;
    int m_selectedMaterial;


    public:
    GUI(Raytracer* _rt)
    : m_rt(_rt)
    , m_selectedLight(0)
    , m_selectedMaterial(0)
    {

    };

    ~GUI() {};


    void ShowUI(float& _deltaTime)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // I was going to abstract the ComputeShaders into ComputeInformation, that would then handle
        // all of these options as contained data structures that could be looped through, 
        // but the design hurdles of how to expose options for specific values controlled by the raytracer (Sample count, uniforms, etc.)
        // made me drop the idea, because the compromises I had came up with between the RT exposing
        // parts and adding ways to interact with the pipeline weren't 'good enough'.

        // The pattern is clearly there, I think it would just need some more time in the oven

        // Settings for individual lights

        ImGui::Begin("Light Settings");

        std::string l_lightSelectCombo = "";
        
        for(int i = 0; i < m_rt->m_lights.size(); i++)
        {
            l_lightSelectCombo += "Light " + std::to_string(i + 1) + '\0';
        }

        l_lightSelectCombo += "\0";

        ImGui::Combo("Selected Light", &m_selectedLight, l_lightSelectCombo.c_str());

        Light* l_selectedLight = &m_rt->m_lights[m_selectedLight];

        bool quad = l_selectedLight->radius == 0.0f;
        ImGui::Checkbox("Quad light", &quad);

        if(quad)
        {
            l_selectedLight->radius = 0.0f;
        }
        else
        {
            if(l_selectedLight->radius == 0.0f)
            {
                l_selectedLight->radius = 0.1f;
            }
        }

        glm::vec3 lightPos = l_selectedLight->position;
        
        if(l_selectedLight->radius > 0.0f)
        {
            ImGui::DragFloat3("Light Position", &(lightPos[0]), 0.1f, -10.0f, 10.0f);

            float l_radius = l_selectedLight->radius;
            ImGui::SliderFloat("Light Radius", &l_radius, 0.001f, 10.0f);
            l_selectedLight->radius = l_radius;
        }
        else
        {
            ImGui::DragFloat3("Light Corner 1", &(lightPos[0]), 0.1f, -10.0f, 10.0f);
            
            glm::vec3 lightPos2 = l_selectedLight->cornerA;
            ImGui::DragFloat3("Light Corner 2", &(lightPos2[0]), 0.1f, -10.0f, 10.0f);
            l_selectedLight->cornerA = lightPos2;

            glm::vec3 lightPos3 = l_selectedLight->cornerB;
            ImGui::DragFloat3("Light Corner 3", &(lightPos3[0]), 0.1f, -10.0f, 10.0f);
            l_selectedLight->cornerB = lightPos3;
        }

        l_selectedLight->position = lightPos;

        glm::vec3 lightCol = l_selectedLight->colour;
        ImGui::ColorEdit3("Light Colour", &(lightCol[0]));
        l_selectedLight->colour = lightCol;

        float inten = l_selectedLight->intensity;
        ImGui::SliderFloat("Light Intensity", &(inten), 0.0f, 100.0f);
        l_selectedLight->intensity = inten;

        if(ImGui::Button("Add Light"))
        {
            Light l_newLight;
            l_newLight.position = glm::vec3(0.0f);
            l_newLight.colour = glm::vec3(1.0f);
            l_newLight.radius = 1.0f;

            m_rt->m_lights.push_back(l_newLight);
        }

        if(ImGui::Button("Delete Light"))
        {
            if(m_rt->m_lights.size() > 1)
            {
                m_rt->m_lights.erase(m_rt->m_lights.begin() + m_selectedLight);
            }

            if(m_selectedLight >= m_rt->m_lights.size())
            {
                m_selectedLight = m_rt->m_lights.size() - 1;
            }
        }

        ImGui::End();

        // A box for enabling/disabling different compute shaders

        // How would you list the uniforms and interpret what they mean inside a ComputeInformation?

        ImGui::Begin("Enabled Shaders");

        bool l_shadow = m_rt->m_shadows;
        ImGui::Checkbox("Shadows", &l_shadow);
        m_rt->m_shadows = l_shadow;

        if(l_shadow)
        {
            int samples = m_rt->m_sampleCount;
            ImGui::InputInt("Sample Count", &samples, 1, 100);
            m_rt->m_sampleCount = samples;
        }

        bool l_shade = m_rt->m_shading;
        ImGui::Checkbox("Shading", &l_shade);
        m_rt->m_shading = l_shade;		

        bool l_lights = m_rt->m_lightVision;
        ImGui::Checkbox("Light Vision", &l_lights);
        m_rt->m_lightVision = l_lights;

        ImGui::End();

        // A box that displays the camera information
        ImGui::Begin("Camera Info");

        glm::vec3 camPos = m_rt->m_camera.Position();
        std::string l_camPosText = "Camera Position: (" + std::to_string(camPos.x) + ", " + std::to_string(camPos.y) + ", " + std::to_string(camPos.z) + ")";
        ImGui::Text(l_camPosText.c_str());

        glm::quat camRot = m_rt->m_camera.Rotation();
        l_camPosText = "Camera Rotation: (" + std::to_string(camRot.x) + ", "+ std::to_string(camRot.y) + ", "+ std::to_string(camRot.z) + ", "+ std::to_string(camRot.w) + ")";
        ImGui::Text(l_camPosText.c_str());

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();


        // Settings for individual materials

        ImGui::Begin("Material settings");

        std::string l_materialSelectCombo = "";
        
        for(int i = 0; i < m_rt->m_mats->size(); i++)
        {
            l_materialSelectCombo += "Mat " + std::to_string(i + 1) + '\0';
        }

        l_materialSelectCombo += "\0";

        ImGui::Combo("Selected Material", &m_selectedMaterial, l_materialSelectCombo.c_str());

        Material* l_selectedMat = &(m_rt->m_mats->at(m_selectedMaterial));

        glm::vec3 l_alb = l_selectedMat->albedo;
        ImGui::ColorEdit3("Albedo", &l_alb[0]);
        l_selectedMat->albedo = l_alb;

        float met = l_selectedMat->metallic;
        ImGui::SliderFloat("Metallic", &met, 0, 1);
        l_selectedMat->metallic = met;

        float rog = l_selectedMat->roughness;
        ImGui::SliderFloat("Roughness", &rog, 0, 1);
        l_selectedMat->roughness = rog;
        
        float ao = l_selectedMat->ambientOcclusion;
        ImGui::SliderFloat("AO", &ao, 0, 1);
        l_selectedMat->ambientOcclusion = ao;

        ImGui::End();
    };
};

#endif