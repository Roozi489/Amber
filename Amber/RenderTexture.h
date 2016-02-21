#pragma once
#include "Texture.h"
#include "OpenGL.h"

#include <vector>

class RenderTexture
{
public:
	enum RenderTextureType
	{
		Color = 1,
		Depth = 2,
		ColorAndDepth = Color | Depth,
		Lighting = 4 | Color,
	};

	GLuint framebufferHandle;
	GLuint vaoHandle;
	GLuint vboHandle;

	RenderTextureType type;
	Texture colorTexture;
	Texture depthTexture;

	std::vector<GLenum> drawbufferAttachments;
	

	RenderTexture();
	bool create(int width, int height, RenderTextureType type = RenderTextureType::ColorAndDepth, TextureFilter minMag = TextureFilter::Linear, TextureWrapMode wrap = TextureWrapMode::ClampToEdge);
	void destroy();
	void bind();
	void unbind();
};

