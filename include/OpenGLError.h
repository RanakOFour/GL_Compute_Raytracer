// Code taken from previous work
#ifndef OPENGLERROR_H

#define OPENGLERROR_H

#include <GL/glew.h>
#include <iostream>
#include <string>

/*
*	Error logging static for OpenGL
*/
class OpenGLError
{
public:

	/*
		Set as a callback by glDebugMessageCallback(), which just prints debug info when necessary
	*/
	static void DisplayDebugMessage(GLenum _source, GLenum _type, GLuint _id,
							 GLenum _severity, GLsizei _length, const GLchar* _message,
							 const void* _userParam)
	{
		printf("GL CALLBACK: from = 0x%x type = 0x%x, severity = 0x%x, message = %s\n",
			   _source, _type, _severity, _message );
		//std::cin.get();
	}

	/*
	*	Sets OpenGLError::DisplayDebugMessage() as the debug message callback
	*/
	static void Init()
	{
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(DisplayDebugMessage, 0);
		printf("OpenGLError Initialized\n");
	}
};
 

#endif
