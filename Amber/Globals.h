#pragma once
#include "World.h"
#include "Camera.h"
#include "ShaderManager.h"
#include "MeshManager.h"
#include "TextureManager.h"
#include "Time.h"
#include "FontManager.h"

#include <SDL/SDL.h>

// TODO: add all of these to Window class
extern SDL_Window *g_MainWindow;
extern SDL_GLContext g_Context;
extern int g_WindowWidth;
extern int g_WindowHeight;
extern Time g_FrameTime;

extern World g_World;
extern Camera g_Camera;
// TODO: Input class
extern const Uint8* g_Keystate;

extern ShaderManager g_ShaderManager; 
extern MeshManager g_MeshManager;
extern TextureManager g_TextureManager;
extern FontManager g_FontManager;