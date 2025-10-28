
#include "GCP_GFX_Framework.h"
#include "Camera.h"
#include "RayTracer.h"
#include "Ray.h"
#include "ComputeShader.h"

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

	ComputeShader myShader("./resources/shaders/TestComputeShader.txt");
    
    // Get shader program and set uniforms
    GLuint shaderProgram = _myFramework.GetShaderProgram(); // You'll need to add this getter to GCP_Framework
    
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
	myShader.SetUniform("numSpheres", spheres.size());
    
    for (int i = 0; i < spheres.size(); i++) {
        std::string base = "spheres[" + std::to_string(i) + "].";

		myShader.SetUniform((base + "position"), spheres[i].position);
		myShader.SetUniform((base + "radius"), spheres[i].radius);
		myShader.SetUniform((base + "color"), spheres[i].colour);
    }
    
    glUseProgram(0);

    // Pushes the framebuffer to OpenGL and renders to screen
    // Also contains an event loop that keeps the window going until it's closed

	glm::vec3 l_lightCol = glm::vec3(1.0, 1.0, 1.0);

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
		float colourAmount = (glm::sin(time) + 1.0f) * 0.5f;
		//printf("Delta: %f\n", time); 
		myShader.use();
		myShader.SetUniform("lightColor", colourAmount);

		_myFramework.SetGLTexture();

		printf("E\n");
		glDispatchCompute((unsigned int)winSize.x, (unsigned int)winSize.y, 1);
		printf("F\n");

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		_myFramework.Show();
	}


    return 0;
}
