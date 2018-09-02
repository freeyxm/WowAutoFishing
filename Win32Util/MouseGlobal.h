#pragma once
#include "MouseBase.h"

class MouseGlobal : public MouseBase
{
public:
    MouseGlobal();
    virtual ~MouseGlobal();

    virtual bool Init() override;

    virtual bool GetCursorPos(POINT &point) override;
    virtual void SetCursorPos(const POINT &point) override;
    virtual void SetCursorPos(int x, int y) override;

    virtual void ClickLeftButton() override;
    virtual void ClickRightButton() override;
};

