#pragma once

#include <GL/glew.h>

#include <string>

class CubeTexture
{
private:
    GLuint mTextureID;

public:
    CubeTexture();

    GLuint getTextureID() const;

    bool loadSide(std::string filename, GLenum sideTarget);
    bool create(std::string front, std::string back, std::string top, std::string bottom, std::string left, std::string right);
};

