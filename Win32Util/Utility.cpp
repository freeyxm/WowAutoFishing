#include "stdafx.h"
#include "Utility.h"
#include <cstdio>
#include <ctime>
#include <cstdarg>
#include <sstream>
#include <Windows.h>
#include <io.h>


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

std::wstring Utility::GetErrorMsg(int error_code)
{
    const int size = 256;
    WCHAR buf[size + 1];

    if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        buf,
        size,
        NULL))
    {
        return std::wstring(buf);
    }
    else
    {
        std::wstringstream stream;
        stream << L"Unknown error code " << error_code;
        return stream.str();
    }
}

size_t Utility::Split(string str, std::list<string> &result)
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

bool Utility::GetWndExePath(HWND hwnd, std::wstring &path)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, false, pid);
    if (!hProcess)
    {
        return false;
    }

    wchar_t buf[MAX_PATH + 1];
    DWORD size = MAX_PATH;
    QueryFullProcessImageNameW(hProcess, 0, buf, &size);

    path = buf;

    CloseHandle(hProcess);

    return true;
}

bool Utility::FindLatestFile(const std::wstring& dir_path, std::wstring& file_path)
{
    __time64_t lastest_time = 0;
    std::wstring lastest_name;

    struct _wfinddata_t fileinfo;
    std::wstring path(dir_path + L"\\*");
    intptr_t hFile = 0;
    if ((hFile = _wfindfirst(path.c_str(), &fileinfo)) != -1)
    {
        do
        {
            if ((fileinfo.attrib &  _A_SUBDIR) != 0)
            {
                continue;
            }
            if (lastest_time < fileinfo.time_create)
            {
                lastest_time = fileinfo.time_create;
                lastest_name = fileinfo.name;
            }
        } while (_wfindnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }

    if (lastest_name.empty())
    {
        return false;
    }

    file_path.assign(dir_path).append(L"\\").append(lastest_name);
    return true;
}

std::wstring Utility::StrToWStr(const std::string& str)
{
    PWSTR wstr = (PWSTR)_malloca((lstrlenA(str.c_str()) + 1) * sizeof(WCHAR));
    if (wstr)
    {
        wsprintfW(wstr, L"%S", str.c_str());
        return std::wstring(wstr);
    }
    else
    {
        return std::wstring();
    }
}
