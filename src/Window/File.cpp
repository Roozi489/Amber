#include "Window/File.h"
#include "Window/Window.h"

#include <fstream>
#include <sstream>

namespace Amber
{


std::vector<char> readFile(const std::string& filePath)
{
	std::ifstream filestream(filePath, std::ios::ate | std::ios::binary);

	if (!filestream.is_open())
		criticalError("Failed to open file: " + filePath);

	size_t fileSize = static_cast<size_t>(filestream.tellg());
	std::vector<char> buffer(fileSize);

	filestream.seekg(0);
	filestream.read(buffer.data(), fileSize);
	filestream.close();

	return buffer;
}

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