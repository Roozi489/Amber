#include "Core/Globals.h"


namespace Amber
{
	GraphicsApi g_graphicsApi = GraphicsApi::Vulkan;
	Window g_window;
	World g_world;
	Camera g_camera;
	Time g_startTime;
	Time g_frameTime;
	const uint8* g_keystate;

	ShaderManager g_shaderManager;
	MeshManager g_meshManager;
	TextureManager g_textureManager;
	FontManager g_fontManager;

}