#include "stdafx.h"
#include "CursorIntercepter.h"
#include <Windows.h>
#include <Win32Util/ShareMemory.h>
#include "detours/include/detours.h"


BOOL(WINAPI * pGetCursorPos)(_Out_ LPPOINT lpPoint) = GetCursorPos;
BOOL(WINAPI * pSetCursorPos)(_In_ int X, _In_ int Y) = SetCursorPos;

CursorIntercepter* CursorIntercepter::m_instance = NULL;

CursorIntercepter::CursorIntercepter(const std::string& name)
    : m_share_memory(name)
{
    m_instance = this;
}


CursorIntercepter::~CursorIntercepter()
{
}


bool CursorIntercepter::Attach()
{
    OpenLog(m_share_memory.GetName() + ".log");

    if (!m_share_memory.Create(sizeof(ShareData)))
    {
        PrintLog(std::string("[Attach] Create share memory failed, code = ").append(std::to_string(GetLastError())));
        return false;
    }

    ShareData *data = (ShareData*)m_share_memory.GetBuf();
    data->cursor_pos.x = 0;
    data->cursor_pos.y = 0;

    if (!IntercepterBase::Attach())
    {
        return false;
    }

    return true;
}


bool CursorIntercepter::Detach()
{
    IntercepterBase::Detach();

    m_share_memory.Close();

    CloseLog();

    return true;
}


bool CursorIntercepter::AttachFunctions()
{
    DetourAttach(&(PVOID&)pGetCursorPos, MyGetCursorPos);
    DetourAttach(&(PVOID&)pSetCursorPos, MySetCursorPos);
    return true;
}


bool CursorIntercepter::DetachFunctions()
{
    DetourDetach(&(PVOID&)pGetCursorPos, MyGetCursorPos);
    DetourDetach(&(PVOID&)pSetCursorPos, MySetCursorPos);
    return true;
}


bool CursorIntercepter::MyGetCursorPos(_Out_ LPPOINT lpPoint)
{
    DWORD cur_pid = GetCurrentProcessId();
    DWORD wnd_pid = 0;
    HWND hwnd = GetActiveWindow();
    GetWindowThreadProcessId(hwnd, &wnd_pid);

    if (cur_pid == wnd_pid)
    {
        return pGetCursorPos(lpPoint);
    }
    else
    {
        if (m_instance->m_share_memory.Lock())
        {
            ShareData* data = (ShareData*)m_instance->m_share_memory.GetBuf();
            *lpPoint = data->cursor_pos;
            m_instance->m_share_memory.Unlock();
        }
        return true;
    }
}


bool CursorIntercepter::MySetCursorPos(int x, int y)
{
    DWORD cur_pid = GetCurrentProcessId();
    DWORD wnd_pid = 0;
    HWND hwnd = GetActiveWindow();
    GetWindowThreadProcessId(hwnd, &wnd_pid);

    if (cur_pid == wnd_pid)
    {
        return pSetCursorPos(x, y);
    }
    else
    {
        // forbid SetCursorPos
        return true;
    }
}

