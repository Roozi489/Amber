#pragma once
#include <SDL/SDL.h>

namespace Amber
{

enum class ProfileMask
{
	Core = SDL_GL_CONTEXT_PROFILE_CORE,
	Compatibility = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY
};

struct ContextSettings
{
	int majorVersion = 3;
	int minorVersion = 2;
	int depthBits = 16;
	int stencilBits = 8;
	int antialiasingLevel = 0;
	int acceleratedVisual = 1;
	ProfileMask profileMask = ProfileMask::Core;
};

}