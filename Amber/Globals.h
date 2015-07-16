#pragma once
#include "World.h"
#include "Camera.h"
#include "ShaderManager.h"
#include "MeshManager.h"

#include <SDL/SDL.h>

#include <ft2build.h>
#include FT_FREETYPE_H

extern SDL_Window *gMainWindow;
extern SDL_GLContext gContext;
extern int gWindowWidth;
extern int gWindowHeight;
extern float gFrameTime;

extern World gWorld;
extern Camera gCamera;
extern const Uint8* gKeystate;

extern ShaderManager gShaderManager; 
extern MeshManager gMeshManager;