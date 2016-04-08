#pragma once
#include "OpenGL.h"

#include <string>

namespace Amber
{

enum class TextureFilter : int32
{
	Linear = GL_LINEAR,
	Nearest = GL_NEAREST,
};

enum class TextureWrapMode : int32
{
	Clamp = GL_CLAMP,
	Repeat = GL_REPEAT,
	ClampToEdge = GL_CLAMP_TO_EDGE,
	ClampToBorder = GL_CLAMP_TO_BORDER,
	MirroredRepeat = GL_MIRRORED_REPEAT,
};

enum class TextureInternalFormat : GLint
{
	Depth = GL_DEPTH_COMPONENT,
	DepthStencil = GL_DEPTH_STENCIL,
	Red = GL_RED,
	RG = GL_RG,
	RGB = GL_RGB,
	RGBA = GL_RGBA,
	RGB8 = GL_RGB8,
	RGB10A2 = GL_RGB10_A2,
	Depth24 = GL_DEPTH_COMPONENT24,
	Depth32 = GL_DEPTH_COMPONENT32,
	Depth32F = GL_DEPTH_COMPONENT32F,
	R32F = GL_R32F
};

enum class TextureFormat : GLenum
{
	Red = GL_RED,
	RG = GL_RG,
	RGB = GL_RGB,
	BGR = GL_BGR,
	RGBA = GL_RGBA,
	BGRA = GL_BGRA,
	RedInteger = GL_RED_INTEGER,
	RGInteger = GL_RG_INTEGER,
	RGBInteger = GL_RGB_INTEGER,
	BGRInteger = GL_BGR_INTEGER,
	RGBAInteger = GL_RGBA_INTEGER,
	BGRAInteger = GL_BGRA_INTEGER,
	StencilIndex = GL_STENCIL_INDEX,
	DepthComponent = GL_DEPTH_COMPONENT,
	DepthStencil = GL_DEPTH_STENCIL
};

enum class TextureType : GLenum
{
	UByte = GL_UNSIGNED_BYTE,
	Byte = GL_BYTE,
	UShort = GL_UNSIGNED_SHORT,
	Short = GL_SHORT,
	UInt = GL_UNSIGNED_INT,
	Int = GL_INT,
	Float = GL_FLOAT,
	UByte332 = GL_UNSIGNED_BYTE_3_3_2,
	UShort565= GL_UNSIGNED_SHORT_5_6_5,
	UShort4444= GL_UNSIGNED_SHORT_4_4_4_4,
	UShort5551 = GL_UNSIGNED_SHORT_5_5_5_1,
	UInt8888= GL_UNSIGNED_INT_8_8_8_8,
	UInt1010102= GL_UNSIGNED_INT_10_10_10_2,
};

class Texture
{
public:
	static const GLuint MAX_TEXTURE_POSITION = 32;

	GLuint textureHandle;
	int width;
	int height;
	TextureFilter minMagfilter;
	TextureWrapMode wrapMode;


	Texture();
	~Texture();

	bool loadFromFile(const std::string& filename, TextureFilter minMag = TextureFilter::Linear, TextureWrapMode wrap = TextureWrapMode::Repeat);
	void setFormat(TextureInternalFormat internalFormat, TextureFormat format, TextureType type);
	void genAndBind();
	void genAndBind(int w, int h);
	void activeAndBind(GLuint position);
	void unbind();
	void setFilterAndWrap(TextureFilter minMag = TextureFilter::Linear, TextureWrapMode wrap = TextureWrapMode::Repeat);
	void destroy();
};

}