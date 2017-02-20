#pragma once
#include "OpenGL.h"

#include <string>
#include "Texture.h"

namespace Amber
{

class CubeTexture
{
public:
	CubeTexture() = default;
	
	uint32 textureHandle;
	int width;
	int height;

	TextureFilter minMagfilter;
	TextureWrapMode wrapMode;
	
	bool loadFromFile(std::string front, std::string back, std::string top, std::string bottom, std::string left, std::string right,
		TextureFilter minMag = TextureFilter::Linear, TextureWrapMode wrap = TextureWrapMode::ClampToEdge);
	void genAndBind();
	void genAndBind(int w, int h);
	void activeAndBind(GLuint position);
	void unbind();
	void setFilterAndWrap(TextureFilter minMag = TextureFilter::Linear, TextureWrapMode wrap = TextureWrapMode::ClampToEdge);
	void destroy();
};

}
