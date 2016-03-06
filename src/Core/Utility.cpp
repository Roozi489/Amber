#include "Core/Utility.h"
#include "Core/Globals.h"

#include <fstream>
#include <sstream>

namespace Amber
{

void log(std::string&& message)
{
	static std::ofstream log_file("log.txt", std::ios::out | std::ios::app);
	log_file << message << std::endl;
}

void log(const std::string& message)
{
	static std::ofstream log_file("log.txt", std::ios::out | std::ios::app);
	log_file << message << std::endl;
}

void clearLog()
{
	std::ofstream log_file("log.txt", std::ios::out | std::ios::trunc);
}

void _assert(bool expression, const char* message, const char* file, int line, const char* function)
{
	if (!expression)
	{
		std::stringstream sstream;
		sstream << "Assert failed: \"" << message << "\"\n" << file << ":" << line << "   -  " << function;
		messageBox("Assert failed", sstream.str());
	}
}

void _assert(bool expression, const std::string& message, const char* file, int line, const char* function)
{
	_assert(expression, message.c_str(), file, line, function);
}

}