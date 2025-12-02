#include "Framebuffer.h"

void Framebuffer::BindGLTex()
{
	glBindTexture(GL_TEXTURE_2D, _glTexName);
}

void Framebuffer::BindGLImage()
{
	glBindImageTexture(0, _glTexName, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

void Framebuffer::GenGLFramebuffer()
{
	// Create OpenGL texture
	glGenTextures(1, &_glTexName);

	glBindTexture(GL_TEXTURE_2D, _glTexName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _width, _height, 0, GL_RGBA, GL_FLOAT, 0);
}
