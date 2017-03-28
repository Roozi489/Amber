#pragma once
#include "Graphics/Graphics.h"
#include "Window/Window.h"
#include "Core/Time.h"
#include "World.h"
#include "Camera.h"
#include "Managers/ShaderManager.h"
#include "Managers/MeshManager.h"
#include "Managers/TextureManager.h"
#include "Managers/FontManager.h"

namespace Amber
{
	extern GraphicsApi g_graphicsApi;
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