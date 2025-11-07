#ifndef IMGUIDISPLAY_H
#define IMGUIDISPLAY_H

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "Sphere.h"
#include "ComputeShader.h"

#include <memory>
#include <vector>


class IMGUIDisplay
{
    private:
    ComputeShader* m_compute;
    std::shared_ptr<std::vector<Sphere>> m_spheres;
    bool m_showWindow;

    glm::vec3 m_cameraForward;
    glm::vec3 m_cameraUp;
    glm::vec3 m_cameraRight;
    glm::vec3 m_cameraPosition;

    float m_fov;
    glm::vec2 m_resolution;

    glm::vec3 m_lightDir;
    glm::vec3 m_lightColor;

    public:

    IMGUIDisplay(ComputeShader* _compute, std::vector<Sphere>* _sphere) :
    m_cameraForward(1.0f, 0.0f, 0.0f),
    m_cameraRight(0.0f, 0.0f, 1.0f),
    m_cameraUp(0.0f, 1.0f, 0.0f),
    m_cameraPosition(0.0f, 0.0f, 3.0f),
    m_fov(0.5f),
    m_resolution(640, 480)
    {
        m_compute = _compute;

        m_spheres = std::shared_ptr<std::vector<Sphere>>();
        m_spheres.reset(_sphere);

        m_showWindow = true;
    };

    ~IMGUIDisplay()
    {

    };

    void DrawDisplay()
    {
        ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		// Our showLightingWindow variable is a state that is kept from frame-to-frame
		// It's the visibility of the GUI window
		// So we only draw the contents of the window if its actually visible
		if (m_showWindow)
		{
			// Create a window, give it a name
			// All ImGui commands after this to create widgets will be added to the window
			ImGui::Begin("Lighting Controls");

			// Here, we do the same sort of thing several times for different properties:
			//  1. Get a state from the scene
			//  2. Use the GUI to present an editor for it
			//  3. Send it back to the scene in case it's changed

			// Light stuff
			ImGui::DragFloat3("Light Direction", &(m_lightDir[0]));
			m_compute->SetUniform("lightDir", m_lightDir);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			// We've finished adding stuff to the window
			ImGui::End();
		}

		// Render GUI to screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    };

    void SetShader(ComputeShader* _shader)
    {
        m_compute = _shader;
    };
};

#endif