#include "Logger.h"
#include <cstdio>


namespace comm_util
{

#define VPRINTF_S(format) \
{\
	va_list ap;\
	va_start(ap, format);\
	vprintf_s(format, ap);\
	va_end(ap);\
}

Logger::LogLevel Logger::m_log_level = Logger::LogLevel::Error;

Logger::Logger()
{
}

Logger::~Logger()
{
}

Logger::LogLevel Logger::GetLogLevel()
{
	return m_log_level;
}

void Logger::SetLogLevel(LogLevel level)
{
	m_log_level = level;
}

void Logger::Log(LogLevel level, const char *format, ...)
{
	if (m_log_level >= level)
	{
		VPRINTF_S(format);
	}
}

void Logger::LogDebug(const char *format, ...)
{
	if (m_log_level >= LogLevel::Debug)
	{
		VPRINTF_S(format);
	}
}

void Logger::LogInfo(const char *format, ...)
{
	if (m_log_level >= LogLevel::Info)
	{
		VPRINTF_S(format);
	}
}

void Logger::LogError(const char *format, ...)
{
	if (m_log_level >= LogLevel::Error)
	{
		VPRINTF_S(format);
	}
}

} // end namespace comm_util
