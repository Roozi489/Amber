#include "Core/Utility.h"
#include "Core/Globals.h"

#include <sstream>

namespace Amber
{

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