/**
 * @file OpenGLError.h
 * @brief OpenGL debug message callback utility
 * 
 * This file provides the OpenGLError class with static methods for
 * setting up and handling OpenGL debug output messages.
 */

// Code taken from previous work
#ifndef OPENGLERROR_H
#define OPENGLERROR_H

#include <GL/glew.h>
#include <iostream>
#include <string>

/**
 * @class OpenGLError
 * @brief Static utility class for OpenGL error logging
 * 
 * Provides debug message callback functionality for OpenGL to aid
 * in debugging rendering issues and shader errors.
 */
class OpenGLError
{
public:
    /**
     * @brief OpenGL debug message callback function
     * @param _source Source of the debug message
     * @param _type Type of debug message
     * @param _id Message identifier
     * @param _severity Severity level of the message
     * @param _length Length of the message string
     * @param _message The debug message text
     * @param _userParam User-provided parameter (unused)
     * 
     * Set as a callback by glDebugMessageCallback(). Prints debug
     * information to stdout when OpenGL encounters issues.
     */
    static void DisplayDebugMessage(GLenum _source, GLenum _type, GLuint _id,
                             GLenum _severity, GLsizei _length, const GLchar* _message,
                             const void* _userParam)
    {
        printf("GL CALLBACK: from = 0x%x type = 0x%x, severity = 0x%x, message = %s\n",
               _source, _type, _severity, _message );
    }

    /**
     * @brief Initialize OpenGL debug output
     * 
     * Enables GL_DEBUG_OUTPUT and optionally sets DisplayDebugMessage
     * as the debug callback function.
     */
    static void Init()
    {
        glEnable(GL_DEBUG_OUTPUT);
        //glDebugMessageCallback(DisplayDebugMessage, 0);
        printf("OpenGLError Initialized\n");
    }
};

#endif
