#include "GCP_GFX_Framework.h"

#include "GL/glew.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "OpenGLError.h"

#include <iostream>
#include <fstream>

#if _WIN32

#else
#include <error.h>
#endif

// Handles local (CPU side) and OpenGL framebuffer functionality
class Framebuffer
{
	public:
	Framebuffer(unsigned int w, unsigned int h)
	{
		_width = w; _height = h;

		GenLocalFramebuffer();
		GenGLFramebuffer();
	};

	~Framebuffer()
	{
		glDeleteTextures(1, &_glTexName);
		delete[] _localBuffer;
	};

	// Requires colours between 0 and 1 per RGB channel
	void DrawPixel(glm::ivec2 position, glm::vec3 colour);

	void SetAllPixels(glm::vec3 colour);

	// Sends local framebuffer copy to OpenGL texture
	void UpdateGL();

	// Binds the OpenGL texture for use with rendering it to screen
	void BindGLTex();

	void BindGLImage();
	
	protected:
	unsigned int _glTexName = 0;

	unsigned int _width = 0;
	unsigned int _height = 0;

	// The CPU side framebuffer
	glm::vec3* _localBuffer = nullptr;

	void GenLocalFramebuffer();

	void GenGLFramebuffer();
};

// An initialisation function, mainly for GLEW
// This will also print to console the version of OpenGL we are using
bool InitGL()
{
	// GLEW has a problem with loading core OpenGL
	// See here: https://www.opengl.org/wiki/OpenGL_Loading_Library
	// The temporary workaround is to enable its 'experimental' features
	glewExperimental = GL_TRUE;

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		std::cerr << "Error: GLEW failed to initialise with message: " << glewGetErrorString(err) << std::endl << "Err num: " << err << std::endl;
		return false;
	}

	std::cout << "INFO: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

	std::cout << "INFO: OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "INFO: OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "INFO: OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	return true;
}


// This creates a Vertex Array Object for a single triangle
// The VAO stores one or more Vertex Buffer Objects
// The VBOs store the actual vertex data (e.g. one each for positions, colours, texture coords etc)
// The VAO tells the server how to actually interpret and use the VBO data
GLuint CreateTriangleVAO()
{
	// Variable for storing our VAO
	// OpenGL has its own defined datatypes - a 'GLuint' is basically an unsigned int
	GLuint VAO = 0;
	// Creates one VAO
	glGenVertexArrays(1, &VAO);
	// 'Binding' something makes it the current one we are using
	// This is like activating it, so that subsequent function calls will work on this item
	glBindVertexArray(VAO);

	// Simple vertex data for a triangle
	// OpenGL is happy for us to work with 2D coordinates if we want
	float vertices[] = {
		 -1.0f, -1.0f,
		  1.0f, -1.0f,
		 -1.0f,  1.0f,

		  1.0f, -1.0f,
		  1.0f,  1.0f,
		 -1.0f,  1.0f
	};

	// Variable for storing a VBO
	GLuint buffer = 0;
	// Create a generic 'buffer'
	glGenBuffers(1, &buffer);
	// Tell OpenGL that we want to activate the buffer and that it's a VBO
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	// With this buffer active, we can now send our data to OpenGL
	// We need to tell it how much data to send
	// We can also tell OpenGL how we intend to use this buffer - here we say GL_STATIC_DRAW because we're only writing it once
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, vertices, GL_STATIC_DRAW);

	// This tells OpenGL how we link the vertex data to the shader
	// (We will look at this properly in the lectures)
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);


	// Unbind for neatness, it just makes life easier
	// As a general tip, especially as you're still learning, for each function that needs to do something specific try to return OpenGL in the state you found it in
	// This means you will need to set the states at the beginning of your function and set them back at the end
	// If you don't do this, your function could rely on states being set elsewhere and it's easy to lose track of this as your project grows
	// If you then change the code from elsewhere, your current code could mysteriously stop working properly!
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Technically we can do this, because the enabled / disabled state is stored in the VAO
	glDisableVertexAttribArray(0);

	return VAO;
}

// Draws the VAO
void DrawVAOTris(GLuint VAO, int numVertices, GLuint shaderProgram)
{
	// Ok, here I like to indent drawing calls - it's just a personal style, you may not like it and that's fine ;)
	// Generally you will need to be activating and deactivating OpenGL states
	// I just find it visually easier if the activations / deactivations happen at different tab depths
	// This can help when things get more complex

	// Activate the shader program
	glUseProgram(shaderProgram);
		// Activate the VAO
		glBindVertexArray(VAO);
			// Tell OpenGL to draw it
			// Must specify the type of geometry to draw and the number of vertices
			glDrawArrays(GL_TRIANGLES, 0, numVertices);
		// Unbind VAO
		glBindVertexArray(0);
	// Technically we can do this, but it makes no real sense because we must always have a valid shader program to draw geometry
	glUseProgram(0);

	printf("Drawn triangles\n");
}

