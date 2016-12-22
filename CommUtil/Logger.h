#pragma once
#include <cstdarg>

namespace comm_util
{

class Logger
{
public:
	enum class LogLevel
	{
		Debug,
		Info,
		Error,
	};

public:
	Logger();
	~Logger();

	static LogLevel GetLogLevel();
	static void SetLogLevel(LogLevel level);

	static void Log(LogLevel level, const char *format, ...);
	static void LogDebug(const char *format, ...);
	static void LogInfo(const char *format, ...);
	static void LogError(const char *format, ...);

private:
	static LogLevel m_logLevel;
};

} // end namespace comm_util
