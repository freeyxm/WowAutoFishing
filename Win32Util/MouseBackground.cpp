#include "stdafx.h"
#include "MouseBackground.h"


MouseBackground::MouseBackground(HWND hwnd)
    : m_hwnd(hwnd)
{
}


MouseBackground::~MouseBackground()
{
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
    ::ScreenToClient(m_hwnd, &m_cursor_pos);

    LPARAM lParam = MAKELPARAM(m_cursor_pos.x, m_cursor_pos.y);
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
