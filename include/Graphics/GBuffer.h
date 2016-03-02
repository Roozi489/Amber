#pragma once
#include "Texture.h"

namespace Amber
{

class GBuffer
{
public:
	enum TextureType
	{
		Albedo,
		Specular,
		Normal,
		Depth,
		//Emission,
		Count
	};

	GBuffer();
	~GBuffer();

	void create(int w, int h);
	void destroy();

	void bind();
	void unbind();

	int width;
	int height;

	GLuint framebufferHandle;
	Texture textures[TextureType::Count];
};

}