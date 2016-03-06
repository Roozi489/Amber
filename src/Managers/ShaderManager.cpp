#include "Managers/ShaderManager.h"
#include "Core/Utility.h"

#include <memory>

namespace Amber
{

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
{
}

void ShaderManager::destroy()
{
	for (auto& pair : m_shaderPrograms)
		pair.second->destroy();

	m_shaderPrograms.clear();
}

ShaderProgram* ShaderManager::createProgram(const std::string& programName, const std::string& vertexShaderFilename)
{
	std::unique_ptr<ShaderProgram> program = std::make_unique<ShaderProgram>();
	program->create(programName);
	program->attachShaderFromFile(ShaderType::Vertex, vertexShaderFilename);
	program->link();

	return (m_shaderPrograms[programName] = std::move(program)).get();
}

ShaderProgram* ShaderManager::createProgram(const std::string& programName, const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename)
{
	std::unique_ptr<ShaderProgram> program = std::make_unique<ShaderProgram>();
	program->create(programName);
	program->attachShaderFromFile(ShaderType::Vertex, vertexShaderFilename);
	program->attachShaderFromFile(ShaderType::Fragment, fragmentShaderFilename);
	program->link();

	return (m_shaderPrograms[programName] = std::move(program)).get();
}

ShaderProgram* ShaderManager::getShaderProgram(const std::string& programName)
{
	assert_amber(m_shaderPrograms.find(programName) != m_shaderPrograms.end(), "ShaderProgram \"" + programName + "\" not found.");
	return m_shaderPrograms[programName].get();
}

}