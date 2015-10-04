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

int Utility::Split(string str, std::list<string> &result)
{
	result.clear();
	size_t sp = 0, ep = 0;
	do
	{
		size_t pos = str.find_first_not_of(' ', sp);
		if (pos != string::npos)
		{
			sp = pos;
		}
		ep = str.find_first_of(' ', sp + 1);
		if (ep == string::npos)
		{
			result.push_back(str.substr(sp));
			break;
		}

		result.push_back(str.substr(sp, ep - sp));
		sp = ep + 1;
	} while (true);

	return result.size();
}