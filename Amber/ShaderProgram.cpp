#include "ShaderProgram.h"

#include <memory>

ShaderProgram::ShaderProgram()
	: handle(0)
	, isLinked(false)
{
}

ShaderProgram::~ShaderProgram()
{
}

void ShaderProgram::attachShaderFromFile(ShaderType type, const std::string& filename) const
{
	int compileStatus;

	GLuint shaderID;
	if (type == ShaderType::Vertex)
		shaderID = glCreateShader(GL_VERTEX_SHADER);
	else if (type == ShaderType::Fragment)
		shaderID = glCreateShader(GL_FRAGMENT_SHADER);
	else
		criticalError("Unsupported shader type");

	std::string sourceString = loadFileToString("Shaders/" + filename);
	const char *source = sourceString.c_str();
	glShaderSource(shaderID, 1, &source, nullptr);
	glCompileShader(shaderID);
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		int logLenght = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLenght);
		std::unique_ptr<char[]> log(new char[logLenght]);
		glGetShaderInfoLog(shaderID, logLenght, nullptr, log.get());
		std::string message = "Failed to compile shader: " + filename + "\n";
		criticalError(message.append(log.get()));
	}

	glAttachShader(handle, shaderID);
}

void ShaderProgram::create()
{
	handle = glCreateProgram();
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
	checkInUse();
	int loc = getUniformLocation(name);
	if (loc == -1)
		return;
	glUniform1f(loc, x);
}

void ShaderProgram::setUniform(const std::string& name, float x, float y)
{
	checkInUse();
	int loc = getUniformLocation(name);
	if (loc == -1)
		return;
	glUniform2f(loc, x, y);
}

void ShaderProgram::setUniform(const std::string& name, float x, float y, float z)
{
	checkInUse();
	int loc = getUniformLocation(name);
	if (loc == -1)
		return;
	glUniform3f(loc, x, y, z);
}

void ShaderProgram::setUniform(
	const std::string& name, float x, float y, float z, float w)
{
	checkInUse();
	int loc = getUniformLocation(name);
	if (loc == -1)
		return;
	glUniform4f(loc, x, y, z, w);
}

void ShaderProgram::setUniform(const std::string& name, uint32_t x)
{
	checkInUse();
	int loc = getUniformLocation(name);
	if (loc == -1)
		return;
	glUniform1ui(loc, x);
}

void ShaderProgram::setUniform(const std::string& name, int x)
{
	checkInUse();
	int loc = getUniformLocation(name);
	if (loc == -1)
		return;
	glUniform1i(loc, x);
}

void ShaderProgram::setUniform(const std::string& name, bool x)
{
	checkInUse();
	int loc = getUniformLocation(name);
	if (loc == -1)
		return;
	glUniform1i(loc, (int)x);
}

void ShaderProgram::setUniform(const std::string& name, const Vector2f& v)
{
	checkInUse();
	int loc = getUniformLocation(name);
	if (loc == -1)
		return;
	glUniform2fv(loc, 1, v.data);
}

void ShaderProgram::setUniform(const std::string& name, const Vector3f& v)
{
	checkInUse();
	int loc = getUniformLocation(name);
	if (loc == -1)
		return;
	glUniform3fv(loc, 1, v.data);
}

void ShaderProgram::setUniform(const std::string& name, const Matrix4x4f& m)
{
	checkInUse();
	int loc = getUniformLocation(name);
	if (loc == -1)
		return;
	glUniformMatrix4fv(loc, 1, false, &m[0][0]);
}

void ShaderProgram::setUniform(const std::string& name, const Quaternion& q)
{
	checkInUse();
	int loc = getUniformLocation(name);
	if (loc == -1)
		return;
	glUniform4fv(loc, 1, &q.x);
}

void ShaderProgram::setUniform(const std::string& name, const TransformComponent& t)
{
	checkInUse();
	{
		int loc = getUniformLocation(name + ".position");
		if (loc == -1)
			return;
		glUniform3fv(loc, 1, t.position.data);
	}
	{
		int loc = getUniformLocation(name + ".orientation");
		if (loc == -1)
			return;
		glUniform4fv(loc, 1, &t.orientation.x);
	}
	{
		int loc = getUniformLocation(name + ".scale");
		if (loc == -1)
			return;
		glUniform3fv(loc, 1, t.scale.data);
	}
}

void ShaderProgram::setUniform(const std::string& name, const Color& c)
{
	float r = c.r / 255.0f;
	float g = c.g / 255.0f;
	float b = c.b / 255.0f;
	float a = c.a / 255.0f;

	setUniform(name, r, g, b, a);
}