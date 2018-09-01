#include "stdafx.h"
#include "KeyboardBase.h"
#include <CommCtrl.h>


KeyboardBase::KeyboardBase()
{
}


KeyboardBase::~KeyboardBase()
{
}


void KeyboardBase::PressKey(int key)
{
    KeyDown(key);
    KeyUp(key);
}


void KeyboardBase::PressKey(int key, int modifiers)
{
    PressModifier(modifiers, true);
    KeyDown(key);
    KeyUp(key);
    PressModifier(modifiers, false);
}


void KeyboardBase::PressModifier(int modifiers, bool press)
{
    if (press)
    {
        if (modifiers & HOTKEYF_CONTROL)
            KeyDown(VK_CONTROL); // Ctrl
        if (modifiers & HOTKEYF_ALT)
            KeyDown(VK_MENU); // Alt
        if (modifiers & HOTKEYF_SHIFT)
            KeyDown(VK_SHIFT); // Shift
    }
    else
    {
        if (modifiers & HOTKEYF_SHIFT)
            KeyUp(VK_SHIFT); // Shift
        if (modifiers & HOTKEYF_ALT)
            KeyUp(VK_MENU); // Alt
        if (modifiers & HOTKEYF_CONTROL)
            KeyUp(VK_CONTROL); // Ctrl
    }
}
