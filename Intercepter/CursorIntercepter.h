#pragma once
#include "IntercepterBase.h"
#include <Windows.h>
#include <Win32Util/ShareMemory.h>
#include <string>

class CursorIntercepter : public IntercepterBase
{
public:
    struct ShareData
    {
        POINT cursor_pos;
    };

public:
    CursorIntercepter(const std::string& name);
    virtual ~CursorIntercepter();

    virtual bool Attach() override;
    virtual bool Detach() override;

protected:
    virtual bool AttachFunctions() override;
    virtual bool DetachFunctions() override;

    static bool MyGetCursorPos(LPPOINT lpPoint);
    static bool MySetCursorPos(int x, int y);

private:
    ShareMemory m_share_memory;
    static CursorIntercepter* m_instance;
};

