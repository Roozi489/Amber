#include "OpenGL.h"
#include "Utility.h"

#include <string>

void _checkGlError(const char* file, int line, const char* function)
{
	GLenum err(glGetError());

	while (err != GL_NO_ERROR)
	{
		std::string error;

		switch (err)
		{
		case GL_INVALID_OPERATION:
			error = "INVALID_OPERATION";
			break;
		case GL_INVALID_ENUM:
			error = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error = "INVALID_VALUE";
			break;
		case GL_OUT_OF_MEMORY:
			error = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}

		log("GL Error - " + error + " - " + file + ":" + std::to_string(line) + "  -  " + function);
		err = glGetError();
	}
}

void ignoreGlError()
{
	GLenum err;
	do
	{
		err = glGetError();
	} while (err != GL_NO_ERROR);
}