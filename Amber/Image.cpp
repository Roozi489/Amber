#include "Image.h"
#include "Types.h"
#include "Window.h"

#include <stb/stb_image.h>

#include <sstream>

SDL_Surface* loadSDL_SurfaceFromFile(const char* filename)
{
	int w, h, bytesperpixel;
	unsigned char *data = stbi_load(filename, &w, &h, &bytesperpixel, 0);
	if (data == nullptr)
	{
		std::stringstream sstream;
		sstream << "Failed to load image: " << filename << "  -  " << "stbi_load returned NULL.";
		criticalError(sstream.str());
	}

	uint32 Rmask, Gmask, Bmask, Amask = 0;
	switch (bytesperpixel)
	{
	case 1:
		Rmask = 0xff;
		Gmask = 0xff;
		Bmask = 0xff;
		break;
	case 2:
		Rmask = 0x00ff;
		Gmask = 0x00ff;
		Bmask = 0x00ff;
		Amask = 0xff00;
		break;
	case 3:
		Rmask = 0x0000FF;
		Gmask = 0x00FF00;
		Bmask = 0xFF0000;
		break;
	case 4:
		Rmask = 0x000000FF;
		Gmask = 0x0000FF00;
		Bmask = 0x00FF0000;
		Amask = 0xFF000000;
		break;
	default:
		stbi_image_free(data);
		std::stringstream sstream;
		sstream << "Failed to load image: " << filename << "  -  Unsupported bit depth of " << bytesperpixel * 8;
		criticalError(sstream.str());
	}

	// ... w = width, h = height, n = # 8-bit components per pixel ...
	// No padding for pitch, so it's w*bytesperpixel
	SDL_Surface* result = SDL_CreateRGBSurfaceFrom(data, w, h, bytesperpixel * 8, w*bytesperpixel, Rmask, Gmask, Bmask, Amask);

	if (result == nullptr)
	{
		std::stringstream sstream;
		sstream << "Failed to load image: " << filename << "  -  SDL_CreateRGBSurfaceFrom() failed.";
		criticalError(sstream.str());
	}

	return result;
}
