#include "stdafx.h"
#include "DLLInjecter.h"
#include <Tlhelp32.h>
#include <iostream>


DLLInjecter::DLLInjecter()
{
}


DLLInjecter::~DLLInjecter()
{
}


bool DLLInjecter::InjectA(int pid, PCSTR dll_path)
{
    PWSTR wpath = (PWSTR)_alloca((lstrlenA(dll_path) + 1) * sizeof(WCHAR));
    wsprintfW(wpath, L"%S", dll_path);
    return InjectW(pid, wpath);
}


bool DLLInjecter::InjectW(int pid, PCWSTR dll_path)
{
    bool bOk = false;
    HANDLE hProcess = NULL;
    HANDLE hThread = NULL;
    LPVOID pRemotePath = NULL;

    __try
    {
        do
        {
            hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, false, pid);
            if (hProcess == NULL)
            {
                printf("Error: OpenProcess %d failed.\n", pid);
                break;
            }

            size_t len = lstrlenW(dll_path) + 1;
            size_t size = len * sizeof(WCHAR);
            pRemotePath = VirtualAllocEx(hProcess, NULL, size, MEM_COMMIT, PAGE_READWRITE);
            if (pRemotePath == NULL)
            {
                printf("Error: VirtualAllocEx failed.\n");
                break;
            }

            if (!WriteProcessMemory(hProcess, pRemotePath, dll_path, size, NULL))
            {
                printf("Error: WriteProcessMemory failed.\n");
                break;
            }

            PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
                GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
            if (pfnThreadRtn == NULL)
            {
                printf("Error: GetProcAddress failed.\n");
                break;
            }

            hThread = CreateRemoteThread(hProcess, NULL, 0, pfnThreadRtn, pRemotePath, 0, NULL);
            if (hThread == NULL)
            {
                printf("Error: CreateRemoteThread failed.\n");
                break;
            }

            WaitForSingleObject(hThread, INFINITE);

            bOk = true;
            printf("Inject success.\n");

        } while (false);
    }
    __finally
    {
        if (pRemotePath != NULL)
        {
            VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        }
        if (hThread != NULL)
        {
            CloseHandle(hThread);
        }
        if (hProcess != NULL)
        {
            CloseHandle(hProcess);
        }
    }

    return bOk;
}


bool DLLInjecter::EjectA(int pid, PCSTR dll_path)
{
    PWSTR wpath = (PWSTR)_alloca((lstrlenA(dll_path) + 1) * sizeof(WCHAR));
    wsprintfW(wpath, L"%S", dll_path);
    return EjectW(pid, wpath);
}


bool DLLInjecter::EjectW(int pid, PCWSTR dll_path)
{
    bool bOk = false;
    HANDLE hSnapshot = NULL;
    HANDLE hProcess = NULL;
    HANDLE hThread = NULL;

    __try
    {
        do
        {
            hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
            if (hSnapshot == NULL)
            {
                printf("Error: CreateToolhelp32Snapshot failed.\n");
                break;
            }

            MODULEENTRY32W me = { sizeof(me) };
            bool bFound = false;
            bool bHasMore = Module32FirstW(hSnapshot, &me);
            while (bHasMore)
            {
                if (lstrcmpiW(me.szModule, dll_path) == 0 ||
                    lstrcmpiW(me.szExePath, dll_path) == 0)
                {
                    bFound = true;
                    break;
                }
                bHasMore = Module32NextW(hSnapshot, &me);
            }
            if (!bFound)
            {
                printf("Error: Module not found.\n");
                break;
            }

            hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION, false, pid);
            if (hProcess == NULL)
            {
                printf("Error: OpenProcess %d failed.\n", pid);
                break;
            }

            PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
                GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "FreeLibrary");
            if (pfnThreadRtn == NULL)
            {
                printf("Error: GetProcAddress failed.\n");
                break;
            }

            hThread = CreateRemoteThread(hProcess, NULL, 0, pfnThreadRtn, me.modBaseAddr, 0, NULL);
            if (hThread == NULL)
            {
                printf("Error: CreateRemoteThread failed.\n");
                break;
            }

            WaitForSingleObject(hThread, INFINITE);

            bOk = true;
            printf("Eject success.\n");

        } while (false);
    }
    __finally
    {
        if (hSnapshot != NULL)
        {
            CloseHandle(hSnapshot);
        }
        if (hThread != NULL)
        {
            CloseHandle(hThread);
        }
        if (hProcess != NULL)
        {
            CloseHandle(hProcess);
        }
    }

    return bOk;
}
