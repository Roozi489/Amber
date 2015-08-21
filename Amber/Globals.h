#pragma once
#include "World.h"
#include "Camera.h"
#include "ShaderManager.h"
#include "MeshManager.h"
#include "TextureManager.h"
#include "Time.h"

#include <SDL/SDL.h>

extern SDL_Window *gMainWindow;
extern SDL_GLContext gContext;
extern int gWindowWidth;
extern int gWindowHeight;
extern Time gFrameTime;

extern World gWorld;
extern Camera gCamera;
extern const Uint8* gKeystate;

extern ShaderManager gShaderManager; 
extern MeshManager gMeshManager;
extern TextureManager gTextureManager;