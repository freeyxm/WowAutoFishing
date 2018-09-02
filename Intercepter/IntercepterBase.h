#pragma once

class IntercepterBase
{
public:
    IntercepterBase();
    virtual ~IntercepterBase();

    virtual bool Attach();
    virtual bool Detach();

protected:
    virtual bool AttachFunctions();
    virtual bool DetachFunctions();
};

