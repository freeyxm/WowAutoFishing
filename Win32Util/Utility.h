#pragma once
#include <string>
#include <list>
#include <Windows.h>

using std::string;

class Utility
{
public:
    Utility();
    ~Utility();

    static void printf_t(const char *fmt, ...);

    static std::wstring GetErrorMsg(int error_code);

    static size_t Split(string str, std::list<string> &result);

    static bool GetWndExePath(HWND hwnd, std::wstring &path);

    static bool FindLatestFile(const std::wstring& dir_path, std::wstring& file_path);

    static std::wstring StrToWStr(const std::string& src);
};

