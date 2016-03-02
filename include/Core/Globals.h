#pragma once
#include "Managers/ShaderManager.h"
#include "Managers/MeshManager.h"
#include "Managers/TextureManager.h"
#include "Managers/FontManager.h"
#include "Window/Window.h"
#include "Core/Time.h"
#include "World.h"
#include "Camera.h"

namespace Amber
{

extern Window g_window;
extern World g_world;
extern Camera g_camera;

extern Time g_startTime;
extern Time g_frameTime;

extern ShaderManager g_shaderManager;
extern MeshManager g_meshManager;
extern TextureManager g_textureManager;
extern FontManager g_fontManager;

}