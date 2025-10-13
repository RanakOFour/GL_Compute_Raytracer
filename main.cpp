
#include "GCP_GFX_Framework.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <vector>

int main(int argc, char* argv[])
{
	// Set window size
	glm::ivec2 winSize(500, 500);

	// This will handle rendering to screen
	GCP_Framework _myFramework;

	// Initialises SDL and OpenGL and sets up a framebuffer
	if (!_myFramework.Init(winSize))
	{
		return -1;
	}
	
	// Sets all pixels the same colour
	_myFramework.SetAllPixels( glm::vec3(0.1f,0.1f,0.3f) );

	int width = 500;
	int height = 500;

	glm::ivec2 pixelPosition = glm::ivec2(0, 0);

	glm::vec3 pixelColour(1, 0, 0);

	int dataOffset = 0;

	unsigned char* img_data = stbi_load("./triangle.png", &width, &height, NULL, 4);

	if (!img_data || img_data == NULL)
	{
		printf("Failed to load texture\n");
		throw std::exception();
	}

	std::vector<unsigned char> data = std::vector<unsigned char>();

	data.assign(img_data, img_data + 500 * 500 * 4);

	stbi_image_free(img_data);

	unsigned char* dataPtr = &(data[0]);

	for(int i = winSize.y; i > -1; --i)
	{
		for(int j = 0; j < width; ++j)
		{
			pixelPosition = glm::ivec2(j, i);

			int newColChannel = 0;
			// Iterate once for each channel (3)
			for (int k = 0; k < 3; k++)
			{
				newColChannel = (int)*dataPtr;
				// if(newColChannel != 0)
				// {
				// 	printf("Color channel %i: %i\n", k, newColChannel);
				// }
				pixelColour[k] = (float)newColChannel / 255.0f;
				++dataPtr;
			}

			// Increment data one last time to skip alpha channel
			++dataPtr;

			// if(pixelColour[0] != 0 || pixelColour[1] != 0 || pixelColour[2] != 0)
			// {
			// 	std::cout << "Drawing pixel with color: " << pixelColour[0] << " " << pixelColour[1] << " " << pixelColour[2] << " at (" << pixelPosition.x << ", " << pixelPosition.y << ")" << std::endl;
			// }

			_myFramework.DrawPixel(pixelPosition, pixelColour);
		}
	}

	// Draws a single pixel
	pixelPosition = glm::ivec2(250, 250);
	pixelColour = glm::vec3(1.0f, 0.0f, 0.0f);
	_myFramework.DrawPixel(pixelPosition, pixelColour);


	// Pushes the framebuffer to OpenGL and renders to screen
	// Also contains an event loop that keeps the window going until it's closed
	_myFramework.ShowAndHold();
	return 0;
}
