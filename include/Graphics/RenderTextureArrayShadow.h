#pragma once
#include "OpenGL.h"
#include "Texture.h"

namespace Amber
{

class RenderTextureArrayShadow
{
public:
	int width;
	int height;

	GLuint framebufferHandle;
	GLuint depthTextureHandle;
	GLuint shadowTextureHandle;


	//Texture depthTexture;
	//CubeTexture shadowTexture;

	RenderTextureArrayShadow() = default;
	~RenderTextureArrayShadow() = default;

	void create(int w, int h, TextureWrapMode wrap = TextureWrapMode::ClampToEdge);
	void destroy();
	void bind();
	void unbind();

	float ratio() const;
};

}
