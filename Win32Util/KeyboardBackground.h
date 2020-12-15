#pragma once
#include "KeyboardBase.h"

class KeyboardBackground : public KeyboardBase
{
public:
    KeyboardBackground(HWND hwnd);
    virtual ~KeyboardBackground();

    virtual HWND GetHwnd() const override { return m_hwnd; }
    virtual void SetHwnd(HWND hwnd) override { m_hwnd = hwnd; }

protected:
    virtual void KeyDown(int key) override;
    virtual void KeyUp(int key) override;

private:
    HWND m_hwnd;
};

