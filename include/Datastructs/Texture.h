#ifndef TEXTURE_H
#define TEXTURE_H

#include "GL/glew.h"
#include "GLM/ext.hpp"

#include <string>
#include <vector>

class Texture
{
protected:
	bool m_dirty;
	std::vector<unsigned char> m_data;
	glm::ivec2 m_size;
	GLuint m_id;

public:
	Texture(const std::string& _path);
	Texture(glm::ivec2 _size);
	~Texture();

	void Size(glm::ivec2 _size);
	const glm::ivec2 Size();

	virtual void Load(const std::string& _path);

	void Pixel(glm::ivec2 _position, glm::vec4& _color);
	const glm::vec4 Pixel(glm::ivec2 _position);

	GLuint GetID();
};

#endif