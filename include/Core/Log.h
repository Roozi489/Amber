#pragma once

#include <string>
#include <fstream>

namespace Amber
{
enum class Severity
{
	Message,
	Warning,
	Error
};

class Log
{
public:
	static void init();

	static void log(const std::string& message, Severity severity);
	static void log(std::string&& message, Severity severity);

	static void message(const std::string& message);
	static void message(std::string&& message);
	static void warning(const std::string& message);
	static void warning(std::string&& message);
	static void error(const std::string& message);
	static void error(std::string&& message);

	static void clearLogs();

	static constexpr char* logDir = "Logs/";

	static constexpr char* allLogFileName = "all.log";
	static constexpr char* messageLogFileName = "messages.log";
	static constexpr char* warningLogFileName = "warnings.log";
	static constexpr char* errorLogFileName = "errors.log";

	Log() = default;
	~Log() = default;

private:
	static constexpr char* timeFormat = "%H:%M:%S";
	
	static void logOnlyToAll(const std::string& message);
	static void logOnlyToAll(std::string&& message);

	static std::ofstream m_allLogStream;
	static std::ofstream m_messageLogStream;
	static std::ofstream m_warningLogStream;
	static std::ofstream m_errorLogStream;
};

}
