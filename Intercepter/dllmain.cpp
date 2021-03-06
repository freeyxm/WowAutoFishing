// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "CursorIntercepter.h"

CursorIntercepter g_cursor_intercepter("Intercepter_CursorIntercepter");

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_cursor_intercepter.Attach();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        g_cursor_intercepter.Detach();
        break;
    }
    return TRUE;
}

