#include "Core/Log.h"
#include "Core/Time.h"
#include "Window/File.h"

namespace Amber
{

std::ofstream Log::m_allLogStream;
std::ofstream Log::m_messageLogStream;
std::ofstream Log::m_warningLogStream;
std::ofstream Log::m_errorLogStream;

void Log::init()
{
	m_allLogStream = std::ofstream(std::string(logDir) + allLogFileName, std::ios::out | std::ios::app);
	m_messageLogStream = std::ofstream(std::string(logDir) + messageLogFileName, std::ios::out | std::ios::app);
	m_warningLogStream = std::ofstream(std::string(logDir) + warningLogFileName, std::ios::out | std::ios::app);
	m_errorLogStream = std::ofstream(std::string(logDir) + errorLogFileName, std::ios::out | std::ios::app);
}

void Log::log(const std::string& message, Severity severity)
{
	switch (severity)
	{
	case Severity::Message:
		Log::message(message);
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
	case Severity::Message:
		Log::message(message);
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

void Log::message(const std::string& message)
{
	m_messageLogStream << Time::localTimeString(timeFormat) << " - " << message << std::endl;

	logOnlyToAll(message);
}

void Log::message(std::string&& message)
{
	m_messageLogStream << Time::localTimeString(timeFormat) << " - " << message << std::endl;

	logOnlyToAll(message);
}

void Log::warning(const std::string& message)
{
	m_warningLogStream << Time::localTimeString(timeFormat) << " - " << message << std::endl;

	logOnlyToAll(message);
}

void Log::warning(std::string&& message)
{
	m_warningLogStream << Time::localTimeString(timeFormat) << " - " << message << std::endl;

	logOnlyToAll(message);
}

void Log::error(const std::string& message)
{
	m_errorLogStream << Time::localTimeString(timeFormat) << " - " << message << std::endl;

	logOnlyToAll(message);
}

void Log::error(std::string&& message)
{
	m_errorLogStream << Time::localTimeString(timeFormat) << " - " << message << std::endl;

	logOnlyToAll(message);
}

void Log::clearLogs()
{
	clearFile(std::string(logDir) + allLogFileName);
	clearFile(std::string(logDir) + messageLogFileName);
	clearFile(std::string(logDir) + warningLogFileName);
	clearFile(std::string(logDir) + errorLogFileName);
}

void Log::logOnlyToAll(const std::string& message)
{
	m_allLogStream << Time::localTimeString(timeFormat) << " - " << message << std::endl;
}

void Log::logOnlyToAll(std::string&& message)
{
	m_allLogStream << Time::localTimeString(timeFormat) << " - " << message << std::endl;
}


}
