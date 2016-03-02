#pragma once
#include "OpenGL.h"

#include <string>

namespace Amber
{

class CubeTexture
{
public:
	CubeTexture();

	GLuint getTextureID() const;

	bool loadSide(std::string filename, GLenum sideTarget);
	bool create(std::string front, std::string back, std::string top, std::string bottom, std::string left, std::string right);
private:
	GLuint m_TextureHandle;
};

}