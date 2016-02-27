#include "Globals.h"

SDL_Window *g_MainWindow;
SDL_GLContext g_Context;
int g_WindowWidth = 1024;
int g_WindowHeight = 768;
Time g_FrameTime;

World g_World;
Camera g_Camera;
const Uint8* g_Keystate;

ShaderManager g_ShaderManager;
MeshManager g_MeshManager;
TextureManager g_TextureManager;
FontManager g_FontManager;