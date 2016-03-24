#pragma once
#include "Core/Types.h"

#include <string>



struct SDL_Surface;

namespace Amber
{

struct Image
{
	int width;
	int height;
	int componentsPerPixel;
	uint8* data;
	std::string name;

	bool create(const std::string& filePath);
	bool create(const char* filePath);
	void destroy();
};

const char* getImageLoadError();

SDL_Surface* loadSDL_SurfaceFromFile(const char* filePath);

}