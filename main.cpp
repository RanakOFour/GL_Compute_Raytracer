#include "Window.h"
#include "GUI.h"
#include "Input.h"

#include "Raytracer/Raytracer.h"

#include "Raytracer/BufferBindPoints.h"
#include "ShaderStorageBuffer.h"
#include "TextureCollection.h"

#include "Datastructs/Texture.h"
#include "Datastructs/Model.h"
#include "Datastructs/Light.h"

#include "GL/glew.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <vector>
#include <chrono>

inline void HandleKBDownInput(Input& _inputMap, SDL_KeyboardEvent& _keyEvent);
inline void HandleKBUpInput(Input& _inputMap, SDL_KeyboardEvent& _keyEvent);
inline void HandleMouseInput(Input& _inputMap, SDL_MouseMotionEvent& _mouseEvent);

int main(int argc, char* argv[])
{
	// Set window size
	glm::ivec2 l_winSize(1000, 800);

	std::shared_ptr<Window> l_window = std::make_shared<Window>(l_winSize, l_winSize);
	std::shared_ptr<Raytracer> l_raytracer = std::make_shared<Raytracer>(l_window);

	std::shared_ptr<ShaderStorageBuffer<Triangle>> l_triangleSSBO =
		std::make_shared<ShaderStorageBuffer<Triangle>>();
	l_triangleSSBO->BindLocation(TRIANGLE_DATA);
	l_raytracer->AddSSBO(l_triangleSSBO);

	std::shared_ptr<ShaderStorageBuffer<Material>> l_materialSSBO =
		std::make_shared<ShaderStorageBuffer<Material>>();
	l_materialSSBO->BindLocation(MATERIALS);
	l_raytracer->AddSSBO(l_materialSSBO);

	std::shared_ptr<TextureCollection> l_matTexCollection =
		std::make_shared<TextureCollection>();
	l_matTexCollection->BindLocation(TEXTURES);
	l_raytracer->AddSSBO(l_matTexCollection);

	std::shared_ptr<ShaderStorageBuffer<Light>> l_lightSSBO =
		std::make_shared<ShaderStorageBuffer<Light>>();
	l_lightSSBO->BindLocation(LIGHTS);
	l_raytracer->AddSSBO(l_lightSSBO);

	// Create GUI with SSBO references
	GUI l_gui(l_raytracer, l_window, l_lightSSBO, l_materialSSBO);

	// Create curuthers model, texture and mat
	Model l_curuthersModel("./resources/objects/curuthers.obj");
	Texture l_modelTexture = Texture("./resources/textures/Whiskers_diffuse.png");
	Material l_matCuruthers;
	l_matCuruthers.albedo = glm::vec3(1.0f);
	l_matCuruthers.metallic = 0.0f;
	l_matCuruthers.roughness = 0.0f;
	l_matCuruthers.ambientOcclusion = 1.0f;

	// Create floor model and mat
	Model l_cubeModel("./resources/objects/cube.obj");

	Light l_light;
	l_light.position = glm::vec3(2.5f, 4.5f, 5.0f);
	l_light.colour = glm::vec3(1.0f);
	l_light.intensity = 15.0f;
	l_light.radius = 2.0f;
	
	l_lightSSBO->AddData(l_light);

	// All the triangles for the scene are pulled into one vector
	std::vector<Triangle> l_tris = l_curuthersModel.GetTriangles(glm::vec3(0.0f));

	for (int i = 0; i < l_tris.size(); i++)
	{
		l_tris[i].textureId = 0;
		l_tris[i].materialId = 0;
	}

	std::vector<Triangle> l_cornelBoxTris = l_cubeModel.GetTriangles(glm::vec3(0.0f, -2.3f, 0.0f),
																 glm::vec3(10.0f, 0.1f, 10.0f));

	for (int i = 0; i < l_cornelBoxTris.size(); i++)
	{
		l_cornelBoxTris[i].textureId = -1;
		l_cornelBoxTris[i].materialId = -1;
	}

	l_tris.insert(l_tris.end(), l_cornelBoxTris.begin(), l_cornelBoxTris.end());

	for(int i = 0; i < l_tris.size(); i++)
	{
		l_triangleSSBO->AddData(l_tris[i]);
	}

	l_materialSSBO->AddData(l_matCuruthers);
	l_matTexCollection->AddTexture(l_modelTexture);

	l_raytracer->BuildBVH(l_triangleSSBO->GetData());

	// The camera needs to be updated by Input data, so it is done here
	Input l_inputMap;
	Camera& l_rtCam = l_raytracer->GetCamera();
	l_rtCam.Position(glm::vec3(-15.0f, 4.7f, 4.0f));
	l_rtCam.Rotate(glm::radians(-60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	l_rtCam.Rotate(glm::radians(-40.0f), glm::vec3(1.0f, 0.0f, 0.0f));

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
					break;

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

		l_rtCam.Update(l_inputMap, l_deltaTime);

		auto time1 = std::chrono::high_resolution_clock::now();

		l_raytracer->Trace(l_deltaTime);

		auto time2 = std::chrono::high_resolution_clock::now();

		std::chrono::microseconds l_timeElapsed =
			std::chrono::duration_cast<std::chrono::microseconds>(time2 - time1);

		std::cout << "DeltaTime: " << l_timeElapsed.count() * 0.001f << " ms" << std::endl;

		l_gui.ShowUI(l_deltaTime);

		l_window->Show();

		std::cout << std::endl;
	}

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
	_inputMap.deltaMouseX = glm::radians((float)-_mouseEvent.xrel);
	_inputMap.deltaMouseY = glm::radians((float)-_mouseEvent.yrel);
};