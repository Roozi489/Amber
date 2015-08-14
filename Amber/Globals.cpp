#include "Globals.h"

SDL_Window *gMainWindow;
SDL_GLContext gContext;
int gWindowWidth = 1024;
int gWindowHeight = 768;
float gFrameTime;

World gWorld;
Camera gCamera;
const Uint8* gKeystate;

ShaderManager gShaderManager;
MeshManager gMeshManager;
TextureManager gTextureManager;