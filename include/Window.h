#ifndef WINDOW_H
#define WINDOW_H

#include "GLM/glm.hpp"
#include "GL/glew.h"
#include "SDL/SDL.h"
#include <memory>

/*
*   Wrapper for an SDL Window with OpenGL context
*/
class Window
{
    private:
    glm::ivec2 m_screenSize;
    std::shared_ptr<SDL_Window> m_window;
    std::shared_ptr<SDL_Renderer> m_renderer;
    SDL_GLContext m_context;  // SDL_GLContext is already a void*, no need for shared_ptr

    GLuint m_screenTexture;

    GLuint m_screenTrianglesBuffer;
    GLuint m_screenTrianglesVAO;

    GLuint m_screenShader ;

    public:
    Window();
    Window(glm::ivec2 _size);
    ~Window();

    void Show();

    void ScreenSize(glm::ivec2 _size);
    glm::ivec2 ScreenSize();

    GLuint GetScreenTexture();
};

#endif