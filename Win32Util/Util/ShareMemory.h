#pragma once
#include <Windows.h>
#include <string>

class ShareMemory
{
public:
    ShareMemory(const std::string& name);
    ~ShareMemory();

    bool Create(size_t size);
    bool Open(size_t size, DWORD access);
    void Close();

    bool Lock(DWORD dwMilliseconds = INFINITE);
    void Unlock();

    const std::string& GetName() const;
    size_t GetSize() const;
    LPVOID GetBuf();

private:
    bool Map(DWORD access);
    void UnMap();

private:
    std::string m_name;
    size_t m_size;
    HANDLE m_hMutex;
    HANDLE m_hFileMap;
    LPVOID m_pMemory;
};

