#pragma once
#include <Windows.h>

class KeyboardBase
{
public:
    KeyboardBase();
    virtual ~KeyboardBase() = 0;

    virtual void PressKey(int key);
    virtual void PressKey(int key, int modifiers);

protected:
    virtual void PressModifier(int modifiers, bool press);
    virtual void KeyDown(int key) = 0;
    virtual void KeyUp(int key) = 0;
};

