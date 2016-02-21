#pragma once
#include "OpenGL.h"

#include <string>

enum class TextureFilter : GLint
{
	Linear = GL_LINEAR,
	Nearest = GL_NEAREST,
};

enum class TextureWrapMode : GLint
{
	Clamp = GL_CLAMP,
	Repeat = GL_REPEAT,
	ClampToEdge = GL_CLAMP_TO_EDGE,
	ClampToBorder = GL_CLAMP_TO_BORDER,
	MirroredRepeat = GL_MIRRORED_REPEAT,
};

class Texture
{
public:
	static const GLuint MaxTexturePosition = 31;

	int width;
	int height;
	TextureFilter minMagfilter;
	TextureWrapMode wrapMode;

    GLuint textureID;

    Texture();
    ~Texture();

    bool load(const std::string& filename, TextureFilter minMag = TextureFilter::Linear, TextureWrapMode wrap = TextureWrapMode::Repeat);
	void genAndBind();
	void genAndBind(int w, int h);
	void setFilterAndWrap(TextureFilter minMag = TextureFilter::Linear, TextureWrapMode wrap = TextureWrapMode::Repeat);
	void bindAndSetActive(GLuint position);
	void destroy();
};

