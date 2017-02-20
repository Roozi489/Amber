#include "Graphics/GL/ShaderProgram.h"
#include "Core/Utility.h"
#include "Core/Log.h"
#include "Core/String.h"
#include "Window/File.h"

#include <fstream>
#include <sstream>
#include <memory>

namespace Amber
{

ShaderProgram::ShaderProgram()
{
}

ShaderProgram::~ShaderProgram()
{
}

void ShaderProgram::attachShaderFromFile(ShaderType type, const std::string& filename)
{
	std::stringstream content;
	std::ifstream filestream("Shaders/" + filename);
	if (!filestream.is_open())
		criticalError("Failed to open file: " + filename);

	std::string line;
	while (std::getline(filestream, line))
	{
		if (line.find("#include") != std::string::npos)
		{
			auto spaceSplit = split(line, '\"');
			if (spaceSplit.size() >= 2)
			{
				std::string includedContent = loadFileToString("Shaders/" + spaceSplit[1] + ".glsl");
				content << includedContent << std::endl;
			}
		}
		else
		{
			content << line << std::endl;
		}
	}
	attachShaderFromString(type, filename, content.str());
}

void ShaderProgram::attachShaderFromString(ShaderType type, const std::string& name, const std::string& content)
{
	GLuint shaderHandle;
	if (type == ShaderType::Vertex)
		shaderHandle = glCreateShader(GL_VERTEX_SHADER);
	else if (type == ShaderType::Geometry)
		shaderHandle = glCreateShader(GL_GEOMETRY_SHADER);
	else if (type == ShaderType::Fragment)
		shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
	else
		criticalError("Unsupported shader type");

	compileShader(shaderHandle, name, content.c_str());

	glAttachShader(handle, shaderHandle);
}

void ShaderProgram::create(const std::string& programName)
{
	handle = glCreateProgram();
	name = programName;
}

void ShaderProgram::destroy()
{
	glDeleteProgram(handle);
}

void ShaderProgram::use() const
{
	if (!isInUse())
		glUseProgram(handle);
}

bool ShaderProgram::isInUse() const
{
	int currentProgram = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

	return currentProgram == (int)handle;
}

void ShaderProgram::stopUsing() const
{
	glUseProgram(0);
}

void ShaderProgram::checkInUse() const
{
	if (!isInUse())
		criticalError("ShaderProgram not in use.");
}

void ShaderProgram::link()
{
	int compileStatus;
	glLinkProgram(handle);
	glGetProgramiv(handle, GL_LINK_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		int log_len = 0;
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_len);
		std::unique_ptr<char[]> log(new char[log_len]);
		glGetProgramInfoLog(handle, log_len, nullptr, log.get());
		criticalError(stringFormat("Failed to link shader: %s\nReason: %s", name.c_str(), log.get()));
	}
}

GLint ShaderProgram::getUniformLocation(const std::string& name)
{
	if (m_UniformLocations.find(name) != m_UniformLocations.end())
		return m_UniformLocations[name];

	GLint loc = glGetUniformLocation(handle, name.c_str());
	m_UniformLocations.insert(std::make_pair(name, loc));
	return loc;
}

GLint ShaderProgram::getUniformLocation(std::string&& name)
{
	if (m_UniformLocations.find(name) != m_UniformLocations.end())
		return m_UniformLocations[name];

	GLint loc = glGetUniformLocation(handle, name.c_str());
	m_UniformLocations.insert(std::make_pair(name, loc));
	return loc;
}

void ShaderProgram::setUniform(const std::string& name, float x)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		Log::warning("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform1f(loc, x);
}

void ShaderProgram::setUniform(const std::string& name, float x, float y)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		Log::warning("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform2f(loc, x, y);
}

void ShaderProgram::setUniform(const std::string& name, float x, float y, float z)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		Log::warning("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform3f(loc, x, y, z);
}

void ShaderProgram::setUniform(const std::string& name, float x, float y, float z, float w)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		Log::warning("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform4f(loc, x, y, z, w);
}

void ShaderProgram::setUniform(const std::string& name, uint32 x)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		Log::warning("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform1ui(loc, x);
}

void ShaderProgram::setUniform(const std::string& name, int x)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		Log::warning("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	
	glUniform1i(loc, x);
}

void ShaderProgram::setUniform(const std::string& name, bool x)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		Log::warning("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform1i(loc, static_cast<int>(x));
}

void ShaderProgram::setUniform(const std::string& name, const Vector2f& v)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		Log::warning("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform2fv(loc, 1, v.data);
}

void ShaderProgram::setUniform(const std::string& name, const Vector3f& v)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		Log::warning("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform3fv(loc, 1, v.data);
}

void ShaderProgram::setUniform(const std::string& name, const Vector4f& v)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		Log::warning("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform4fv(loc, 1, v.data);
}

void ShaderProgram::setUniform(const std::string& name, const Matrix3x3f& m)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		Log::warning("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniformMatrix3fv(loc, 1, false, m.data);
}

void ShaderProgram::setUniform(const std::string& name, const Matrix4x4f& m)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		Log::warning("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniformMatrix4fv(loc, 1, false, m.data);
}

void ShaderProgram::setUniform(const std::string& name, const Quaternion& q)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		Log::warning("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform4fv(loc, 1, q.data);
}

void ShaderProgram::setUniform(const std::string& name, const TransformComponent& t)
{
	// TOOD: glBufferSubData
	// TODO: add better string handling
	auto loc = getUniformLocation(stringFormat("%s.position", name.c_str()));
	if (loc == -1)
		Log::warning(stringFormat("Uniform: %s.position not found.\nShaderProgram: %s", name.c_str(), this->name));
	else
		glUniform3fv(loc, 1, t.position.data);

	loc = getUniformLocation(stringFormat("%s.orientation", name.c_str()));
	if (loc == -1)
		Log::warning(stringFormat("Uniform: %s.orientation not found.\nShaderProgram: %s", name.c_str(), this->name));
	else
		glUniform4fv(loc, 1, t.orientation.data);

	loc = getUniformLocation(stringFormat("%s.scale", name.c_str()));
	if (loc == -1)
		Log::warning(stringFormat("Uniform: %s.scale not found.\nShaderProgram: %s", name.c_str(), this->name));
	else
		glUniform3fv(loc, 1, t.scale.data);
}

void ShaderProgram::setUniform(const std::string& name, const Color& c)
{
	setUniform(name, c.rgba);
}

void ShaderProgram::compileShader(GLuint shaderHandle, const std::string& name, const char* content)
{
	glShaderSource(shaderHandle, 1, &content, nullptr);
	glCompileShader(shaderHandle);
	int compileStatus;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		int logLenght = 0;
		glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLenght);
		std::unique_ptr<char[]> log(new char[logLenght]);
		glGetShaderInfoLog(shaderHandle, logLenght, nullptr, log.get());
		writeStringToFile("Logs/shader_compile_error.log", content);
		criticalError(stringFormat("Failed to compile shader: %s\nReason: %s", name.c_str(), log.get()));
	}

}

}