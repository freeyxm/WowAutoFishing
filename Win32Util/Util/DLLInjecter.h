#pragma once
#include <Windows.h>

class DLLInjecter
{
public:
    DLLInjecter();
    ~DLLInjecter();

    static bool Inject(int pid, PCSTR dll_path);
    static bool Inject(int pid, PCWSTR dll_path);

    static bool Eject(int pid, PCSTR dll_path);
    static bool Eject(int pid, PCWSTR dll_path);

private:
    static void PrintError(LPCWSTR msg);
    static void PrintLastError(LPCWSTR msg);
};

