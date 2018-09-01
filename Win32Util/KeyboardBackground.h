#pragma once
#include "KeyboardBase.h"

class KeyboardBackground : public KeyboardBase
{
public:
    KeyboardBackground(HWND hwnd);
    virtual ~KeyboardBackground();

protected:
    virtual void KeyDown(int key) override;
    virtual void KeyUp(int key) override;

private:
    HWND m_hwnd;
};

