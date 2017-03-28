#include "Systems/RenderSystem.h"
#include "Core/Globals.h"
#include "Core/String.h"
#include "Core/Utility.h"
#include "Graphics/Vulkan/VulkanRenderer.h"
#include "Graphics/GL/GLRenderer.h"

#include <unordered_map>

namespace Amber
{

RenderSystem::RenderSystem()
{

}

Renderer* RenderSystem::getRenderer()
{
	return m_renderer.get();
}

void RenderSystem::init()
{
	if (g_graphicsApi == GraphicsApi::OpenGL)
	{
		m_renderer = std::unique_ptr<Renderer>{ std::make_unique<GLRenderer>() };
	}
	else if (g_graphicsApi == GraphicsApi::Vulkan)
	{
		m_renderer = std::unique_ptr<Renderer>{ std::make_unique<Vulkan::VulkanRenderer>() };
	}
}

void RenderSystem::destroy()
{

}

void RenderSystem::update(Time delta)
{
	BaseSystem::update(delta);
	m_renderer->renderAll(delta);
}

void RenderSystem::handleWindowSizeChange(int newWidth, int newHeight)
{
	m_renderer->handleWindowSizeChange(newWidth, newHeight);
}

void RenderSystem::display()
{
	m_renderer->finalRenderToMainFramebuffer();
}


}
