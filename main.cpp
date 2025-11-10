
#include "GCP_GFX_Framework.h"
#include "Camera.h"
#include "RayTracer.h"
#include "Light.h"
#include "ComputeShader.h"
#include "Cube.h"

#include "GL/glew.h"

int main(int argc, char* argv[])
{
	// Set window size
	glm::ivec2 winSize(640, 480);

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
    Camera camera;

	ComputeShader myShader("./resources/shaders/RTCompute.txt");
    
    myShader.use();
    
    // Set camera uniforms
    glm::vec3 camPos(0, 0, 3);
    glm::vec3 camForward(0, 0, -1);
    glm::vec3 camRight(1, 0, 0);
    glm::vec3 camUp(0, 1, 0);

	myShader.SetUniform("cameraPos", camPos);
	myShader.SetUniform("cameraForward", camForward);
	myShader.SetUniform("cameraRight", camRight);
	myShader.SetUniform("cameraUp", camUp);
	myShader.SetUniform("fov", 0.5f);
	myShader.SetUniform("resolution", winSize);
    
    // Set sphere uniforms
	myShader.SetUniform("numSpheres", (int)spheres.size());
    
    for (int i = 0; i < spheres.size(); i++) {
        std::string base = "spheres[" + std::to_string(i) + "].";

		myShader.SetUniform((base + "position"), spheres[i].position);
		myShader.SetUniform((base + "radius"), spheres[i].radius);
		myShader.SetUniform((base + "color"), spheres[i].colour);
    }

	Cube floor;
	floor.position = glm::vec3(0.0f, -0.5f, 0.0f);
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

	bool keepGoing = true;
	SDL_Event e;
	while(keepGoing)
	{
		while(SDL_PollEvent(&e))
		{
			switch(e.type)
			{
				case SDL_QUIT:
					keepGoing = false;
					break;
			}
		}

		float time = (float)SDL_GetTicks64() * 0.001f;
		
		myShader.use();

		light.position = glm::vec3(3.0f * glm::sin(time), 0.0f, 3.0f * glm::cos(time));
		myShader.SetUniform("lights[0].position", light.position);

		_myFramework.SetGLTexture();

		glDispatchCompute((unsigned int)winSize.x, (unsigned int)winSize.y, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		_myFramework.Show();
	}


    return 0;
}
