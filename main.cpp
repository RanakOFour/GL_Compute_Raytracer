
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
	
	// Sets all pixels the same colour
	_myFramework.SetAllPixels( glm::vec3(0.1f,0.1f,0.3f) );

	RayTracer rayTracer;
	Camera camera;
	Ray r;
	glm::ivec2 pixelPosition;
	glm::vec3 pixelColour;
	for(int i = 0; i < 480; ++i)
	{
		pixelPosition.y = i;
		for(int j = 0; j < 640; ++j)
		{
			pixelPosition.x = j;
			r = camera.GetRay(pixelPosition);
			pixelColour = rayTracer.TraceRay(&r);

			_myFramework.DrawPixel(pixelPosition, pixelColour);
		}
	}

	// Pushes the framebuffer to OpenGL and renders to screen
	// Also contains an event loop that keeps the window going until it's closed
	_myFramework.ShowAndHold();
	return 0;


}
