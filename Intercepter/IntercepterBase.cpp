#include "stdafx.h"
#include "IntercepterBase.h"
#include "detours/include/detours.h"


IntercepterBase::IntercepterBase()
{
}


IntercepterBase::~IntercepterBase()
{
}


bool IntercepterBase::Attach()
{
    long ret = NO_ERROR;

    do
    {
        ret = DetourTransactionBegin();
        if (ret != NO_ERROR)
        {
            break;
        }
        ret = DetourUpdateThread(GetCurrentThread());
        if (ret != NO_ERROR)
        {
            DetourTransactionAbort();
            break;
        }
        if (!AttachFunctions())
        {
            DetourTransactionAbort();
            break;
        }
        ret = DetourTransactionCommit();
        if (ret != NO_ERROR)
        {
            DetourTransactionAbort();
            break;
        }
    } while (false);

    return ret == NO_ERROR;
}


bool IntercepterBase::Detach()
{
    long ret = NO_ERROR;

    do
    {
        ret = DetourTransactionBegin();
        if (ret != NO_ERROR)
        {
            break;
        }
        ret = DetourUpdateThread(GetCurrentThread());
        if (ret != NO_ERROR)
        {
            DetourTransactionAbort();
            break;
        }
        if (!DetachFunctions())
        {
            DetourTransactionAbort();
            break;
        }
        ret = DetourTransactionCommit();
        if (ret != NO_ERROR)
        {
            DetourTransactionAbort();
            break;
        }
    } while (false);

    return ret == NO_ERROR;
}

bool IntercepterBase::AttachFunctions()
{
    // DetourAttach(...);
    return true;
}


bool IntercepterBase::DetachFunctions()
{
    // DetourDetach(...);
    return true;
}

