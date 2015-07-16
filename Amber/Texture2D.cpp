#include "Texture2D.h"
#include "Utility.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture2D::Texture2D()
: textureID(-1)
{

}

Texture2D::~Texture2D()
{

}

GLuint Texture2D::getTextureID() const
{
    return textureID;
}

bool Texture2D::load(std::string filename)
{
    int width, height, componentsPerPixel;
    int forceChannels = 4;
    std::string path = "Textures/" + filename;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &componentsPerPixel, forceChannels);
    if (!data)
    {
        criticalError("Error loading texture from: " + filename + "\nReason: " + stbi__g_failure_reason);
        return false;
    }

    // some magic formula i found
    int num_mipmaps = 1 + static_cast<int>(floor(log2(max(width, height, componentsPerPixel))));

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexStorage2D(GL_TEXTURE_2D, num_mipmaps, GL_RGBA8, width, height);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);  // Generate num_mipmaps number of mipmaps here.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    checkGlError();

    stbi_image_free(data);
    return true;
}
