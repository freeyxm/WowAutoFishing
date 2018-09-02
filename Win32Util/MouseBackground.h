#pragma once
#include "MouseBase.h"
#include <string>

class ShareMemory;

class MouseBackground : public MouseBase
{
public:
    MouseBackground(HWND hwnd);
    virtual ~MouseBackground();

    HWND GetHwnd() const { return m_hwnd; }
    void SetHwnd(HWND hwnd) { m_hwnd = hwnd; }

    virtual bool Init() override;
    virtual void Close() override;

    virtual bool GetCursorPos(POINT &point) override;
    virtual void SetCursorPos(const POINT &point) override;
    virtual void SetCursorPos(int x, int y) override;

    virtual void ClickLeftButton() override;
    virtual void ClickRightButton() override;

private:
    HWND m_hwnd;
    DWORD m_pid;
    POINT m_cursor_pos;
    ShareMemory* m_share_memory;
    std::wstring m_dll_path;
};

