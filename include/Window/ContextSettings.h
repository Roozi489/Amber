#pragma once
#include "Core/Types.h"

#include <SDL/SDL.h>

// Asks for the context to be created on nVidia graphics when optimus is enabled
extern "C"
{
	_declspec(dllexport) Amber::uint32 NvOptimusEnablement = 0x00000001;
}

namespace Amber
{

enum class ProfileMask
{
	Core = SDL_GL_CONTEXT_PROFILE_CORE,
	Compatibility = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY
};

struct ContextSettings
{
	int majorVersion = 4;
	int minorVersion = 2;
	int depthBits = 16;
	int stencilBits = 8;
	int antialiasingLevel = 0;
	int acceleratedVisual = 1;
	int vsyncEnabled = 0;
	ProfileMask profileMask = ProfileMask::Core;
};

}