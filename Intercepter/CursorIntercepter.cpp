#include "stdafx.h"
#include "CursorIntercepter.h"
#include <Windows.h>
#include <Win32Util/ShareMemory.h>
#include "detours/include/detours.h"


BOOL(WINAPI * pGetCursorPos)(_Out_ LPPOINT lpPoint) = GetCursorPos;


CursorIntercepter::CursorIntercepter(const std::string& name)
    : m_share_memory(name)
{
    auto func = [=](LPPOINT lpPoint)
    {
        if (lpPoint)
        {
            if (m_share_memory.Lock())
            {
                ShareData* data = (ShareData*)m_share_memory.GetBuf();
                *lpPoint = data->cursor_pos;
                m_share_memory.Unlock();
            }
        }
        return true;
    };
    m_fun_get_cursor_pos = &func;
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
    long ret = DetourAttach(&(PVOID&)pGetCursorPos, m_fun_get_cursor_pos);
    return ret == NO_ERROR;
}


bool CursorIntercepter::DetachFunctions()
{
    long ret = DetourDetach(&(PVOID&)pGetCursorPos, m_fun_get_cursor_pos);
    return ret == NO_ERROR;
}

