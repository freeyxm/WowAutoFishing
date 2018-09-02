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


void KeyboardBase::PressKey(int key, const std::string& modifiers)
{
    for (size_t i = 0; i < modifiers.size(); ++i)
    {
        PressModifier(modifiers[i], true);
    }

    KeyDown(key);
    KeyUp(key);

    for (size_t i = modifiers.size(); i > 0; --i)
    {
        PressModifier(modifiers[i - 1], false);
    }
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

void KeyboardBase::PressModifier(char modifier, bool press)
{
    int key = 0;
    switch (modifier)
    {
    case 'C':
        key = VK_CONTROL;
        break;
    case 'A':
        key = VK_MENU;
        break;
    case 'S':
        key = VK_SHIFT;
        break;
    default:
        break;
    }

    if (key != 0)
    {
        if (press)
            KeyDown(key);
        else
            KeyUp(key);
    }
}
