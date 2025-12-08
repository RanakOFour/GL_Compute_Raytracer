#include "GCP_GFX_Framework.h"
#include "Framebuffer.h"

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

GCP_Framework::GCP_Framework(glm::ivec2 _screenSize)
{
	Init(_screenSize);
}

// An initialisation function, mainly for GLEW
// This will also print to console the version of OpenGL we are using
bool InitGL()
{
	// GLEW has a problem with loading core OpenGL
	// See here: https://www.opengl.org/wiki/OpenGL_Loading_Library
	// The temporary workaround is to enable its 'experimental' features
	glewExperimental = GL_TRUE; // This is important for modern OpenGL
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		printf("GLEW Error: %s\n", glewGetErrorString(glewError));
		return -1;
	}

	printf("glewInit() res: %i\n", (int)glewError);

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
		glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, numVertices);
		glBindVertexArray(0);
	glUseProgram(0);
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

		// Owen: On my machine this check always returns true no matter what

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

		// Owen: On my machine this check always returns true no matter what

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
	GLuint m_screenShader = glCreateProgram();

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
	glAttachShader(m_screenShader, vShader);

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
	glAttachShader(m_screenShader, fShader);

	// This makes sure the vertex and fragment shaders connect together
	glLinkProgram(m_screenShader);
	// Check this worked
	GLint linked;
	glGetProgramiv(m_screenShader, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLsizei len;
		glGetProgramiv(m_screenShader, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = new GLchar[len + 1];
		glGetProgramInfoLog(m_screenShader, len, &len, log);
		std::cerr << "ERROR: Shader linking failed: " << log << std::endl;
		delete[] log;

		return 0;
	}

	printf("Shader compiled\n");

	return m_screenShader;
}

// Sets up SDL, OpenGL, framebuffer
bool GCP_Framework::Init( glm::ivec2 screenSize )
{
	m_screenSize = screenSize;

	// SDL_Init is the main initialisation function for SDL
	// It takes a 'flag' parameter which we use to tell SDL what systems we're going to use
	// Here, we want to use SDL's video system, so we give it the flag for this
	// Incidentally, this also initialises the input event system
	// This function also returns an error value if something goes wrong
	// So we can put this straight in an 'if' statement to check and exit if need be
	printf("Initialising SDL\n");
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
	printf("Creating window\n");
	int winPosX = 100;
	int winPosY = 100;
	int winWidth = m_screenSize.x;
	int winHeight = m_screenSize.y;
	m_SDLwindow = SDL_CreateWindow("My Window!!!",  // The first parameter is the window title
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
	printf("Creating renderer\n");
	SDL_Renderer* renderer = SDL_CreateRenderer(m_SDLwindow, -1, 0);


	// Now that the SDL renderer is created for the window, we can create an OpenGL context for it!
	// This will allow us to actually use OpenGL to draw to the window
	printf("Creating GL context\n");
	m_SDLglcontext = SDL_GL_CreateContext(m_SDLwindow);

	// Call our initialisation function to set up GLEW and print out some GL info to console
	printf("Creating initing GL\n");
	if (!InitGL())
	{
		printf("Failed to init GL\n");
		return false;
	}

	printf("Starting OpenGLError\n");
	OpenGLError::Init();

	printf("Setting up imgui\n");
	// Setting up the GUI system
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	const char* glslVersion = "#version 130";
	ImGui_ImplSDL2_InitForOpenGL(m_SDLwindow, m_SDLglcontext);
	ImGui_ImplOpenGL3_Init(glslVersion);

	// Create the vertex array object for our triangle
	m_screenTrianglesVAO = CreateTriangleVAO();

	// Create the shaders and link them together into the shader program
	m_screenShader = LoadShaders("./resources/shaders/ScreenVertex.txt", "./resources/shaders/ScreenFragment.txt");

	m_mainBuffer = new Framebuffer(winWidth, winHeight);

	glEnable(GL_DEPTH_TEST);

	return true;
}

void GCP_Framework::Show()
{
	// sanity check that Init() has been called
	assert(m_mainBuffer != nullptr);
	// Show

	// Specify the colour to clear the framebuffer to
	glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
	// This writes the above colour to the colour part of the framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Binds OpenGL Texture
	glUseProgram(m_screenShader);
	glActiveTexture(GL_TEXTURE0);
	m_mainBuffer->BindGLTex();

	// Call our drawing function to draw that triangle!
	DrawVAOTris(m_screenTrianglesVAO, 6, m_screenShader);

	// Render GUI to screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// This tells the renderer to actually show its contents to the screen
	SDL_GL_SwapWindow(m_SDLwindow);
}

GCP_Framework::~GCP_Framework()
{
	delete m_mainBuffer;

	if(m_SDLglcontext)
	{
		SDL_GL_DeleteContext(m_SDLglcontext);
	}

	if(m_SDLwindow)
	{
		SDL_DestroyWindow(m_SDLwindow);
	}
	
	SDL_Quit();


	// TODO: currently doesn't clean up VAO or VBO
	if(m_screenTrianglesVAO)
	{
		glDeleteVertexArrays(1, &m_screenTrianglesVAO);
	}
}

void GCP_Framework::Shutdown()
{
	SDL_GL_DeleteContext(m_SDLglcontext);
	SDL_DestroyWindow(m_SDLwindow);
	SDL_Quit();
}