#include "Graphics/CubeTexture.h"
#include "Graphics/Texture.h"
#include "Graphics/Image.h"
#include "Core/Utility.h"
#include "Window/Window.h"

namespace Amber
{

bool CubeTexture::loadFromFile(std::string front, std::string back, std::string top, std::string bottom, std::string left, std::string right, TextureFilter minMag, TextureWrapMode wrap)
{
	auto loadSide = [&](const std::string& name, GLenum target) -> bool
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle);

		Image image;
		if (image.create("Textures/" + name))
		{
			criticalError("Error loading texture from: " + name + "\nReason: " + getImageLoadError());
			return false;
		}
		width = image.width;
		height = image.height;

		glTexImage2D(target, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
		image.destroy();

		return true;
	};

	glGenTextures(1, &textureHandle);

	bool result = true;
	result &= loadSide(front, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	result &= loadSide(back, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	// Swap bottom and top
	result &= loadSide(bottom, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	result &= loadSide(top, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	result &= loadSide(left, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	result &= loadSide(right, GL_TEXTURE_CUBE_MAP_POSITIVE_X);

	setFilterAndWrap();

	return result;
}

void CubeTexture::genAndBind()
{
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle);
}

void CubeTexture::genAndBind(int w, int h)
{
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle);

	width = w;
	height = h;
}

void CubeTexture::activeAndBind(GLuint position)
{
	assert_amber(position < Texture::MAX_TEXTURE_POSITION, "Invalid texture position : " + position);

	glActiveTexture(GL_TEXTURE0 + position);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle);
}

void CubeTexture::unbind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeTexture::setFilterAndWrap(TextureFilter minMag, TextureWrapMode wrap)
{
	minMagfilter = minMag;
	wrapMode = wrap;
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrap));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrap));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, static_cast<GLint>(wrap));

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(minMag));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(minMag));
}

void CubeTexture::destroy()
{
	if (textureHandle != -1)
		glDeleteTextures(1, &textureHandle);

	textureHandle = -1;
}

}