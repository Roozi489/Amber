#include "Core/Log.h"
#include "Core/Time.h"
#include "Window/File.h"

namespace Amber
{

std::ofstream Log::m_allLogStream;
std::ofstream Log::m_infoLogStream;
std::ofstream Log::m_warningLogStream;
std::ofstream Log::m_errorLogStream;

void Log::init()
{
	m_allLogStream = std::ofstream(std::string(logDir) + allLogFileName, std::ios::out | std::ios::app);
	m_infoLogStream = std::ofstream(std::string(logDir) + infoLogFileName, std::ios::out | std::ios::app);
	m_warningLogStream = std::ofstream(std::string(logDir) + warningLogFileName, std::ios::out | std::ios::app);
	m_errorLogStream = std::ofstream(std::string(logDir) + errorLogFileName, std::ios::out | std::ios::app);
}

void Log::log(const std::string& message, Severity severity)
{
	switch (severity)
	{
	case Severity::Info:
		Log::info(message);
		break;
	case Severity::Warning:
		Log::warning(message);
		break;
	case Severity::Error:
		Log::error(message);
		break;
	default:
		break;
	}
}

void Log::log(std::string&& message, Severity severity)
{
	switch (severity)
	{
	case Severity::Info:
		Log::info(message);
		break;
	case Severity::Warning:
		Log::warning(message);
		break;
	case Severity::Error:
		Log::error(message);
		break;
	default:
		break;
	}
}

void Log::info(const std::string& message)
{
	m_infoLogStream << Time::localTimeString(TimeFormat) << " - " << message << std::endl;

	logOnlyToAll(message, Severity::Info);
}

void Log::info(std::string&& message)
{
	m_infoLogStream << Time::localTimeString(TimeFormat) << " - " << message << std::endl;

	logOnlyToAll(message, Severity::Info);
}

void Log::warning(const std::string& message)
{
	m_warningLogStream << Time::localTimeString(TimeFormat) << " - " << message << std::endl;

	logOnlyToAll(message, Severity::Warning);
}

void Log::warning(std::string&& message)
{
	m_warningLogStream << Time::localTimeString(TimeFormat) << " - " << message << std::endl;

	logOnlyToAll(message, Severity::Warning);
}

void Log::error(const std::string& message)
{
	m_errorLogStream << Time::localTimeString(TimeFormat) << " - " << message << std::endl;

	logOnlyToAll(message, Severity::Error);
}

void Log::error(std::string&& message)
{
	m_errorLogStream << Time::localTimeString(TimeFormat) << " - " << message << std::endl;

	logOnlyToAll(message, Severity::Error);
}

void Log::clearLogs()
{
	clearFile(std::string(logDir) + allLogFileName);
	clearFile(std::string(logDir) + infoLogFileName);
	clearFile(std::string(logDir) + warningLogFileName);
	clearFile(std::string(logDir) + errorLogFileName);
}

void Log::logOnlyToAll(const std::string& message, Severity severity)
{
	switch (severity)
	{
	case Severity::Info:
		m_allLogStream << Time::localTimeString(TimeFormat) << " - INFO - " << message << std::endl;
		break;
	case Severity::Warning:
		m_allLogStream << Time::localTimeString(TimeFormat) << " - WARN - " << message << std::endl;
		break;
	case Severity::Error:
		m_allLogStream << Time::localTimeString(TimeFormat) << " - ERROR - " << message << std::endl;
		break;
	}
}

void Log::logOnlyToAll(std::string&& message, Severity severity)
{
	switch (severity)
	{
	case Severity::Info:
		m_allLogStream << Time::localTimeString(TimeFormat) << " - INFO - " << message << std::endl;
		break;
	case Severity::Warning:
		m_allLogStream << Time::localTimeString(TimeFormat) << " - WARN - " << message << std::endl;
		break;
	case Severity::Error:
		m_allLogStream << Time::localTimeString(TimeFormat) << " - ERROR - " << message << std::endl;
		break;
	}
}


}
