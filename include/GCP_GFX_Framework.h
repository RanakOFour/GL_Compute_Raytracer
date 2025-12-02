#ifndef GCP_GFX_FRAMEWORK_H
#define GCP_GFX_FRAMEWORK_H

#include <string>

#include "SDL/SDL.h"
#include "GLM/glm.hpp"
#include "GL/glew.h"

// Forward declaration of internal utility class to handle framebuffer functionality
class Framebuffer;

// Main interface for the framework
// Must call Init() before other functions
class GCP_Framework
{
public:
	GCP_Framework(glm::ivec2 _screenSize);
	~GCP_Framework();

	// Must call Init after creation
	// Sets up SDL, OpenGL and the internal framebuffer
	bool Init( glm::ivec2 screenSize );

	// Draws the buffer on OpenGL
	void Show();

	void Shutdown();

	void SwapBuffer()
	{
		SDL_GL_SwapWindow(m_SDLwindow);
	}

protected:

	// Internal variables
		Framebuffer* m_mainBuffer = nullptr;
		glm::ivec2 m_screenSize;

	// SDL variables
		SDL_Window* m_SDLwindow = nullptr;
		SDL_GLContext m_SDLglcontext;

	// OpenGL drawing variables
		GLuint m_screenTrianglesVAO = 0;
		GLuint m_screenShader = 0;
};


#endif