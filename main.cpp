
#include "GCP_GFX_Framework.h"
#include "Camera.h"
#include "Light.h"
#include "ComputeShader.h"
#include "Sphere.h"
#include "Cube.h"

#include "GL/glew.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <vector>

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

	std::vector<Sphere> spheres;
    spheres.push_back(Sphere(glm::vec3(0, 0, 0), 0.5f, glm::vec3(1, 0, 0)));    // Red sphere
    spheres.push_back(Sphere(glm::vec3(1, 0, 0), 0.3f, glm::vec3(0, 1, 0)));    // Green sphere
    spheres.push_back(Sphere(glm::vec3(-1, 0, 0), 0.4f, glm::vec3(0, 0, 1)));   // Blue sphere
    
    // Camera setup
    Camera camera(glm::vec2(400 * 16.0f/9.0f, 400));

	ComputeShader myShader("./resources/shaders/RTCompute.txt");
    
    myShader.use();
    
    // Set camera uniforms
    camera.SetShaderValues(&myShader);
    
    // Set sphere uniforms
	myShader.SetUniform("numSpheres", (int)spheres.size());
    
    for (int i = 0; i < spheres.size(); i++) {
        std::string base = "spheres[" + std::to_string(i) + "].";

		myShader.SetUniform((base + "position"), spheres[i].position);
		myShader.SetUniform((base + "radius"), spheres[i].radius);
		myShader.SetUniform((base + "color"), spheres[i].colour);
    }

	Cube floor;
	floor.position = glm::vec3(0.0f, -0.75f, 0.0f);
	floor.size = glm::vec3(5.0f, 0.5, 5.0f);
	floor.color = glm::vec3(1.0f, 1.0f, 1.0f);

	myShader.SetUniform("numCubes", 1);
	myShader.SetUniform("cubes[0].position", floor.position);
	myShader.SetUniform("cubes[0].size", floor.size);
	myShader.SetUniform("cubes[0].color", floor.color);

    // Pushes the framebuffer to OpenGL and renders to screen
    // Also contains an event loop that keeps the window going until it's closed

	Light light;
	light.position = glm::vec3(0.0f, 0.0f, 3.0f);
	light.colour = glm::vec3(1.0f, 1.0f, 1.0f);

	myShader.SetUniform("numLights", 1);
	myShader.SetUniform("lights[0].position", light.position);
	myShader.SetUniform("lights[0].color", light.colour);

	float sampleCount = 1;
	myShader.SetUniform("sampleCount", sampleCount);

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
			}
		}

		float time = (float)SDL_GetTicks64() * 1000.0f;
		
		myShader.use();

		myShader.SetUniform("sampleCount", sampleCount);
		myShader.SetUniform("time", time);
		myShader.SetUniform("lights[0].position", light.position);
		myShader.SetUniform("u_camera.position", camera.Position());
		myShader.SetUniform("u_camera.focalLength", camera.FocalLength());

		_myFramework.SetGLTexture();
		
		glDispatchCompute((unsigned int)winSize.x, (unsigned int)winSize.y, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Light Settings");

		glm::vec3 lightPos = light.position;
		ImGui::DragFloat3("Position", &(lightPos[0]), 0.1f, -10.0f, 10.0f);
		light.position = lightPos;

		float s3amples = sampleCount;
		ImGui::DragFloat("Sample Count", &s3amples, 1.0f, 0.0f, 20.0f);
		sampleCount = s3amples;

		ImGui::End();

		ImGui::Begin("Camera Settings");

		glm::vec3 camPos = camera.Position();
		ImGui::DragFloat3("Position", &(camPos[0]), 0.1f, -10.0f, 10.0f);
		camera.Position(camPos);

		float fl = camera.FocalLength();
		ImGui::DragFloat("Focal Length", &(fl), 0.1f, -10.0f, 10.0f);
		camera.FocalLength(fl);



		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();

		_myFramework.Show();
	}

	_myFramework.Shutdown();


    return 0;
}
