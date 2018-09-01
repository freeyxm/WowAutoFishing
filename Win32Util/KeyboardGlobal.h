#pragma once
#include "KeyboardBase.h"

class KeyboardGlobal : public KeyboardBase
{
public:
    KeyboardGlobal();
    virtual ~KeyboardGlobal();

protected:
    virtual void KeyDown(int key) override;
    virtual void KeyUp(int key) override;
};

