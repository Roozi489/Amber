#include "ShaderManager.h"
#include "Utility.h"

#include <memory>

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
{
}

void ShaderManager::destroy()
{
	for (auto& pair : mShaderPrograms)
		pair.second->destroy();
	
	mShaderPrograms.clear();
}

ShaderProgram* ShaderManager::createProgram(const std::string& programName, const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename)
{
	std::unique_ptr<ShaderProgram> program = std::make_unique<ShaderProgram>();
	program->create(programName);
	program->attachShaderFromFile(ShaderType::Vertex, vertexShaderFilename);
	program->attachShaderFromFile(ShaderType::Fragment, fragmentShaderFilename);
	program->link();

    mShaderPrograms[programName] = std::move(program);

    return program.get();
}

ShaderProgram* ShaderManager::getShaderProgram(const std::string& shaderName)
{
	assert_amber(mShaderPrograms.find(shaderName) != mShaderPrograms.end(), "ShaderProgram \"" + shaderName + "\" not found.");
    return mShaderPrograms[shaderName].get();
}
