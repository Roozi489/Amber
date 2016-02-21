#pragma once
#include "Texture.h"

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

	bool create(int w, int h);
	void destroy();

	void bind();
	void unbind();

	int width;
	int height;
	
	GLuint frambufferHandle;
	Texture textures[TextureType::Count];
};

