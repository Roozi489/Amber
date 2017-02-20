#pragma once

#include <vector>
#include <string>

namespace Amber
{
std::vector<char> readFile(const std::string& filePath);
std::string loadFileToString(const std::string& filePath);
void writeStringToFile(const std::string& filePath, const std::string& content);

void clearFile(const std::string& filePath);


}