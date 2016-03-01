#pragma once
#include "Types.h"

#include <GL/glew.h>
#include <type_traits>

// Verify types
static_assert(std::is_same<GLubyte, uint8>::value, "GLubyte is not the same as uint8.");
static_assert(std::is_same<GLbyte, int8>::value, "GLbyte is not the same as int8.");
static_assert(std::is_same<GLushort, uint16>::value, "GLushort is not the same as uint16.");
static_assert(std::is_same<GLshort, int16>::value, "GLshort is not the same as int16.");
static_assert(std::is_same<GLuint, uint32>::value, "GLuint is not the same as uint32.");
static_assert(std::is_same<GLint, int32>::value, "GLint is not the same as int32.");

// Verify boolean values
static_assert(GL_FALSE == false, "GL_FALSE is not the same as false.");
static_assert(GL_TRUE == true, "GL_TRUE is not the same as true.");

void _checkGlError(const char* file, int line, const char* function);
#define checkGlError() _checkGlError(__FILE__,__LINE__,__FUNCTION__)
void ignoreGlError();