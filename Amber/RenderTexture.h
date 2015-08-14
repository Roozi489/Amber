#pragma once

#include <gl/glew.h>

class RenderTexture
{
public:
	int width;
	int height;

	GLuint bufferID;
	GLuint textureID;

	RenderTexture();

	bool create(int width, int height);
	void bind();
	void unbind();
};

