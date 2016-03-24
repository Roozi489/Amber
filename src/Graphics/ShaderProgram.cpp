#include "Graphics/ShaderProgram.h"
#include "Core/Utility.h"
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
	else if (type == ShaderType::Fragment)
		shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
	else
		criticalError("Unsupported shader type");

	loadFromString(shaderHandle, name, content.c_str());
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
		std::string message = "Failed to link shader:\n";
		criticalError(message.append(log.get()));
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
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform1f(loc, x);
}

void ShaderProgram::setUniform(const std::string& name, float x, float y)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform2f(loc, x, y);
}

void ShaderProgram::setUniform(const std::string& name, float x, float y, float z)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform3f(loc, x, y, z);
}

void ShaderProgram::setUniform(const std::string& name, float x, float y, float z, float w)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform4f(loc, x, y, z, w);
}

void ShaderProgram::setUniform(const std::string& name, uint32 x)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform1ui(loc, x);
}

void ShaderProgram::setUniform(const std::string& name, int x)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform1i(loc, x);
}

void ShaderProgram::setUniform(const std::string& name, bool x)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform1i(loc, static_cast<int>(x));
}

void ShaderProgram::setUniform(const std::string& name, const Vector2f& v)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform2fv(loc, 1, v.data);
}

void ShaderProgram::setUniform(const std::string& name, const Vector3f& v)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform3fv(loc, 1, v.data);
}

void ShaderProgram::setUniform(const std::string& name, const Vector4f& v)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform4fv(loc, 1, v.data);
}

void ShaderProgram::setUniform(const std::string& name, const Matrix4x4f& m)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniformMatrix4fv(loc, 1, false, m.data);
}

void ShaderProgram::setUniform(const std::string& name, const Quaternion& q)
{
	auto loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform4fv(loc, 1, q.data);
}

void ShaderProgram::setUniform(const std::string& name, const TransformComponent& t)
{
	// TOOD: glBufferSubData
	// TODO: add better string handling
	assert_amber(name == "transform", "Tranform name in shader isn't \"transform\". ");
	auto loc = getUniformLocation("transform.position");
	if (loc == -1)
		log("Uniform: tranform.position not found.\nShaderProgram: " + this->name);
	else
		glUniform3fv(loc, 1, t.position.data);

	loc = getUniformLocation("transform.orientation");
	if (loc == -1)
		log("Uniform: tranform.orientation not found.\nShaderProgram: " + this->name);
	else
		glUniform4fv(loc, 1, t.orientation.data);

	loc = getUniformLocation("transform.scale");
	if (loc == -1)
		log("Uniform: tranform.scale not found.\nShaderProgram: " + this->name);
	else
		glUniform3fv(loc, 1, t.scale.data);
}

void ShaderProgram::setUniform(const std::string& name, const Color& c)
{
	setUniform(name, c.toNormalizedRGBA());
}

void ShaderProgram::loadFromString(GLuint shaderHandle, const std::string& name, const char* content)
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
		writeStringToFile("error.txt", content);
		std::string message = "Failed to compile shader: \"" + name + "\"\n";
		criticalError(message.append(log.get()));
	}

	glAttachShader(handle, shaderHandle);
}

}