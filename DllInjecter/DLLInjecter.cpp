// Win32Injecter.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <Win32Util/Util/DLLInjecter.h>

void PrintHelp();

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        PrintHelp();
        return -1;
    }

    int pid = ::atoi(argv[1]);
    const char* dll_path = argv[2];
    bool inject = strcmp(argv[3], "true") == 0;

    bool res = false;
    if (inject)
    {
        res = DLLInjecter::Inject(pid, dll_path);
    }
    else
    {
        res = DLLInjecter::Eject(pid, dll_path);
    }

    return res ? 0 : -1;
}

void PrintHelp()
{
    std::cout << "Usage: DLLInjecter <pid> <dll_path>" << std::endl;
}

