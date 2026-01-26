/**
 * @file Window.h
 * @brief SDL Window with OpenGL context wrapper
 * 
 * This file provides the Window class which manages an SDL window with
 * OpenGL context, fullscreen quad rendering, and the output texture display.
 */

#ifndef WINDOW_H
#define WINDOW_H

#include "GLM/glm.hpp"
#include "GL/glew.h"
#include "SDL/SDL.h"
#include <memory>

/**
 * @class Window
 * @brief Wrapper for an SDL Window with OpenGL context
 * 
 * Manages window creation, OpenGL context setup, and rendering the final
 * raytraced image to the screen via a fullscreen quad.
 */
class Window
{
private:
    /** @brief Window dimensions (width, height) */
    glm::ivec2 m_screenSize;
    
    /** @brief SDL window handle */
    std::shared_ptr<SDL_Window> m_window;
    
    /** @brief SDL renderer handle */
    std::shared_ptr<SDL_Renderer> m_renderer;
    
    /** @brief OpenGL context (SDL_GLContext is already a void*) */
    SDL_GLContext m_context;

    /** @brief Texture ID for the raytracer output */
    GLuint m_screenTexture;

    /** @brief VBO for fullscreen triangle vertices */
    GLuint m_screenTrianglesBuffer;
    
    /** @brief VAO for fullscreen triangle rendering */
    GLuint m_screenTrianglesVAO;

    /** @brief Shader program for displaying the screen texture */
    GLuint m_screenShader;

public:
    /**
     * @brief Default constructor
     * 
     * Creates a window with default size.
     */
    Window();
    
    /**
     * @brief Construct a window with specified size
     * @param _size Window dimensions (width, height)
     */
    Window(glm::ivec2 _size);
    
    /** @brief Destructor - releases SDL and OpenGL resources */
    ~Window();

    /**
     * @brief Render and display the current frame
     * 
     * Draws the raytracer output texture to the screen using a fullscreen quad.
     */
    void Show();

    /**
     * @brief Set the window/render size
     * @param _size New dimensions (width, height)
     */
    void ScreenSize(glm::ivec2 _size);
    
    /**
     * @brief Get the current window/render size
     * @return Current dimensions (width, height)
     */
    glm::ivec2 ScreenSize();

    /**
     * @brief Get the screen texture ID for raytracer output
     * @return OpenGL texture ID
     */
    GLuint GetScreenTexture();
};

#endif