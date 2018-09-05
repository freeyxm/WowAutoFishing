#include "stdafx.h"
#include "MouseBackground.h"
#include "ShareMemory.h"
#include "DLLInjecter.h"
#include <Intercepter/CursorIntercepter.h>
#include <iostream>


MouseBackground::MouseBackground(HWND hwnd)
    : m_hwnd(hwnd)
    , m_pid(0)
    , m_share_memory(NULL)
{
    m_cursor_pos.x = 0;
    m_cursor_pos.y = 0;
}


MouseBackground::~MouseBackground()
{
    Close();
}


bool MouseBackground::Init()
{
    Close();

    m_pid = 0;
    GetWindowThreadProcessId(m_hwnd, &m_pid);
    if (m_pid == 0)
    {
        std::cout << "Error: GetWindowThreadProcessId failed! " << GetLastError() << std::endl;
        return false;
    }

    { // Intercepter.dll need put in the same dir with exe.
        const int size = 255;
        TCHAR buf[size + 1] = { 0 };
        GetModuleFileName(NULL, buf, size);

        m_dll_path = buf;
        size_t pos = m_dll_path.find_last_of(L'\\');
        if (pos != std::string::npos)
        {
            m_dll_path = m_dll_path.substr(0, pos);
        }
        m_dll_path.append(L"\\Intercepter.dll");
    }

    DLLInjecter::Eject(m_pid, m_dll_path.c_str());

    if (!DLLInjecter::Inject(m_pid, m_dll_path.c_str()))
    {
        std::cout << "Error: Inject dll failed! " << std::endl;
        return false;
    }

    if (m_share_memory)
    {
        delete m_share_memory;
        m_share_memory = NULL;
    }
    m_share_memory = new ShareMemory("Intercepter_CursorIntercepter");
    if (!m_share_memory->Open(sizeof(CursorIntercepter::ShareData), FILE_MAP_WRITE))
    {
        std::cout << "Error: Open ShareMemory failed! " << GetLastError() << std::endl;
        return false;
    }

    return true;
}


void MouseBackground::Close()
{
    if (m_share_memory)
    {
        delete m_share_memory;
        m_share_memory = NULL;
    }

    if (m_pid)
    {
        DLLInjecter::Eject(m_pid, m_dll_path.c_str());
        m_pid = 0;
    }
}


bool MouseBackground::GetCursorPos(POINT &point)
{
    point = m_cursor_pos;
    return true;
}


void MouseBackground::SetCursorPos(const POINT &point)
{
    this->SetCursorPos(point.x, point.y);
}


void MouseBackground::SetCursorPos(int x, int y)
{
    m_cursor_pos.x = x;
    m_cursor_pos.y = y;

    if (m_share_memory && m_share_memory->Lock())
    {
        POINT* cursor_pos = &((CursorIntercepter::ShareData*)m_share_memory->GetBuf())->cursor_pos;
        cursor_pos->x = x;
        cursor_pos->y = y;
        m_share_memory->Unlock();
    }

    POINT client_pos = m_cursor_pos;
    ::ScreenToClient(m_hwnd, &client_pos);

    LPARAM lParam = MAKELPARAM(client_pos.x, client_pos.y);
    ::PostMessage(m_hwnd, WM_MOUSEMOVE, 0, lParam);
}


void MouseBackground::ClickLeftButton()
{
    LPARAM lParam = MAKELPARAM(m_cursor_pos.x, m_cursor_pos.y);
    ::PostMessage(m_hwnd, WM_LBUTTONDOWN, 0, lParam);
    ::PostMessage(m_hwnd, WM_LBUTTONUP, 0, lParam);
}


void MouseBackground::ClickRightButton()
{
    LPARAM lParam = MAKELPARAM(m_cursor_pos.x, m_cursor_pos.y);
    ::PostMessage(m_hwnd, WM_RBUTTONDOWN, 0, lParam);
    ::PostMessage(m_hwnd, WM_RBUTTONUP, 0, lParam);
}
