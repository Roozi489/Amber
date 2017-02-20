#include "Graphics/GL/CommandQueue.h"

namespace Amber
{

UniformValue::UniformValue(std::string name, bool value)
	: name(name)
	, type(UniformType::Bool)
{
	boolean = value;
}

UniformValue::UniformValue(std::string name, int32 value)
	: name(name)
	, type(UniformType::Int32)
{
	integer = value;
}

UniformValue::UniformValue(std::string name, float value)
	: name(name)
	, type(UniformType::Float)
{
	single = value;
}

UniformValue::UniformValue(std::string name, const Vector2f& value)
	: name(name)
	, type(UniformType::Vec2)
{
	vec2 = value;
}

UniformValue::UniformValue(std::string name, const Vector3f& value)
	: name(name)
	, type(UniformType::Vec3)
{
	vec3 = value;
}

UniformValue::UniformValue(std::string name, const Vector4f& value)
	: name(name)
	, type(UniformType::Vec4)
{
	vec4 = value;
}

UniformValue::UniformValue(std::string name, const Matrix3x3f& value)
	: name(name)
	, type(UniformType::Mat3)
{
	mat3 = value;
}

UniformValue::UniformValue(std::string name, const Matrix4x4f& value)
	: name(name)
	, type(UniformType::Mat4)
{
	mat4 = value;
}

UniformValue::UniformValue(const UniformValue& other)
	: name(other.name)
	, type(other.type)
{
	mat4 = other.mat4;
}

void Batch::AddUniform(UniformValue&& value)
{
	uniforms.push_back(std::move(value));
}

void CommandQueue::AddUniform(UniformValue&& value)
{
	uniforms.push_back(std::move(value));
}

}