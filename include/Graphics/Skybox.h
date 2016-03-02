#pragma once

#include "CubeTexture.h"

namespace Amber
{

// TODO: decide how to do this
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
	void destroy();
	void update();
	void render();
};

}