// Useful little function to just check for compiler errors
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
	// OpenGL doesn't provide any functions for loading shaders from file


	std::ifstream vertFile(vertFilename);
	char* vShaderText = NULL;

	if (vertFile.is_open())
	{
		// Find out how many characters are in the file
		vertFile.seekg(0, vertFile.end);
		int length = (int)vertFile.tellg();
		vertFile.seekg(0, vertFile.beg);

		// Create our buffer
		vShaderText = new char[length + 1];

		// Transfer data from file to buffer
		vertFile.read(vShaderText, length);

		// Check it reached the end of the file
		// if (!vertFile.eof())
		// {
		// 	vertFile.close();
		// 	std::cerr << "WARNING: could not read vertex shader from file: " << vertFilename << std::endl;
		// 	return false;
		// }

		// Find out how many characters were actually read
		length = (int)vertFile.gcount();

		// Needs to be NULL-terminated
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
		// Find out how many characters are in the file
		fragFile.seekg(0, fragFile.end);
		int length = (int)fragFile.tellg();
		fragFile.seekg(0, fragFile.beg);

		// Create our buffer
		fShaderText = new char[length + 1];

		// Transfer data from file to buffer
		fragFile.read(fShaderText, length);

		// Check it reached the end of the file
		// if (!fragFile.eof())
		// {
		// 	fragFile.close();
		// 	std::cerr << "WARNING: could not read fragment shader from file: " << fragFilename << std::endl;
		// 	return false;
		// }

		// Find out how many characters were actually read
		length = (int)fragFile.gcount();

		// Needs to be NULL-terminated
		fShaderText[length] = 0;

		fragFile.close();
	}
	else
	{
		std::cerr << "WARNING: could not open fragment shader from file: " << fragFilename << std::endl;
		return false;
	}



	// The 'program' stores the shaders
	GLuint _shaderProgram = glCreateProgram();

	// Create the vertex shader
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	// Give GL the source for it
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
	glAttachShader(_shaderProgram, vShader);

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
	glAttachShader(_shaderProgram, fShader);

	// This makes sure the vertex and fragment shaders connect together
	glLinkProgram(_shaderProgram);
	// Check this worked
	GLint linked;
	glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLsizei len;
		glGetProgramiv(_shaderProgram, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = new GLchar[len + 1];
		glGetProgramInfoLog(_shaderProgram, len, &len, log);
		std::cerr << "ERROR: Shader linking failed: " << log << std::endl;
		delete[] log;

		return 0;
	}

	printf("Shader compiled\n");

	return _shaderProgram;
}

// Sets up SDL, OpenGL, framebuffer
bool GCP_Framework::Init( glm::ivec2 screenSize )
{
	_screenSize = screenSize;

	// SDL_Init is the main initialisation function for SDL
	// It takes a 'flag' parameter which we use to tell SDL what systems we're going to use
	// Here, we want to use SDL's video system, so we give it the flag for this
	// Incidentally, this also initialises the input event system
	// This function also returns an error value if something goes wrong
	// So we can put this straight in an 'if' statement to check and exit if need be
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		// Something went very wrong in initialisation, all we can do is exit
		std::cout << "Whoops! Something went very wrong, cannot initialise SDL :(" << std::endl;
		return false;
	}

	// This is how we set the context profile
	// We need to do this through SDL, so that it can set up the OpenGL drawing context that matches this
	// (of course in reality we have no guarantee this will be available and should provide fallback methods if it's not!)
	// Anyway, we basically first say which version of OpenGL we want to use
	// So let's say 4.3:
	// Major version number (4):
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	// Minor version number (3):
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// Then we say whether we want the core profile or the compatibility profile
	// Flag options are either: SDL_GL_CONTEXT_PROFILE_CORE   or   SDL_GL_CONTEXT_PROFILE_COMPATIBILITY
	// We'll go for the core profile
	// This means we are using the latest version and cannot use the deprecated functions
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// Now we have got SDL initialised, we are ready to create a window!
	// These are some variables to help show you what the parameters are for this function
	// You can experiment with the numbers to see what they do
	int winPosX = 100;
	int winPosY = 100;
	int winWidth = _screenSize.x;
	int winHeight = _screenSize.y;
	_SDLwindow = SDL_CreateWindow("My Window!!!",  // The first parameter is the window title
		winPosX, winPosY,
		winWidth, winHeight,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	// The last parameter lets us specify a number of options
	// Here, we tell SDL that we want the window to be shown and that it can be resized
	// You can learn more about SDL_CreateWindow here: https://wiki.libsdl.org/SDL_CreateWindow?highlight=%28\bCategoryVideo\b%29|%28CategoryEnum%29|%28CategoryStruct%29
	// The flags you can pass in for the last parameter are listed here: https://wiki.libsdl.org/SDL_WindowFlags

	// The SDL_CreateWindow function returns an SDL_Window
	// This is a structure which contains all the data about our window (size, position, etc)
	// We will also need this when we want to draw things to the window
	// This is therefore quite important we don't lose it!

	// The SDL_Renderer is a structure that handles rendering
	// It will store all of SDL's internal rendering related settings
	// When we create it we tell it which SDL_Window we want it to render to
	// That renderer can only be used for this window
	// (yes, we can have multiple windows - feel free to have a play sometime)
	SDL_Renderer* renderer = SDL_CreateRenderer(_SDLwindow, -1, 0);


	// Now that the SDL renderer is created for the window, we can create an OpenGL context for it!
	// This will allow us to actually use OpenGL to draw to the window
	_SDLglcontext = SDL_GL_CreateContext(_SDLwindow);

	// Call our initialisation function to set up GLEW and print out some GL info to console
	if (!InitGL())
	{
		return false;
	}

	OpenGLError::Init();

	// Setting up the GUI system
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	const char* glslVersion = "#version 130";
	ImGui_ImplSDL2_InitForOpenGL(_SDLwindow, _SDLglcontext);
	ImGui_ImplOpenGL3_Init(glslVersion);

	// Create the vertex array object for our triangle
	_triangleVAO = CreateTriangleVAO();

	// Create the shaders and link them together into the shader program
	_shaderProgram = LoadShaders("./resources/shaders/ScreenVertex.txt", "./resources/shaders/ScreenFragment.txt");

	_mainBuffer = new Framebuffer(winWidth, winHeight);

	_mainBuffer->SetAllPixels(glm::vec3(0, 0, 0));

	glEnable(GL_DEPTH_TEST);

	return true;
}


