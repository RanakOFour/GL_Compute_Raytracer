#include "Window.h"

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_sdl.h"
#include "IMGUI/imgui_impl_opengl3.h"

#include <iostream>
#include <fstream>
#include <stdexcept>

void CreateTriangleVAO(GLuint& _vao, GLuint& _buffer)
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	float vertices[] = {
		 -1.0f, -1.0f,
		  1.0f, -1.0f,
		 -1.0f,  1.0f,

		  1.0f, -1.0f,
		  1.0f,  1.0f,
		 -1.0f,  1.0f
	};

	glGenBuffers(1, &_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
}

bool CheckShaderCompiled(GLint shader)
{
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLsizei len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

		// OpenGL will store an error message as a string that we can retrieve and print
		GLchar* log = new GLchar[len + 1];
		glGetShaderInfoLog(shader, len, &len, log);
		std::cerr << "ERROR: Shader compilation failed: " << log << std::endl;
		delete[] log;

		return false;
	}
	return true;
}


// Loads shaders from file and returns shader program
GLuint LoadShaders(std::string vertFilename, std::string fragFilename)
{
	std::ifstream vertFile(vertFilename);
	char* vShaderText = NULL;

	if (vertFile.is_open())
	{
		vertFile.seekg(0, vertFile.end);
		int length = (int)vertFile.tellg();
		vertFile.seekg(0, vertFile.beg);

		vShaderText = new char[length + 1];

		vertFile.read(vShaderText, length);

		length = (int)vertFile.gcount();

		vShaderText[length] = 0;

		vertFile.close();
	}
	else
	{
		std::cerr << "WARNING: could not open vertex shader from file: " << vertFilename << std::endl;
		return false;
	}


	std::ifstream fragFile(fragFilename);
	char* fShaderText = NULL;

	if (fragFile.is_open())
	{
		fragFile.seekg(0, fragFile.end);
		int length = (int)fragFile.tellg();
		fragFile.seekg(0, fragFile.beg);

		fShaderText = new char[length + 1];

		fragFile.read(fShaderText, length);
		length = (int)fragFile.gcount();

		fShaderText[length] = 0;

		fragFile.close();
	}
	else
	{
		std::cerr << "WARNING: could not open fragment shader from file: " << fragFilename << std::endl;
		return false;
	}

	GLuint l_screenShader = glCreateProgram();
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    

	glShaderSource(vShader, 1, &vShaderText, NULL);
	// Delete buffer
	delete[] vShaderText;
	// Compile the shader
	glCompileShader(vShader);
	// Check it compiled and give useful output if it didn't work!
	if (!CheckShaderCompiled(vShader))
	{
		std::cerr << "ERROR: failed to compile vertex shader" << std::endl;
		return 0;
	}
	// This links the shader to the program
	glAttachShader(l_screenShader, vShader);

	// Same for the fragment shader
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fShaderText, NULL);
	// Delete buffer
	delete[] fShaderText;
	glCompileShader(fShader);
	if (!CheckShaderCompiled(fShader))
	{
		std::cerr << "ERROR: failed to compile fragment shader" << std::endl;
		return 0;
	}
	glAttachShader(l_screenShader, fShader);

	// This makes sure the vertex and fragment shaders connect together
	glLinkProgram(l_screenShader);
	// Check this worked
	GLint linked;
	glGetProgramiv(l_screenShader, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLsizei len;
		glGetProgramiv(l_screenShader, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = new GLchar[len + 1];
		glGetProgramInfoLog(l_screenShader, len, &len, log);
		std::cerr << "ERROR: Shader linking failed: " << log << std::endl;
		delete[] log;

		return 0;
	}

	printf("Shader compiled\n");

	return l_screenShader;
}

Window::Window(glm::ivec2 _screenSize, glm::ivec2 _renderSize)
: m_screenSize(_screenSize)
, m_renderSize(_renderSize)
, m_screenTrianglesBuffer(0)
, m_screenTrianglesVAO(0)
, m_screenShader(0)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        throw std::runtime_error("Failed to initialize SDL");
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* l_window = SDL_CreateWindow("GL Tracer", 0, 0, m_screenSize.x, m_screenSize.y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
    m_window = std::shared_ptr<SDL_Window>(l_window, SDL_DestroyWindow);
    
    if (!m_window)
    {
        throw std::runtime_error("Failed to create SDL window");
    }

    SDL_Renderer* l_renderer = SDL_CreateRenderer(m_window.get(), -1, SDL_RENDERER_ACCELERATED);
    m_renderer = std::shared_ptr<SDL_Renderer>(l_renderer, SDL_DestroyRenderer);

    if(!m_renderer)
    {
        throw std::runtime_error("Failed to create SDL renderer");
    }

    m_context = SDL_GL_CreateContext(m_window.get());

    if (!m_context)
    {
        throw std::runtime_error("Failed to create OpenGL context");
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        throw std::runtime_error("Failed to initialize GLEW");
    }

	glGenTextures(1, &m_screenTexture);

	glBindTexture(GL_TEXTURE_2D, m_screenTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_renderSize.x, m_renderSize.y, 0, GL_RGBA, GL_FLOAT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	const char* glslVersion = "#version 130";
	ImGui_ImplSDL2_InitForOpenGL(m_window.get(), m_context);
	ImGui_ImplOpenGL3_Init(glslVersion);

    CreateTriangleVAO(m_screenTrianglesVAO, m_screenTrianglesBuffer);
	m_screenShader = LoadShaders("./resources/shaders/Framebuffer/ScreenVertex.txt", "./resources/shaders/Framebuffer/ScreenFragment.txt");

    glEnable(GL_DEPTH_TEST);
}

Window::~Window()
{
    // Clean up ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    // Clean up OpenGL resources while context is still valid
    if (m_screenShader != 0)
    {
        glDeleteProgram(m_screenShader);
    }
    if (m_screenTrianglesVAO != 0)
    {
        glDeleteVertexArrays(1, &m_screenTrianglesVAO);
    }
    if (m_screenTrianglesBuffer != 0)
    {
        glDeleteBuffers(1, &m_screenTrianglesBuffer);
    }
    if (m_screenTexture != 0)
    {
        glDeleteTextures(1, &m_screenTexture);
    }

    // Delete GL context before window (must be done explicitly)
    if (m_context)
    {
        SDL_GL_DeleteContext(m_context);
        m_context = nullptr;
    }

    // shared_ptr will clean up renderer and window automatically
}

void Window::Show()
{
    // Specify the colour to clear the framebuffer to
	glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
	// This writes the above colour to the colour part of the framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Binds OpenGL Texture
	glUseProgram(m_screenShader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_screenTexture);

	glUseProgram(m_screenShader);
		glBindVertexArray(m_screenTrianglesVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	glUseProgram(0);

	// Render GUI to screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// This tells the renderer to actually show its contents to the screen
	SDL_GL_SwapWindow(m_window.get());
}

void Window::ScreenSize(glm::ivec2 _size)
{
    m_screenSize = _size;
}

glm::ivec2 Window::ScreenSize()
{
    return m_screenSize;
}

GLuint Window::GetScreenTexture()
{
    return m_screenTexture;
}