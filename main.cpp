
#include "Raytracer.h"
#include "Input.h"
#include "Datastructs/Texture.h"
#include "Datastructs/Model.h"
#include "Datastructs/Light.h"

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

	printf("Initialising RT\n");
	// This will handle rendering to screen
	Raytracer l_raytracer(l_winSize);

	Input l_inputMap;

	Model l_sphereModel("./resources/objects/curuthers.obj");

	Light l_light;
	l_light.position = glm::vec3(0.0f, 0.0f, 3.0f);
	l_light.colour = glm::vec3(1.0f, 1.0f, 1.0f);
	
	l_raytracer.AddLight(l_light);

	printf("Creating vectors\n");
	std::vector<Triangle> l_tris = l_sphereModel.GetTriangles(glm::vec3(0.0f));
	std::vector<Texture> l_textures;
	std::vector<Material> l_materials;

	Texture l_modelTexture = Texture("./resources/textures/Whiskers_diffuse.png");
	
	Material l_material;
	l_material.albedo = glm::vec3(1.0f);
	l_material.metallic = 1.0f;
	l_material.roughness = 0.0f;
	l_material.ambientOcclusion = 1.0f;

	for(int i = 0; i < l_tris.size(); i++)
	{
		l_tris[i].textureId = 0;
		l_tris[i].materialId = 0;
	}

	l_materials.push_back(l_material);
	l_textures.push_back(l_modelTexture);

	
	printf("Setting materials\n");
	l_raytracer.SetMaterials(&l_materials);
	printf("Setting textures\n");
	l_raytracer.SetTextures(&l_textures);
	printf("Setting tris\n");
	l_raytracer.SetTris(&l_tris);

	Light* l_light0 = l_raytracer.GetLight(0);
	Camera* l_rtCam = l_raytracer.GetCamera();

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

		l_rtCam->Update(l_inputMap);

		l_raytracer.Trace();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Settings");

		glm::vec3 lightPos = l_light0->position;
		ImGui::DragFloat3("Position", &(lightPos[0]), 0.1f, -10.0f, 10.0f);
		l_light0->position = lightPos;

		bool l_shadow = l_raytracer.Shadows();
		ImGui::Checkbox("Shadows", &l_shadow);
		l_raytracer.Shadows(l_shadow);

		bool l_shade = l_raytracer.Shading();
		ImGui::Checkbox("Shading", &l_shade);
		l_raytracer.Shading(l_shade);

		ImGui::End();

		ImGui::Begin("Camera Info");

		glm::vec3 camPos = l_rtCam->Position();
		std::string l_camPosText = "Camera Position: (" + std::to_string(camPos.x) + ", " + std::to_string(camPos.y) + ", " + std::to_string(camPos.z) + ")";
		ImGui::Text(l_camPosText.c_str());

		glm::quat camRot = l_rtCam->Rotation();
		l_camPosText = "Camera Rotation: (" + std::to_string(camRot.x) + ", "+ std::to_string(camRot.y) + ", "+ std::to_string(camRot.z) + ", "+ std::to_string(camRot.w) + ")";
		ImGui::Text(l_camPosText.c_str());

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();

		ImGui::Begin("Material settings");

		float met = l_material.metallic;
		ImGui::SliderFloat("Metallic", &met, 0, 1);
		l_material.metallic = met;

		float rog = l_material.roughness;
		ImGui::SliderFloat("Roughness", &rog, 0, 1);
		l_material.roughness = rog;
		
		float ao = l_material.ambientOcclusion;
		ImGui::SliderFloat("AO", &ao, 0, 1);
		l_material.ambientOcclusion = ao;

		ImGui::End();

		l_raytracer.Show();
	}

	l_raytracer.Shutdown();

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