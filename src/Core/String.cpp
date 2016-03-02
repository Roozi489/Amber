#include "Core/String.h"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace Amber
{

std::string loadFileToString(const std::string& filePath)
{
	std::ifstream file(filePath);
	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string>& elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

std::string toLower(const std::string& text)
{
	std::string result = text;
	std::transform(text.begin(), text.end(), result.begin(), ::tolower);
	return result;
}

bool compareStringsCaseInsensitive(const std::string& a, const std::string& b)
{
	size_t sz = a.size();
	if (b.size() != sz)
		return false;
	for (unsigned int i = 0; i < sz; ++i)
		if (tolower(a[i]) != tolower(b[i]))
			return false;
	return true;
}

}