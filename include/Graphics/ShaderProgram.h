#pragma once
#include "Math/Vector.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"
#include "Components/TransformComponent.h"
#include "Core/NonCopyable.h"
#include "Color.h"
#include "OpenGL.h"

#include <unordered_map>
#include <string>

namespace Amber
{

// TODO: geometry shaders
enum class ShaderType
{
	Vertex,
	Geometry,
	Fragment,
};

class ShaderProgram : public NonCopyable
{
public:
	GLuint handle;
	std::string name;
	std::string errorLog;

	ShaderProgram();
	~ShaderProgram();

	void create(const std::string& programName);
	void destroy();

	void link();
	void use() const;
	bool isInUse() const;
	void stopUsing() const;
	void checkInUse() const;

	void attachShaderFromFile(ShaderType type, const std::string& filename);
	void attachShaderFromString(ShaderType type, const std::string& name, const std::string& content);

	GLint getUniformLocation(const std::string& name);
	GLint getUniformLocation(std::string&& name);

	void setUniform(const std::string& name, float x);
	void setUniform(const std::string& name, float x, float y);
	void setUniform(const std::string& name, float x, float y, float z);
	void setUniform(const std::string& name, float x, float y, float z, float w);
	void setUniform(const std::string& name, uint32 x);
	void setUniform(const std::string& name, int x);
	void setUniform(const std::string& name, bool x);
	void setUniform(const std::string& name, const Vector2f& v);
	void setUniform(const std::string& name, const Vector3f& v);
	void setUniform(const std::string& name, const Vector4f& v);
	void setUniform(const std::string& name, const Matrix4x4f& m);
	void setUniform(const std::string& name, const Matrix4x4f m[6]);
	void setUniform(const std::string& name, const Quaternion& t);
	void setUniform(const std::string& name, const TransformComponent& t);
	void setUniform(const std::string& name, const Color& c);
private:
	std::unordered_map<std::string, GLint> m_UniformLocations;

	void compileShader(GLuint shaderHandle, const std::string& name, const char* content);
};

}