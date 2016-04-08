#pragma once
#include "Graphics/ShaderProgram.h"

#include <string>
#include <unordered_map>
#include <memory>

namespace Amber
{

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	void destroy();

	ShaderProgram* createProgram(const std::string& programName, const std::string& vsFilename);
	ShaderProgram* createProgram(const std::string& programName, const std::string& vsFilename, const std::string& fSFilename);
	ShaderProgram* createProgram(const std::string& programName, const std::string& vsFilename, const std::string& gsFilename, const std::string& fsFilename);
	ShaderProgram* getShaderProgram(const std::string& programName);

private:
	std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> m_shaderPrograms;
};

}