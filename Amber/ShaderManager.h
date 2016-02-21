#pragma once
#include "ShaderProgram.h"

#include <string>
#include <unordered_map>
#include <memory>

class ShaderManager
{
public:
    ShaderManager();
    ~ShaderManager();

	void destroy();

	ShaderProgram& createProgram(const std::string& shaderName, const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename);
	ShaderProgram& getShaderProgram(const std::string& programName);

private:
    std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> mShaderPrograms;
};

