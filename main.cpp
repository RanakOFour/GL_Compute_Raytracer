
#include "GCP_GFX_Framework.h"
#include "Camera.h"
#include "Light.h"
#include "ComputeShader.h"
#include "Model.h"
#include "BVH.h"
#include "Input.h"
#include "Texture.h"

#include "GL/glew.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <vector>

inline void HandleKBDownInput(Input& _inputMap, SDL_KeyboardEvent& _keyEvent);
inline void HandleKBUpInput(Input& _inputMap, SDL_KeyboardEvent& _keyEvent);
inline void HandleMouseInput(Input& _inputMap, SDL_MouseMotionEvent& _mouseEvent);

int main(int argc, char* argv[])
{
	// Set window size
	glm::ivec2 l_winSize(1000, 800);

	// This will handle rendering to screen
	GCP_Framework l_myFramework;

	// Initialises SDL and OpenGL and sets up a framebuffer
	if (!l_myFramework.Init(l_winSize))
	{
		return -1;
	}

	Input l_inputMap;
    
    // Camera setup
    Camera l_camera(glm::vec2(500, 500));

	ComputeShader l_compute("./resources/shaders/RTComputeTriangle.comp");
    
    l_compute.use();

	Model l_sphereModel("./resources/objects/curuthers.obj");

	

    // Set l_camera uniforms
    l_camera.UpdateShader(l_compute);

	Light l_light;
	l_light.position = glm::vec3(0.0f, 0.0f, 3.0f);
	l_light.colour = glm::vec3(1.0f, 1.0f, 1.0f);

	l_compute.SetUniform("u_numLights", 1);
	l_compute.SetUniform("u_lights[0].position", l_light.position);
	l_compute.SetUniform("u_lights[0].color", l_light.colour);

	std::vector<Triangle> l_tris = l_sphereModel.GetTriangles(glm::vec3(0.0f));

	printf("%i Triangles sent to BVH\n", (int)l_tris.size());

	BVH l_BVH(&(l_tris));

	GLuint l_triangleBuffer;
	glGenBuffers(1, &l_triangleBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, l_triangleBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Triangle) * l_tris.size(), &(l_tris.at(0)), GL_DYNAMIC_READ);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, l_triangleBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	Texture l_modelTexture = Texture("./resources/textures/Whiskers_diffuse.png");
	// Bind all the required stuff to the raytracer shader
	l_myFramework.SetGLTexture();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, l_triangleBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, l_BVH.GetIndexSSBO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, l_BVH.GetNodeSSBO());
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, l_modelTexture.GetID());

	bool l_keepGoing = true;
	bool l_mouseMovement = false;

	SDL_Event l_event;
	while(l_keepGoing)
	{
		l_mouseMovement = false;
		while(SDL_PollEvent(&l_event))
		{
			ImGui_ImplSDL2_ProcessEvent(&l_event);
			switch(l_event.type)
			{
				case SDL_QUIT:
					l_keepGoing = false;
					break;

				case SDL_KEYDOWN:
					if (SDL_GetRelativeMouseMode())
					{
						HandleKBDownInput(l_inputMap, l_event.key);
					}
					break;

				case SDL_KEYUP:
					if(SDL_GetRelativeMouseMode())
					{
						HandleKBUpInput(l_inputMap, l_event.key);
					}

				case SDL_MOUSEMOTION:
					if (SDL_GetRelativeMouseMode())
					{
						l_mouseMovement = true;
						HandleMouseInput(l_inputMap, l_event.motion);
					}
					break;

				case SDL_MOUSEBUTTONDOWN:
					if (l_event.button.button == SDL_BUTTON_RIGHT)
					{
						SDL_SetRelativeMouseMode(SDL_TRUE);
					}
			}
		}

		if(!l_mouseMovement)
		{
			l_inputMap.deltaMouseX = 0.0f;
			l_inputMap.deltaMouseY = 0.0f;
		}

		l_camera.Update(l_inputMap);

		float time = (float)SDL_GetTicks64() * 1000.0f;
		
		l_compute.use();

		l_compute.SetUniform("u_lights[0].position", l_light.position);
		l_camera.UpdateShader(l_compute);

		l_myFramework.SetGLTexture();

		glDispatchCompute((unsigned int)l_winSize.x, (unsigned int)l_winSize.y, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Settings");

		glm::vec3 lightPos = l_light.position;
		ImGui::DragFloat3("Position", &(lightPos[0]), 0.1f, -10.0f, 10.0f);
		l_light.position = lightPos;

		int planes = l_BVH.PlaneCount();
		ImGui::DragInt("Plane Count", &planes, 1, 1, 1000);
		l_BVH.PlaneCount(planes);

		ImGui::End();

		ImGui::Begin("Camera Info");

		glm::vec3 camPos = l_camera.Position();
		std::string l_camPosText = "Camera Position: (" + std::to_string(camPos.x) + ", " + std::to_string(camPos.y) + ", " + std::to_string(camPos.z) + ")";
		ImGui::Text(l_camPosText.c_str());

		glm::quat camRot = l_camera.Rotation();
		l_camPosText = "Camera Rotation: (" + std::to_string(camRot.x) + ", "+ std::to_string(camRot.y) + ", "+ std::to_string(camRot.z) + ", "+ std::to_string(camRot.w) + ")";
		ImGui::Text(l_camPosText.c_str());

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();

		l_myFramework.Show();
	}

	l_myFramework.Shutdown();


    return 0;
};

void HandleKBDownInput(Input& _inputMap, SDL_KeyboardEvent& _keyEvent)
{
	int l_key = _keyEvent.keysym.sym;
	switch (l_key)
	{
		case SDLK_w:
			_inputMap.forward = 1;
			break;

		case SDLK_s:
			_inputMap.forward = -1;
			break;

		case SDLK_d:
			_inputMap.right = 1;
			break;

		case SDLK_a:
			_inputMap.right = -1;
			break;

		case SDLK_e:
			_inputMap.up = 1;
			break;

		case SDLK_q:
			_inputMap.up = -1;
			break;

		case SDLK_ESCAPE:
			SDL_SetRelativeMouseMode(SDL_FALSE);
			break;
			
	}
};

void HandleKBUpInput(Input& _inputMap, SDL_KeyboardEvent& _keyEvent)
{
	int l_key = _keyEvent.keysym.sym;

	// Don't reset movement if the opposite button is also being held down
	if((l_key == SDLK_w && _inputMap.forward == 1) || (l_key == SDLK_s && _inputMap.forward == -1))
	{
		_inputMap.forward = 0;
	}
	else if((l_key == SDLK_d && _inputMap.right == 1) || (l_key == SDLK_a && _inputMap.right == -1))
	{
		_inputMap.right = 0;
	}
	else if((l_key == SDLK_e && _inputMap.up == 1) || (l_key == SDLK_q && _inputMap.up == -1))
	{
		_inputMap.up = 0;
	}
};

void HandleMouseInput(Input& _inputMap, SDL_MouseMotionEvent& _mouseEvent)
{
	_inputMap.deltaMouseX = glm::radians((float)-_mouseEvent.xrel) * 3.0f;
	_inputMap.deltaMouseY = glm::radians((float)-_mouseEvent.yrel) * 3.0f;
};