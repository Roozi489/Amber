#include "ShaderProgram.h"

#include <fstream>
#include <sstream>
#include <memory>

ShaderProgram::ShaderProgram()
	: handle(0)
	, isLinked(false)
{
}

ShaderProgram::~ShaderProgram()
{
}

void ShaderProgram::attachShaderFromFile(ShaderType type, const std::string& filename)
{
	std::stringstream content;
	std::ifstream fileStream("Shaders/" + filename);
	std::string line;
	while (std::getline(fileStream, line))
	{
		if (line.find("#include") != std::string::npos)
		{
			auto spaceSplit = split(line, '\"');
			if (spaceSplit.size() == 2)
			{
				std::string includedContent = loadFileToString("Shaders/" + spaceSplit[1] + ".glsl");
				content << includedContent;
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

	return (currentProgram == (int)handle);
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

void ShaderProgram::bindAttribLocation(int location, const std::string& name)
{
	glBindAttribLocation(handle, location, name.c_str());
	mAttribLocations.insert(std::make_pair(name, location));
}

int ShaderProgram::getAttribLocation(const std::string& name)
{
	if (mAttribLocations.find(name) != mAttribLocations.end())
		return mAttribLocations[name];

	int loc = glGetAttribLocation(handle, name.c_str());
	mAttribLocations.insert(std::make_pair(name, loc));
	return loc;
}

int ShaderProgram::getUniformLocation(const std::string& name)
{
	if (mUniformLocations.find(name) != mUniformLocations.end())
		return mUniformLocations[name];

	int loc = glGetUniformLocation(handle, name.c_str());
	mAttribLocations.insert(std::make_pair(name, loc));
	return loc;
}

void ShaderProgram::setUniform(const std::string& name, float x)
{
	int loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform1f(loc, x);
}

void ShaderProgram::setUniform(const std::string& name, float x, float y)
{
	int loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform2f(loc, x, y);
}

void ShaderProgram::setUniform(const std::string& name, float x, float y, float z)
{
	int loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform3f(loc, x, y, z);
}

void ShaderProgram::setUniform(const std::string& name, float x, float y, float z, float w)
{
	int loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform4f(loc, x, y, z, w);
}

void ShaderProgram::setUniform(const std::string& name, uint32_t x)
{
	int loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform1ui(loc, x);
}

void ShaderProgram::setUniform(const std::string& name, int x)
{
	int loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform1i(loc, x);
}

void ShaderProgram::setUniform(const std::string& name, bool x)
{
	int loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform1i(loc, static_cast<int>(x));
}

void ShaderProgram::setUniform(const std::string& name, const Vector2f& v)
{
	int loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform2fv(loc, 1, v.data);
}

void ShaderProgram::setUniform(const std::string& name, const Vector3f& v)
{
	int loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform3fv(loc, 1, v.data);
}

void ShaderProgram::setUniform(const std::string& name, const Vector4f& v)
{
	int loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform4fv(loc, 1, v.data);
}

void ShaderProgram::setUniform(const std::string& name, const Matrix4x4f& m)
{
	int loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniformMatrix4fv(loc, 1, false, m.data);
}

void ShaderProgram::setUniform(const std::string& name, const Quaternion& q)
{
	int loc = getUniformLocation(name);
	if (loc == -1)
		log("Uniform: " + name + " not found.\nShaderProgram: " + this->name);
	glUniform4fv(loc, 1, q.data);
}

void ShaderProgram::setUniform(const std::string& name, const TransformComponent& t)
{
	int loc = getUniformLocation(name + ".position");
	if (loc == -1)
		log("Uniform: " + name + ".position not found.\nShaderProgram: " + this->name);
	glUniform3fv(loc, 1, t.position.data);

	loc = getUniformLocation(name + ".orientation");
	if (loc == -1)
		log("Uniform: " + name + ".orientation not found.\nShaderProgram: " + this->name);
	glUniform4fv(loc, 1, t.orientation.data);

	loc = getUniformLocation(name + ".scale");
	if (loc == -1)
		log("Uniform: " + name + ".scale not found.\nShaderProgram: " + this->name);
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
		std::string message = "Failed to compile shader: \"" + name + "\"\n";
		criticalError(message.append(log.get()).append(content));
	}

	glAttachShader(handle, shaderHandle);
}
