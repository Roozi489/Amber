#pragma once
#include "Core/NonCopyable.h"
#include "Graphics/GL/OpenGL.h"
#include "Texture.h"
#include "CubeTexture.h"


namespace Amber
{

class RenderTextureCubeShadow : public NonCopyable
{
public:
	int width;
	int height;

	GLuint framebufferHandle;

	Texture depthTexture;
	CubeTexture shadowTexture;

	RenderTextureCubeShadow() = default;

	void create(int w, int h);
	void destroy();
	void bind();
	void unbind();

	float ratio() const;
};

}

