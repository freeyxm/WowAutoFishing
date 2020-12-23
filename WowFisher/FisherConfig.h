#pragma once
#include <string>

class FisherConfig
{
public:
    FisherConfig(LPCWSTR path);
    ~FisherConfig();

    bool LoadConfig();
    void SaveConfig();

private:
    int GetPrivateProfileInt(LPCWSTR key, int defaultValue);
    void WritePrivateProfileInt(LPCWSTR key, int value);

public:
    int m_nAmpMax;
    int m_nAmpL;
    int m_nAmpH;
    int m_nSilentMaxCount;
    int m_nSoundMinCount;

    int m_nThrowCount;
    int m_nTimeoutCount;
    int m_nFloatCount;

    int m_bShowConsole;

    DWORD m_hotkeyThrow;
    DWORD m_hotkeyBite1;
    DWORD m_hotkeyBite2;
    DWORD m_hotkeyBite3;

private:
    std::wstring m_filePath;
};

