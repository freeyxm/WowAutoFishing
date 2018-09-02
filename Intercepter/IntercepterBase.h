#pragma once
#include <fstream>
#include <string>

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

    virtual void OpenLog(const std::string& file);
    virtual void CloseLog();
    virtual void PrintLog(const std::string& msg);

protected:
    std::fstream m_log_file;
};

