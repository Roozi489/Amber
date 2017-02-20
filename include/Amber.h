#pragma once
#include "Core/Time.h"

namespace Amber
{

enum class GraphicsApi
{
	OpenGL,
	Vulkan
};

class Amber
{
public:
	Amber();
	~Amber();

	void run();
private:
	// TODO: load graphics api from config
	GraphicsApi m_graphicsApi = GraphicsApi::Vulkan;

	void init();
	void update(Time delta);
};

}