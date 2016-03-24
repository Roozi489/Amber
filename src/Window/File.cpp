#include "Window/File.h"
#include "Window/Window.h"

#include <fstream>
#include <sstream>

namespace Amber
{

std::string loadFileToString(const std::string& filePath)
{
	std::ifstream filestream(filePath);
	if (!filestream.is_open())
		criticalError("Failed to open file: " + filePath);
	std::stringstream ss;
	ss << filestream.rdbuf();
	return ss.str();
}

void writeStringToFile(const std::string& filePath, const std::string& content)
{
	std::ofstream file(filePath);
	file << content;
}

void clearFile(const std::string& filePath)
{
	std::ofstream file(filePath, std::ios::out | std::ios::trunc);
}

}