void GCP_Framework::SetAllPixels(glm::vec3 pixelColour)
{
	// sanity check that Init() has been called
	assert(_mainBuffer != nullptr);

	_mainBuffer->SetAllPixels(pixelColour);
}

void GCP_Framework::DrawPixel(glm::ivec2 pixelPosition, glm::vec3 pixelColour)
{
	// sanity check that Init() has been called
	assert(_mainBuffer != nullptr);

	_mainBuffer->DrawPixel(pixelPosition, pixelColour);
}

void GCP_Framework::Show()
{
	// sanity check that Init() has been called
	assert(_mainBuffer != nullptr);
	// Show

	// Specify the colour to clear the framebuffer to
	glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
	// This writes the above colour to the colour part of the framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Send offline framebuffer to the OpenGL texture
	//_mainBuffer->UpdateGL();

	// Binds OpenGL Texture
	glUseProgram(_shaderProgram);
	_mainBuffer->BindGLTex();

	// Call our drawing function to draw that triangle!
	DrawVAOTris(_triangleVAO, 6, _shaderProgram);

	// Render GUI to screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// This tells the renderer to actually show its contents to the screen
	SDL_GL_SwapWindow(_SDLwindow);
}

void GCP_Framework::SetGLTexture()
{
	_mainBuffer->BindGLImage();
}

GCP_Framework::~GCP_Framework()
{
	delete _mainBuffer;

	if(_SDLglcontext)
	{
		SDL_GL_DeleteContext(_SDLglcontext);
	}

	if(_SDLwindow)
	{
		SDL_DestroyWindow(_SDLwindow);
	}
	
	SDL_Quit();


	// TODO: currently doesn't clean up VAO or VBO
}

void GCP_Framework::Shutdown()
{
	SDL_GL_DeleteContext(_SDLglcontext);
	SDL_DestroyWindow(_SDLwindow);
	SDL_Quit();
}


void Framebuffer::DrawPixel(glm::ivec2 position, glm::vec3 colour)
{
	position = glm::clamp(position, glm::ivec2(0), glm::ivec2(_width, _height));
	colour = glm::clamp(colour, 0.0f, 1.0f);

	// Store in local memory only, only send to OpenGL when we've got all pixel draw calls finished
	_localBuffer[position.y * _width + position.x] = colour;
}

void Framebuffer::SetAllPixels(glm::vec3 colour)
{
	colour = glm::clamp(colour, 0.0f, 1.0f);

	for (unsigned int i = 0; i < _width * _height; ++i)
	{
		_localBuffer[i] = colour;
	}
}

void Framebuffer::UpdateGL()
{
	// Send offline framebuffer to the OpenGL texture
	glBindTexture(GL_TEXTURE_2D, _glTexName);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _width, _height, 0, GL_RGBA, GL_FLOAT, _localBuffer);

}

void Framebuffer::BindGLTex()
{
	glBindTexture(GL_TEXTURE_2D, _glTexName);
}

void Framebuffer::BindGLImage()
{
	glBindImageTexture(0, _glTexName, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

void Framebuffer::GenLocalFramebuffer()
{
	_localBuffer = new glm::vec3[_width * _height];
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
