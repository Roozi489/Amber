#pragma once
#include "Graphics/Graphics.h"
#include "Core/Time.h"

namespace Amber
{

class Renderer
{
protected:
	GraphicsApi m_graphicsApi;

	int outWidth;
	int outHeight;

public:
	Renderer(GraphicsApi api);
	~Renderer();

	virtual void init() = 0;
	virtual void handleWindowSizeChange(int newWidth, int newHeight);
	virtual void renderAll(Time delta) = 0;
	virtual void finalRenderToMainFramebuffer() = 0;

	GraphicsApi getGraphicsApi();
};

}