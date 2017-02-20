#pragma once
#include "OpenGL.h"
#include "Graphics/Color.h"
#include "Graphics/GL/Mesh.h"
#include "Math/Quaternion.h"

#include <unordered_map>

namespace Amber
{
class ShaderProgram;

enum class CullFace : uint16
{
	Front = GL_FRONT,
	Back = GL_BACK,
	FrontAndBack = GL_FRONT_AND_BACK,
	None = GL_NONE
};

enum class ClearBuffer : uint32
{
	ColorBuffer = GL_COLOR_BUFFER_BIT,
	DepthBuffer = GL_DEPTH_BUFFER_BIT,
	ColorAndDepthBuffer = ColorBuffer | DepthBuffer,
};

enum class BlendFactor
{
	Zero = GL_ZERO,
	One = GL_ONE,
	SourceColor = GL_SRC_COLOR,
	OneMinusSourceColor = GL_ONE_MINUS_SRC_COLOR,
	DestinationColor = GL_DST_COLOR,
	OneMinusDestinationColor = GL_ONE_MINUS_DST_COLOR,
	SourceAlpha = GL_SRC_ALPHA,
	OneMinusSourceAlpha = GL_ONE_MINUS_SRC_ALPHA,
	DestinationAlpha = GL_DST_ALPHA,
	OneMinusDestinationAlpha = GL_ONE_MINUS_DST_ALPHA,
	ConstantColor = GL_CONSTANT_COLOR,
	OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,
	ConstantAlpha = GL_CONSTANT_ALPHA,
	OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,
	SourceAlphaSaturate = GL_SRC_ALPHA_SATURATE,
	Source1Color = GL_SRC1_COLOR,
	OneMinusSource1Color = GL_ONE_MINUS_SRC1_COLOR,
	Source1Alpha = GL_SRC1_ALPHA,
	OneMinusSource1Alpha = GL_ONE_MINUS_SRC1_ALPHA
};

struct CommandQueueSettings
{
	// TODO: add rendertarget
	bool writeDepth = true;
	bool depthTest = true;
	bool blending = true;
	BlendFactor sourceBlendFactor = BlendFactor::SourceAlpha;
	BlendFactor destinationBlendFactor = BlendFactor::OneMinusSourceAlpha;
	CullFace cullFace = CullFace::Back;
	ClearBuffer clearBufferFlags = ClearBuffer::ColorAndDepthBuffer;
	int viewportWidth;
	int viewportHeight;
};

enum class UniformType
{
	Bool,
	Int32,
	Float,
	Vec2,
	Vec3,
	Vec4,
	Mat3,
	Mat4,
	Sampler
};

struct UniformValue
{
	std::string name;
	UniformType type = UniformType::Vec3;
	// Ideally should be replaced by something like boost::variant
	union
	{
		bool boolean;
		int32 integer;
		float single;
		Vector2f vec2;
		Vector3f vec3;
		Vector4f vec4;
		Matrix3x3f mat3;
		Matrix4x4f mat4;
	};

	UniformValue(const UniformValue& other);
	UniformValue(std::string name, bool value);
	UniformValue(std::string name, int32 value);
	UniformValue(std::string name, float value);
	UniformValue(std::string name, const Vector2f& value);
	UniformValue(std::string name, const Vector3f& value);
	UniformValue(std::string name, const Vector4f& value);
	UniformValue(std::string name, const Matrix3x3f& value);
	UniformValue(std::string name, const Matrix4x4f& value);
};

struct Batch
{
	bool useInstancing = false;
	int count = 0;
	Mesh* mesh;
	ShaderProgram* shaderProgram;
	std::vector<Matrix4x4f> mvps;
	std::vector<Quaternion> orientations;
	std::vector<Color> colors;

	std::vector<UniformValue> uniforms;

	void AddUniform(UniformValue&& value);
};

struct CommandQueue
{
	CommandQueueSettings settings;
	std::unordered_map<MeshID, Batch> batches;
	std::vector<UniformValue> uniforms;

	CommandQueue() = default;
	~CommandQueue() = default;

	void AddUniform(UniformValue&& value);
};

}
