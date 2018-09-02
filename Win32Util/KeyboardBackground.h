#pragma once
#include "KeyboardBase.h"

class KeyboardBackground : public KeyboardBase
{
public:
    KeyboardBackground(HWND hwnd);
    virtual ~KeyboardBackground();

    HWND GetHwnd() const { return m_hwnd; }
    void SetHwnd(HWND hwnd) { m_hwnd = hwnd; }

protected:
    virtual void KeyDown(int key) override;
    virtual void KeyUp(int key) override;

private:
    HWND m_hwnd;
};

