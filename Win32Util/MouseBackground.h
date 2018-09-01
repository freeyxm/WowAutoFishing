#pragma once
#include "MouseBase.h"

class MouseBackground : public MouseBase
{
public:
    MouseBackground(HWND hwnd);
    virtual ~MouseBackground();

    virtual bool GetCursorPos(POINT &point) override;
    virtual void SetCursorPos(const POINT &point) override;
    virtual void SetCursorPos(int x, int y) override;

    virtual void ClickLeftButton() override;
    virtual void ClickRightButton() override;

private:
    HWND m_hwnd;
    POINT m_cursor_pos;
};

