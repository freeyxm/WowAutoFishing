#pragma once
#include <Windows.h>

class MouseBase
{
public:
    MouseBase();
    virtual ~MouseBase() = 0;

    virtual bool Init() = 0;

    virtual bool GetCursorPos(POINT &point) = 0;
    virtual void SetCursorPos(const POINT &point) = 0;
    virtual void SetCursorPos(int x, int y) = 0;

    virtual void ClickLeftButton() = 0;
    virtual void ClickRightButton() = 0;
};

