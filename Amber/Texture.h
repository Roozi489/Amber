#pragma once

#include "GL\glew.h"

#include <string>

class Texture
{
public:
    GLuint textureID;

    Texture();
    ~Texture();

    GLuint getTextureID() const;

    bool load(const std::string& filename);

	void freeGLData();
};

