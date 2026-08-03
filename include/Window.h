#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "GL/glew.h"
#include "GLM/ext.hpp"
#include "SDL/SDL.h"

#include <memory>

class TextureBuffer;
class Window
{
private:
	glm::vec2 m_screenSize;
	std::shared_ptr<SDL_Window> m_sdlWindow;
	std::shared_ptr<SDL_Renderer> m_renderer;
	SDL_GLContext m_sdlglContext;

	std::shared_ptr<TextureBuffer> m_outputBuffer;

	GLuint m_screenTriVAO;
	GLuint m_screenTriBuffer;
	GLuint m_screenShaderId;

public:
	Window();
	~Window();

	void SetOutputBuffer(std::shared_ptr<TextureBuffer> _buffer);
	void Show();
};

#endif
