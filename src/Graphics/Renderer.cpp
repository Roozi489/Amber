#include <Graphics/Renderer.h>

namespace Amber
{

Renderer::Renderer(GraphicsApi api)
	: m_graphicsApi(api)
{
}

Renderer::~Renderer()
{
}

void Renderer::handleWindowSizeChange(int newWidth, int newHeight)
{
	outWidth = newWidth;
	outHeight = newHeight;
}

GraphicsApi Renderer::getGraphicsApi()
{
	return m_graphicsApi;
}

}
