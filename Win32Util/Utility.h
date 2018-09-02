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

    static std::wstring GetErrorMsg(int error_code);

    static size_t Split(string str, std::list<string> &result);
};

