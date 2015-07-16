#pragma once

#include "GL\glew.h"

#include <string>

class Texture2D
{
public:
    GLuint textureID;

    Texture2D();
    ~Texture2D();

    GLuint getTextureID() const;

    bool load(std::string filename);
};

