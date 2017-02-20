#pragma once

#include <string>
#include <fstream>

namespace Amber
{
enum class Severity
{
	Info,
	Warning,
	Error
};

class Log
{
public:
	static void init();

	static void log(const std::string& message, Severity severity);
	static void log(std::string&& message, Severity severity);

	static void info(const std::string& message);
	static void info(std::string&& message);
	static void warning(const std::string& message);
	static void warning(std::string&& message);
	static void error(const std::string& message);
	static void error(std::string&& message);

	static void clearLogs();

	static constexpr char* logDir = "Logs/";

	static constexpr char* allLogFileName = "all.log";
	static constexpr char* infoLogFileName = "info.log";
	static constexpr char* warningLogFileName = "warnings.log";
	static constexpr char* errorLogFileName = "errors.log";

	Log() = default;
	~Log() = default;

private:
	static constexpr char* TimeFormat = "%d/%m/%Y %H:%M:%S";
	
	static void logOnlyToAll(const std::string& message, Severity severity);
	static void logOnlyToAll(std::string&& message, Severity severity);

	static std::ofstream m_allLogStream;
	static std::ofstream m_infoLogStream;
	static std::ofstream m_warningLogStream;
	static std::ofstream m_errorLogStream;
};

}
