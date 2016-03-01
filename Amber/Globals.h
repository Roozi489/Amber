#pragma once
#include "Window.h"
#include "World.h"
#include "Camera.h"
#include "Time.h"
#include "ShaderManager.h"
#include "MeshManager.h"
#include "TextureManager.h"
#include "FontManager.h"

extern Window g_window;
extern World g_world;
extern Camera g_camera;

extern Time g_startTime;
extern Time g_frameTime;

extern ShaderManager g_shaderManager; 
extern MeshManager g_meshManager;
extern TextureManager g_textureManager;
extern FontManager g_fontManager;