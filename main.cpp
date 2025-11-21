
#include "GCP_GFX_Framework.h"
#include "Camera.h"
#include "Light.h"
#include "ComputeShader.h"
#include "Model.h"
#include "BVH.h"

#include "GL/glew.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <vector>

inline void HandleKBInput(Camera& _camera, SDL_KeyboardEvent& _keyEvent);
inline void HandleMouseInput(Camera& _camera, SDL_MouseMotionEvent& _mouseEvent);

int main(int argc, char* argv[])
{
	// Set window size
	glm::ivec2 winSize(500, 500);

	// This will handle rendering to screen
	GCP_Framework _myFramework;

	// Initialises SDL and OpenGL and sets up a framebuffer
	if (!_myFramework.Init(winSize))
	{
		return -1;
	}
    
    // Camera setup
    Camera camera(glm::vec2(500, 500));

	ComputeShader myShader("./resources/shaders/RTComputeTriangle.comp");
    
    myShader.use();

	Model sphereModel("./resources/objects/curuthers.obj");
    
    // Set camera uniforms
    camera.UpdateShader(myShader);

	Light light;
	light.position = glm::vec3(0.0f, 0.0f, 3.0f);
	light.colour = glm::vec3(1.0f, 1.0f, 1.0f);

	myShader.SetUniform("numLights", 1);
	myShader.SetUniform("lights[0].position", light.position);
	myShader.SetUniform("lights[0].color", light.colour);

	std::vector<Triangle> tris[] = { sphereModel.GetTriangles(glm::vec3(0.0f))
								   , sphereModel.GetTriangles(glm::vec3(3.0f, 0.0f, 3.0f))
								   , sphereModel.GetTriangles(glm::vec3(-3.0f, 0.0f, -3.0f)) 
	};

	/*tris[0].insert(tris[0].end(), tris[1].begin(), tris[1].end());
	tris[0].insert(tris[0].end(), tris[2].begin(), tris[2].end());*/

	printf("%i Triangles sent to BVH\n", (int)tris[0].size());

	BVH myBVH(&(tris[0]));


	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, myBVH.GetTriangleSSBO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, myBVH.GetIndexSSBO());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, myBVH.GetNodeSSBO());

	bool keepGoing = true;

	SDL_Event e;
	while(keepGoing)
	{
		while(SDL_PollEvent(&e))
		{
			ImGui_ImplSDL2_ProcessEvent(&e);
			switch(e.type)
			{
				case SDL_QUIT:
					keepGoing = false;
					break;

				case SDL_KEYDOWN:
					if (SDL_GetRelativeMouseMode())
					{
						HandleKBInput(camera, e.key);
					}
					break;

				case SDL_MOUSEMOTION:
					if (SDL_GetRelativeMouseMode())
					{
						HandleMouseInput(camera, e.motion);
					}
					break;

				case SDL_MOUSEBUTTONDOWN:
					if (e.button.button == SDL_BUTTON_RIGHT)
					{
						SDL_SetRelativeMouseMode(SDL_TRUE);
					}
			}
		}

		float time = (float)SDL_GetTicks64() * 1000.0f;
		
		myShader.use();

		myShader.SetUniform("time", time);
		myShader.SetUniform("lights[0].position", light.position);
		camera.UpdateShader(myShader);

		_myFramework.SetGLTexture();
		
		glDispatchCompute((unsigned int)winSize.x, (unsigned int)winSize.y, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Settings");

		glm::vec3 lightPos = light.position;
		ImGui::DragFloat3("Position", &(lightPos[0]), 0.1f, -10.0f, 10.0f);
		light.position = lightPos;

		int planes = myBVH.PlaneCount();
		ImGui::DragInt("Plane Count", &planes, 1, 1, 1000);
		myBVH.PlaneCount(planes);

		ImGui::End();

		ImGui::Begin("Camera Info");

		glm::vec3 camPos = camera.Position();
		std::string l_camPosText = "Camera Position: (" + std::to_string(camPos.x) + ", " + std::to_string(camPos.y) + ", " + std::to_string(camPos.z) + ")";
		ImGui::Text(l_camPosText.c_str());

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();

		_myFramework.Show();
	}

	_myFramework.Shutdown();


    return 0;
};

void HandleKBInput(Camera& _camera, SDL_KeyboardEvent& _keyEvent)
{
	int l_key = _keyEvent.keysym.sym;
	switch (l_key)
	{
		case SDLK_w:
			_camera.Move(_camera.Forward() * (1.0f / 15.0f));
			break;

		case SDLK_s:
			_camera.Move(-_camera.Forward() * (1.0f / 15.0f));
			break;

		case SDLK_d:
			_camera.Move(_camera.Right() * (1.0f / 15.0f));
			break;

		case SDLK_a:
			_camera.Move(-_camera.Right() * (1.0f / 15.0f));
			break;

		case SDLK_e:
			_camera.Move(_camera.Up() * (1.0f / 15.0f));
			break;

		case SDLK_q:
			_camera.Move(-_camera.Up() * (1.0f / 15.0f));
			break;

		case SDLK_ESCAPE:
			SDL_SetRelativeMouseMode(SDL_FALSE);
			break;
			
	}
};

void HandleMouseInput(Camera& _camera, SDL_MouseMotionEvent& _mouseEvent)
{
	_camera.Rotate(glm::radians((float)-_mouseEvent.xrel * 3.0f), _camera.Up());
	_camera.Rotate(glm::radians((float)-_mouseEvent.yrel * 5.0f), _camera.Right());
};