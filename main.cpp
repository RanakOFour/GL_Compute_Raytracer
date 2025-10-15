
#include "GCP_GFX_Framework.h"
#include "Camera.h"
#include "RayTracer.h"
#include "Ray.h"
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
    
    // Get shader program and set uniforms
    GLuint shaderProgram = _myFramework.GetShaderProgram(); // You'll need to add this getter to GCP_Framework
    
    glUseProgram(shaderProgram);
    
    // Set camera uniforms
    glm::vec3 camPos(0, 0, 3);
    glm::vec3 camForward(0, 0, -1);
    glm::vec3 camRight(1, 0, 0);
    glm::vec3 camUp(0, 1, 0);
    
    GLuint camPosLoc = glGetUniformLocation(shaderProgram, "cameraPos");
    GLuint camForwardLoc = glGetUniformLocation(shaderProgram, "cameraForward");
    GLuint camRightLoc = glGetUniformLocation(shaderProgram, "cameraRight");
    GLuint camUpLoc = glGetUniformLocation(shaderProgram, "cameraUp");
    GLuint fovLoc = glGetUniformLocation(shaderProgram, "fov");
    GLuint resLoc = glGetUniformLocation(shaderProgram, "resolution");
    
    glUniform3f(camPosLoc, camPos.x, camPos.y, camPos.z);
    glUniform3f(camForwardLoc, camForward.x, camForward.y, camForward.z);
    glUniform3f(camRightLoc, camRight.x, camRight.y, camRight.z);
    glUniform3f(camUpLoc, camUp.x, camUp.y, camUp.z);
    glUniform1f(fovLoc, 0.5f); // Adjust FOV as needed
    glUniform2f(resLoc, winSize.x, winSize.y);
    
    // Set sphere uniforms
    GLuint numSpheresLoc = glGetUniformLocation(shaderProgram, "numSpheres");
    glUniform1i(numSpheresLoc, spheres.size());
    
    for (int i = 0; i < spheres.size(); i++) {
        std::string base = "spheres[" + std::to_string(i) + "].";
        
        GLuint posLoc = glGetUniformLocation(shaderProgram, (base + "position").c_str());
        GLuint radiusLoc = glGetUniformLocation(shaderProgram, (base + "radius").c_str());
        GLuint colorLoc = glGetUniformLocation(shaderProgram, (base + "color").c_str());
        
        glUniform3f(posLoc, spheres[i].position.x, spheres[i].position.y, spheres[i].position.z);
        glUniform1f(radiusLoc, spheres[i].radius);
        glUniform3f(colorLoc, spheres[i].colour.x, spheres[i].colour.y, spheres[i].colour.z);
    }
    
    glUseProgram(0);

    // Pushes the framebuffer to OpenGL and renders to screen
    // Also contains an event loop that keeps the window going until it's closed

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
		//printf("Delta: %f\n", time); 
		glUseProgram(shaderProgram);
		
		for (int i = 0; i < spheres.size(); i++) 
		{
			std::string base = "spheres[" + std::to_string(i) + "].";
			
			GLuint colorLoc = glGetUniformLocation(shaderProgram, (base + "color").c_str());
		}

		glUseProgram(0);

		_myFramework.Show();
	}


    return 0;


	// Old CPU tracing
	// Sets all pixels the same colour
	// _myFramework.SetAllPixels( glm::vec3(0.1f,0.1f,0.3f) );

	// RayTracer rayTracer;
	// Camera camera;
	// Ray r;
	// glm::ivec2 pixelPosition;
	// glm::vec3 pixelColour;
	// for(int i = 0; i < 480; ++i)
	// {
	// 	pixelPosition.y = i;
	// 	for(int j = 0; j < 640; ++j)
	// 	{
	// 		pixelPosition.x = j;
	// 		r = camera.GetRay(pixelPosition);
	// 		pixelColour = rayTracer.TraceRay(&r);

	// 		_myFramework.DrawPixel(pixelPosition, pixelColour);
	// 	}
	// }

	// // Pushes the framebuffer to OpenGL and renders to screen
	// // Also contains an event loop that keeps the window going until it's closed
	// _myFramework.ShowAndHold();
	// return 0;

}
