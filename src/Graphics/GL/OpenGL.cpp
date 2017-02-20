#include "Graphics/GL/OpenGL.h"
#include "Core/Log.h"
#include "Core/String.h"

#include <string>

namespace Amber
{

void _checkGlError(const char* file, int line, const char* function)
{
	GLenum err{ glGetError() };

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

		Log::error(stringFormat("GL Error - %s - %d:%s  -  %s", error, file, line, function));
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

}
