#include "stdafx.h"
#include "ShareMemory.h"
#include <mutex>


ShareMemory::ShareMemory(const std::string& name)
    : m_name(name)
    , m_size(0)
    , m_hMutex(NULL)
    , m_hFileMap(NULL)
    , m_pMemory(NULL)
{
}

ShareMemory::~ShareMemory()
{
    Close();
    Unlock();
}

bool ShareMemory::Create(size_t size)
{
    Close();

    if (size <= 0)
    {
        return false;
    }

    m_size = size;

    m_hFileMap = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, m_name.c_str());
    if (m_hFileMap == NULL)
    {
        return false;
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        Close();
        return false;
    }

    return Map(FILE_MAP_ALL_ACCESS);
}

bool ShareMemory::Open(size_t size, DWORD access)
{
    Close();

    m_size = size;

    m_hFileMap = OpenFileMappingA(access, false, m_name.c_str());
    if (!m_hFileMap)
    {
        return false;
    }

    return Map(access);
}

void ShareMemory::Close()
{
    UnMap();

    if (m_hFileMap)
    {
        CloseHandle(m_hFileMap);
        m_hFileMap = NULL;
    }
}

bool ShareMemory::Map(DWORD access)
{
    if (!m_hFileMap)
    {
        return false;
    }

    m_pMemory = MapViewOfFile(m_hFileMap, access, 0, 0, m_size);
    if (!m_pMemory)
    {
        Close();
        return false;
    }

    return true;
}

void ShareMemory::UnMap()
{
    if (m_pMemory)
    {
        UnmapViewOfFile(m_pMemory);
        m_pMemory = NULL;
    }
}

size_t ShareMemory::GetSize() const
{
    return m_size;
}

LPVOID ShareMemory::GetBuf()
{
    return m_pMemory;
}

bool ShareMemory::Lock(DWORD dwMilliseconds)
{
    if (!m_hMutex)
    {
        std::string mutex_name = m_name + "_Lock";
        m_hMutex = CreateMutexA(NULL, false, mutex_name.c_str());
        if (!m_hMutex)
        {
            return false;
        }
    }

    DWORD ret = WaitForSingleObject(m_hMutex, dwMilliseconds);
    return (ret == WAIT_OBJECT_0 || ret == WAIT_ABANDONED);
}

void ShareMemory::Unlock()
{
    if (m_hMutex)
    {
        ReleaseMutex(m_hMutex);
    }
}