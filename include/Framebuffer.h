#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "GL/glew.h"

class Framebuffer
{
	public:
	Framebuffer(unsigned int w, unsigned int h)
	{
		_width = w; _height = h;
		GenGLFramebuffer();
	};

	~Framebuffer()
	{
		glDeleteTextures(1, &_glTexName);
	};

	// Binds the OpenGL texture for use with rendering it to screen
	void BindGLTex();

	void BindGLImage();
	
	protected:
	unsigned int _glTexName = 0;

	unsigned int _width = 0;
	unsigned int _height = 0;

	void GenGLFramebuffer();
};

#endif