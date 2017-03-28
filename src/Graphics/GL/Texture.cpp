#include "Graphics/GL/Texture.h"
#include "Graphics/Image.h"
#include "Window/Window.h"
#include "Core/Utility.h"
#include "Math/Math.h"



#include <Core/Globals.h>


namespace Amber
{

Texture::Texture()
{
}

Texture::~Texture()
{
}

bool Texture::loadFromFile(const std::string& filename, TextureFilter minMag, TextureWrapMode wrap)
{
	Image image;
	if (image.create("Textures/" + filename))
	{
		criticalError("Error loading texture from: " + filename + "\nReason: " + getImageLoadError());
		return false;
	}
	width = image.width;
	height = image.height;

	// some magic formula I found
	int numMipmaps = 1 + static_cast<int>(floor(log2(max(width, height, image.componentsPerPixel))));


	genAndBind();
	glTexStorage2D(GL_TEXTURE_2D, numMipmaps, GL_RGBA8, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
	glGenerateMipmap(GL_TEXTURE_2D);  // Generate numMipmaps number of mipmaps here.

	setFilterAndWrap(minMag, wrap);

	image.destroy();

	return true;
}

void Texture::setFormat(TextureInternalFormat internalFormat, TextureFormat format, TextureType type)
{
	glTexImage2D(GL_TEXTURE_2D, 0, toUnderlyingType(internalFormat), width, height, 0, toUnderlyingType(format), toUnderlyingType(type), nullptr);
}

void Texture::genAndBind()
{
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
}

void Texture::genAndBind(int w, int h)
{
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	width = w;
	height = h;
}

void Texture::activeAndBind(GLuint position)
{
	assert_amber(position < MaxTexturePosition, "Invalid texture position : " + position);

	glActiveTexture(GL_TEXTURE0 + position);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setFilterAndWrap(TextureFilter minMag, TextureWrapMode wrap)
{
	minMagfilter = minMag;
	wrapMode = wrap;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toUnderlyingType(wrap));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toUnderlyingType(wrap));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toUnderlyingType(minMag));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toUnderlyingType(minMag));
}

void Texture::destroy()
{
	if (textureHandle != -1)
		glDeleteTextures(1, &textureHandle);

	textureHandle = -1;
}

}
