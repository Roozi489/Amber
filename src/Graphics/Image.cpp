#include "Graphics/Image.h"
#include "Core/Types.h"
#include "Window/Window.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <sstream>

namespace Amber
{

bool Image::create(const std::string& filePath)
{
	return create(filePath.c_str());
}

bool Image::create(const char* filePath)
{
	int forceComponents = 4;
	data = stbi_load(filePath, &width, &height, &componentsPerPixel, forceComponents);
	name = filePath;
	return data == nullptr;
}

void Image::destroy()
{
	stbi_image_free(data);
}

const char* getImageLoadError()
{
	return stbi__g_failure_reason;
}


SDL_Surface* loadSDL_SurfaceFromFile(const char* filename)
{
	Image image;
	image.create(filename);
	if (image.data == nullptr)
	{
		std::stringstream sstream;
		sstream << "Failed to load image: " << filename << "  -  " << "stbi_load returned NULL.";
		criticalError(sstream.str());
	}

	uint32 Rmask, Gmask, Bmask, Amask = 0;
	switch (image.componentsPerPixel)
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
		image.destroy();
		std::stringstream sstream;
		sstream << "Failed to load image: " << filename << "  -  Unsupported bit depth of " << image.componentsPerPixel * 8;
		criticalError(sstream.str());
	}

	// n = # 8-bit components per pixel ...
	// No padding for pitch, so it's w*bytesperpixel
	SDL_Surface* result = SDL_CreateRGBSurfaceFrom(image.data, image.width, image.height, image.componentsPerPixel * 8, image.width*image.componentsPerPixel, Rmask, Gmask, Bmask, Amask);

	if (result == nullptr)
	{
		std::stringstream sstream;
		sstream << "Failed to load image: " << filename << "  -  SDL_CreateRGBSurfaceFrom() failed.";
		criticalError(sstream.str());
	}

	return result;
}

}
