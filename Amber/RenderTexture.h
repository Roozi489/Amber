#pragma once
#include "Texture.h"

#include <gl/glew.h>

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

	RenderTextureType type;
	Texture colorTexture;
	Texture depthTexture;

	std::vector<GLenum> drawbufferAttachments;
	
	GLuint bufferID;

	RenderTexture();
	bool create(int width, int height, RenderTextureType type = RenderTextureType::ColorAndDepth, TextureFilter minMag = TextureFilter::Linear, TextureWrapMode wrap = TextureWrapMode::ClampToEdge);
	void destroy();
	void bind();
	void unbind();
};

