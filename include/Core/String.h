#pragma once
#include <string>
#include <vector>

namespace Amber
{

std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems);
std::vector<std::string> split(const std::string& s, char delim);

std::string toLower(const std::string& text);

template<typename ... Args>
std::string stringFormat(const std::string& format, Args ... args)
{
	std::size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	char buffer[] = new char[size];
	snprintf(buffer, size, format.c_str(), args ...);
	return std::string(buffer, buffer + size - 1);
}

template<typename ... Args>
std::string stringFormat(std::string&& format, Args ... args)
{
	std::size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	char* buffer = new char[size];
	snprintf(buffer, size, format.c_str(), args ...);
	return std::string(buffer, buffer + size - 1);
}

template<typename ... Args>
std::string stringFormat(const char* format, Args ... args)
{
	std::size_t size = snprintf(nullptr, 0, format, args ...) + 1; // Extra space for '\0'
	char* buffer = new char[size];
	snprintf(buffer, size, format, args ...);
	return std::string(buffer, buffer + size - 1);
}

bool compareStringsCaseInsensitive(const std::string& a, const std::string& b);

}