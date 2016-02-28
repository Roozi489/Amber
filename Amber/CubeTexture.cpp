#include "CubeTexture.h"
#include "Utility.h"

#include <stb/stb_image.h>

CubeTexture::CubeTexture()
{
}

GLuint CubeTexture::getTextureID() const
{
    return m_TextureHandle;
}

bool CubeTexture::loadSide(std::string filename, GLenum sideTarget)
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureHandle);

    // TODO: write helper for this
    int x, y, n;
    int forceChannels = 4;
    unsigned char*  imageData = stbi_load(filename.c_str(), &x, &y, &n, forceChannels);
    if (!imageData)
    {
        // TODO: non-critical error
        criticalError("ERROR: Could not load texture " + filename + " for cubemap.");
        return false;
    }
    // non-power-of-2 dimensions check
    if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0)
    {
        log("WARNING: Image \"" + filename + "\" is not power-of-2 dimensions.");
    }

    glTexImage2D(sideTarget, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    stbi_image_free(imageData);

    return true;
}

bool CubeTexture::create(std::string front, std::string back, std::string top, std::string bottom, std::string left, std::string right)
{
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &m_TextureHandle);

    bool result = true;
    result &= loadSide(front, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
    result &= loadSide(back, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    // Swap bottom and top
    result &= loadSide(bottom, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    result &= loadSide(top, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    result &= loadSide(left, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    result &= loadSide(right, GL_TEXTURE_CUBE_MAP_POSITIVE_X);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return result;
}
