#pragma once
#include <string>
#include <list>

using std::string;

class Utility
{
public:
	Utility();
	~Utility();

	static void printf_t(const char *fmt, ...);

	static int Split(string str, std::list<string> &result);
};

