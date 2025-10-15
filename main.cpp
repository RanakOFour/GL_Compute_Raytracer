
#include "GCP_GFX_Framework.h"
#include "Camera.h"
#include "RayTracer.h"
#include "Ray.h"

int main(int argc, char* argv[])
{
	// Set window size
	glm::ivec2 winSize(640, 480);

	// This will handle rendering to screen
	GCP_Framework _myFramework;

	// Initialises SDL and OpenGL and sets up a framebuffer
	if (!_myFramework.Init(winSize))
	{
		return -1;
	}
	
	// Preparing a position to draw a pixel
	glm::ivec2 pixelPosition = winSize / 2;

	// Preparing a colour to draw
	// Colours are RGB, each value ranges between 0 and 1
	glm::vec3 pixelColour(1, 0, 0);


	// Sets all pixels the same colour
	_myFramework.SetAllPixels( glm::vec3(0.1f,0.1f,0.3f) );

	// Draws a single pixel
	//_myFramework.DrawPixel(pixelPosition, pixelColour);

	RayTracer rayTracer;
	Camera camera;
	Ray r;
	for(int i = 0; i < 480; ++i)
	{
		pixelPosition.y = i;
		for(int j = 0; j < 640; ++j)
		{
			pixelPosition.x = j;
			r = camera.GetRay(pixelPosition);
			pixelColour = rayTracer.TraceRay(&r);
			if(pixelColour != glm::vec3(0))
			{
				printf("Ray hit sphere at (%i, %i)\n", j, i);
			}

			_myFramework.DrawPixel(pixelPosition, pixelColour);
		}
	}



	// Pushes the framebuffer to OpenGL and renders to screen
	// Also contains an event loop that keeps the window going until it's closed
	_myFramework.ShowAndHold();
	return 0;


}
