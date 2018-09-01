#include "stdafx.h"
#include "KeyboardBackground.h"


KeyboardBackground::KeyboardBackground(HWND hwnd)
    : m_hwnd(hwnd)
{
}


KeyboardBackground::~KeyboardBackground()
{
}


void KeyboardBackground::KeyDown(int key)
{
    ::PostMessage(m_hwnd, WM_KEYDOWN, key, 0);
}


void KeyboardBackground::KeyUp(int key)
{
    ::PostMessage(m_hwnd, WM_KEYUP, key, 0);
}

