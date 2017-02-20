#pragma once
#include "Core/NonCopyable.h"
#include "Texture.h"
#include "OpenGL.h"

#include <vector>

namespace Amber
{

class RenderTexture : public NonCopyable
{
public:
	enum RenderTextureType
	{
		Color = 1,
		Depth = 2,
		ColorAndDepth = Color | Depth,
		Lighting = 4 | Color,
		Shadow = 8 | Depth
	};

	int width;
	int height;

	GLuint framebufferHandle;

	RenderTextureType type;
	Texture colorTexture;
	Texture depthTexture;

	std::vector<GLenum> drawbufferAttachments;

	RenderTexture() = default;

	void create(int w, int h, RenderTextureType type = RenderTextureType::ColorAndDepth, TextureFilter minMag = TextureFilter::Linear, TextureWrapMode wrap = TextureWrapMode::ClampToEdge);
	void destroy();
	void bind();
	void unbind();

	float ratio() const;
};

}