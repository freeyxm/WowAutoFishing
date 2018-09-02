#include "stdafx.h"
#include "IntercepterBase.h"
#include "detours/include/detours.h"
#include <ctime>
#include <iomanip>


IntercepterBase::IntercepterBase()
{
}


IntercepterBase::~IntercepterBase()
{
}


bool IntercepterBase::Attach()
{
    long ret = NO_ERROR;

    PrintLog("[Attach] Attach start");

    do
    {
        ret = DetourTransactionBegin();
        if (ret != NO_ERROR)
        {
            PrintLog(std::string("[Attach] DetourTransactionBegin failed, code = ").append(std::to_string(ret)));
            break;
        }
        ret = DetourUpdateThread(GetCurrentThread());
        if (ret != NO_ERROR)
        {
            PrintLog(std::string("[Attach] DetourUpdateThread failed, code = ").append(std::to_string(ret)));
            DetourTransactionAbort();
            break;
        }
        if (!AttachFunctions())
        {
            PrintLog(std::string("[Attach] AttachFunctions failed"));
            DetourTransactionAbort();
            break;
        }
        ret = DetourTransactionCommit();
        if (ret != NO_ERROR)
        {
            PrintLog(std::string("[Attach] DetourTransactionBegin failed, code = ").append(std::to_string(ret)));
            DetourTransactionAbort();
            break;
        }

        PrintLog("[Attach] Attach success");

    } while (false);

    return ret == NO_ERROR;
}


bool IntercepterBase::Detach()
{
    long ret = NO_ERROR;

    PrintLog("[Detach] Detach start");

    do
    {
        ret = DetourTransactionBegin();
        if (ret != NO_ERROR)
        {
            PrintLog(std::string("[Detach] DetourTransactionBegin failed, code = ").append(std::to_string(ret)));
            break;
        }
        ret = DetourUpdateThread(GetCurrentThread());
        if (ret != NO_ERROR)
        {
            PrintLog(std::string("[Detach] DetourUpdateThread failed, code = ").append(std::to_string(ret)));
            DetourTransactionAbort();
            break;
        }
        if (!DetachFunctions())
        {
            PrintLog("[Detach] DetachFunctions failed");
            DetourTransactionAbort();
            break;
        }
        ret = DetourTransactionCommit();
        if (ret != NO_ERROR)
        {
            PrintLog(std::string("[Detach] DetourTransactionCommit failed, code = ").append(std::to_string(ret)));
            DetourTransactionAbort();
            break;
        }

        PrintLog("[Detach] Detach success");

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


void IntercepterBase::OpenLog(const std::string& file)
{
    m_log_file.close();
    m_log_file.open(file, std::ios_base::app);
}

void IntercepterBase::CloseLog()
{
    m_log_file.close();
}

void IntercepterBase::PrintLog(const std::string& msg)
{
    if (m_log_file)
    {
        time_t t = ::time(NULL);
        struct tm ti;
        ::localtime_s(&ti, &t);

        m_log_file.fill('0');
        m_log_file << std::setw(2);
        m_log_file << ti.tm_year + 1900 << "-" << (ti.tm_mon + 1) << "-" << ti.tm_mday << " ";
        m_log_file << ti.tm_hour << ":" << ti.tm_min << ":" << ti.tm_sec << " ";
        m_log_file << msg.c_str() << std::endl;
    }
}
