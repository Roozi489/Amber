#pragma once
#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"
#include "TransformComponent.h"
#include "Color.h"
#include "OpenGL.h"
#include "NonCopyable.h"

#include <unordered_map>
#include <string>

enum class ShaderType
{
	Vertex,
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

	int getUniformLocation(const std::string& name);

	void setUniform(const std::string& name, float x);
	void setUniform(const std::string& name, float x, float y);
	void setUniform(const std::string& name, float x, float y, float z);
	void setUniform(const std::string& name, float x, float y, float z, float w);
	void setUniform(const std::string& name, uint32_t x);
	void setUniform(const std::string& name, int x);
	void setUniform(const std::string& name, bool x);
	void setUniform(const std::string& name, const Vector2f& v);
	void setUniform(const std::string& name, const Vector3f& v);
	void setUniform(const std::string& name, const Vector4f& v);
	void setUniform(const std::string& name, const Matrix4x4f& m);
	void setUniform(const std::string& name, const Quaternion& t);
	void setUniform(const std::string& name, const TransformComponent& t);
	void setUniform(const std::string& name, const Color& c);
private:
	std::unordered_map<std::string, int> m_UniformLocations;

	void loadFromString(GLuint shaderHandle, const std::string& name, const char* content);
};
