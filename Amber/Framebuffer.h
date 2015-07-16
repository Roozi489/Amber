#pragma once

#include <gl/glew.h>

class Framebuffer
{
public:
	Framebuffer();

	bool create();

private:
	int mWidth;
	int mHeight;

	GLuint mFramebuffer;
};

