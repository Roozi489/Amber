#pragma once

#include <string>

namespace Amber
{

std::string loadFileToString(const std::string& filePath);
void writeStringToFile(const std::string& filePath, const std::string& content);

void clearFile(const std::string& filePath);


}