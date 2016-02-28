#include "Texture.h"
#include "Utility.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Texture::Texture()
: width(-1)
, height(-1)
, textureHandle(-1)
{
}

Texture::~Texture()
{
}

bool Texture::load(const std::string& filename, TextureFilter minMag, TextureWrapMode wrap)
{
    int componentsPerPixel;
    int forceChannels = 4;
    std::string path = "Textures/" + filename;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &componentsPerPixel, forceChannels);
    if (!data)
    {
        criticalError("Error loading texture from: " + filename + "\nReason: " + stbi__g_failure_reason);
        return false;
    }

    // some magic formula I found
    int numMipmaps = 1 + static_cast<int>(floor(log2(max(width, height, componentsPerPixel))));

    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glTexStorage2D(GL_TEXTURE_2D, numMipmaps, GL_RGBA8, width, height);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);  // Generate numMipmaps number of mipmaps here.
    
	setFilterAndWrap(minMag, wrap);

    stbi_image_free(data);
    return true;
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrap));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrap));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(minMag));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(minMag));
}

void Texture::destroy()
{
	if (textureHandle != -1)
		glDeleteTextures(1, &textureHandle);

	textureHandle = -1;
}
