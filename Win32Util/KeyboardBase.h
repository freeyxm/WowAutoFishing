#pragma once
#include <Windows.h>
#include <string>

class KeyboardBase
{
public:
    KeyboardBase();
    virtual ~KeyboardBase() = 0;

    virtual void PressKey(int key);
    virtual void PressKey(int key, int modifiers);
    virtual void PressKey(int key, const std::string& modifiers);

protected:
    virtual void PressModifier(int modifiers, bool press);
    virtual void PressModifier(char modifier, bool press);
    virtual void KeyDown(int key) = 0;
    virtual void KeyUp(int key) = 0;
};

