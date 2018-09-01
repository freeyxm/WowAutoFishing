#include "stdafx.h"
#include "MouseGlobal.h"


MouseGlobal::MouseGlobal()
{
}


MouseGlobal::~MouseGlobal()
{
}


bool MouseGlobal::GetCursorPos(POINT &point)
{
    return ::GetCursorPos(&point);
}


void MouseGlobal::SetCursorPos(const POINT &point)
{
    this->SetCursorPos(point.x, point.y);
}


void MouseGlobal::SetCursorPos(int x, int y)
{
    ::SetCursorPos(x, y);
}


void MouseGlobal::ClickLeftButton()
{
    INPUT input[2];

    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    ::SendInput(sizeof(input) / sizeof(INPUT), input, sizeof(INPUT));
}


void MouseGlobal::ClickRightButton()
{
    INPUT input[2];

    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;

    ::SendInput(sizeof(input) / sizeof(INPUT), input, sizeof(INPUT));
}
