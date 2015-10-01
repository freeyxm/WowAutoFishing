#include "stdafx.h"
#include "Utility.h"
#include <cstdio>
#include <ctime>
#include <cstdarg>


Utility::Utility()
{
}


Utility::~Utility()
{
}

void Utility::printf_t(const char *fmt, ...)
{
	time_t t = ::time(NULL);
	struct tm ti;
	int ret = ::localtime_s(&ti, &t);
	::printf("[%04d-%02d-%02d %02d:%02d:%02d] ", (ti.tm_year + 1900), (ti.tm_mon + 1), ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);

	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	::vprintf(fmt, arg_ptr);
	va_end(arg_ptr);
}
