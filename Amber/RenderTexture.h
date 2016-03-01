#pragma once
#include "Texture.h"
#include "OpenGL.h"
#include "NonCopyable.h"

#include <vector>

class RenderTexture : public NonCopyable
{
public:
	enum RenderTextureType
	{
		Color = 1,
		Depth = 2,
		ColorAndDepth = Color | Depth,
		Lighting = 4 | Color,
	};

	int width;
	int height;

	GLuint framebufferHandle;
	GLuint vaoHandle;
	GLuint vboHandle;

	RenderTextureType type;
	Texture colorTexture;
	Texture depthTexture;

	std::vector<GLenum> drawbufferAttachments;

	RenderTexture();

	void create(int w, int h, RenderTextureType type = RenderTextureType::ColorAndDepth, TextureFilter minMag = TextureFilter::Linear, TextureWrapMode wrap = TextureWrapMode::ClampToEdge);
	void destroy();
	void bind();
	void unbind();
};

