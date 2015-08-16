#pragma once

#include "GL\glew.h"

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

    GLuint getTextureID() const;

    bool load(const std::string& filename, TextureFilter minMag = TextureFilter::Linear, TextureWrapMode wrap = TextureWrapMode::Repeat);
	void genAndBind(int width_, int height_);
	void setFilterAndWrap(TextureFilter minMag = TextureFilter::Linear, TextureWrapMode wrap = TextureWrapMode::Repeat);
	void bind(GLuint position);
	void destroy();
};

