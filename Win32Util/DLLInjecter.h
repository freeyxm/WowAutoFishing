#pragma once
#include <Windows.h>

class DLLInjecter
{
public:
    DLLInjecter();
    ~DLLInjecter();

    static bool InjectA(int pid, PCSTR dll_path);
    static bool InjectW(int pid, PCWSTR dll_path);

    static bool EjectA(int pid, PCSTR dll_path);
    static bool EjectW(int pid, PCWSTR dll_path);
};

