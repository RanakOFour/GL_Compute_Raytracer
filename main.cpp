
#include "Raytracer.h"
#include "GUI.h"
#include "Input.h"
#include "Datastructs/Texture.h"
#include "Datastructs/Model.h"
#include "Datastructs/Light.h"

#include "GL/glew.h"

#include "SDL/SDL.h"
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
	GUI l_gui(&l_raytracer);

	std::string l_masterString = "./resources/pipelines/master.txt";

	Input l_inputMap;

	Model l_curuthersModel("./resources/objects/curuthers.obj");

	Texture l_modelTexture = Texture("./resources/textures/Whiskers_diffuse.png");
	
	Material l_matCuruthers;
	l_matCuruthers.albedo = glm::vec3(1.0f);
	l_matCuruthers.metallic = 0.0f;
	l_matCuruthers.roughness = 0.0f;
	l_matCuruthers.ambientOcclusion = 1.0f;

	Model l_cubeModel("./resources/objects/cube.obj");

	Material l_matFloor;
	l_matFloor.albedo = glm::vec3(1.0f);
	l_matFloor.metallic = 0.0f;
	l_matFloor.roughness = 0.0f;
	l_matFloor.ambientOcclusion = 1.0f;

	Light l_light;
	l_light.position = glm::vec3(-1.5f, 3.0f, -1.5f);
	l_light.colour = glm::vec3(1.0f);
	l_light.intensity = 1.0f;
	l_light.radius = 1.0f;
	l_light.cornerA = glm::vec3(0.0, -1.0, 0.0);
	l_light.cornerB = glm::vec3(0.5, 0.5, 0.5);
	
	l_raytracer.AddLight(l_light);

	printf("Creating vectors\n");
	std::vector<Triangle> l_tris = l_curuthersModel.GetTriangles(glm::vec3(0.0f));

	for (int i = 0; i < l_tris.size(); i++)
	{
		l_tris[i].textureId = 0;
		l_tris[i].materialId = 0;
	}

	std::vector<Triangle> l_floorTris = l_cubeModel.GetTriangles(glm::vec3(0.0f, -2.3f, 0.0f),
																 glm::vec3(10.0f, 0.1f, 10.0f));

	for (int i = 0; i < l_floorTris.size(); i++)
	{
		l_floorTris[i].textureId = -1;
		l_floorTris[i].materialId = -1;
	}

	l_tris.insert(l_tris.end(), l_floorTris.begin(), l_floorTris.end());
	std::vector<Texture> l_textures;
	std::vector<Material> l_materials;

	l_materials.push_back(l_matCuruthers);
	l_materials.push_back(l_matFloor);

	l_textures.push_back(l_modelTexture);

	
	printf("Setting materials\n");
	l_raytracer.SetMaterials(&l_materials);
	printf("Setting textures\n");
	l_raytracer.SetTextures(&l_textures);
	printf("Setting tris\n");
	l_raytracer.SetTris(&l_tris);

	Camera* l_rtCam = l_raytracer.GetCamera();

	bool l_keepGoing = true;
	bool l_mouseMovement = false;

	Uint64 l_lastTime = SDL_GetTicks64();
	float l_deltaTime = 0.0f;

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

		Uint64 l_startTime = SDL_GetTicks64();
		l_deltaTime = (float)(l_startTime - l_lastTime) * 0.001f;
		l_lastTime = l_startTime;

		if(l_deltaTime < 16.6f)
		{
			SDL_Delay(16.6f - l_deltaTime);
		}
		
		printf("FPS: %f\nDT: %f\n", 1.0f / l_deltaTime, l_deltaTime);

		l_rtCam->Update(l_inputMap, l_deltaTime);

		l_raytracer.Trace(l_deltaTime);

		l_gui.ShowUI(l_deltaTime);

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