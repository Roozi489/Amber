#pragma once

#include "CubeTexture.h"

class Skybox
{
private:
    GLuint vbo;
    GLuint vao;
    CubeTexture skyboxTexture;

    GLuint shaderProgramID;

public:
    Skybox();
    ~Skybox();

    void init();
    void update();
    void render();
};

