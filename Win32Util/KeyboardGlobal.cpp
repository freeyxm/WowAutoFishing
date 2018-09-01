#include "stdafx.h"
#include "KeyboardGlobal.h"


KeyboardGlobal::KeyboardGlobal()
{
}


KeyboardGlobal::~KeyboardGlobal()
{
}


void KeyboardGlobal::KeyDown(int key)
{
    INPUT input[1];

    input[0].type = INPUT_KEYBOARD;
    input[0].ki.dwFlags = 0;
    input[0].ki.wVk = key;

    ::SendInput(sizeof(input) / sizeof(INPUT), input, sizeof(INPUT));
}


void KeyboardGlobal::KeyUp(int key)
{
    INPUT input[1];

    input[0].type = INPUT_KEYBOARD;
    input[0].ki.dwFlags = KEYEVENTF_KEYUP;
    input[0].ki.wVk = key;

    ::SendInput(sizeof(input) / sizeof(INPUT), input, sizeof(INPUT));
}