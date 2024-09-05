#include "stdafx.h"
#include "FisherConfig.h"
#include <locale.h>


FisherConfig::FisherConfig(LPCWSTR path)
    : m_filePath(path)
    , m_nAmpMax(0)
    , m_nAmpL(0)
    , m_nAmpH(0)
    , m_nSilentMaxCount(0)
    , m_nSoundMinCount(0)
    , m_nThrowCount(0)
    , m_nTimeoutCount(0)
    , m_nFloatCount(0)
    , m_bShowConsole(0)
    , m_hotkeyThrow(0)
    , m_hotkeyBite1(0)
    , m_hotkeyBite2(0)
    , m_hotkeyBite3(0)
    , m_nMaxDtw(10)
    , m_nFishingTime(0)
{
}

FisherConfig::~FisherConfig()
{
}

int FisherConfig::GetPrivateProfileInt(LPCWSTR key, int defaultValue)
{
    int value = ::GetPrivateProfileInt(L"Fisher", key, defaultValue, m_filePath.c_str());
    return value;
}

void FisherConfig::WritePrivateProfileInt(LPCWSTR key, int value)
{
    WCHAR buf[20];
    wsprintf(buf, L"%d", value);
    ::WritePrivateProfileString(L"Fisher", key, buf, m_filePath.c_str());
}

bool FisherConfig::LoadConfig()
{
    m_nAmpMax = GetPrivateProfileInt(L"AmpMax", 1000);
    m_nAmpH = GetPrivateProfileInt(L"AmpH", 200);
    m_nAmpL = GetPrivateProfileInt(L"AmpL", 5);

    m_hotkeyThrow = GetPrivateProfileInt(L"HotkeyThrow", 0x31);
    m_hotkeyBite1 = GetPrivateProfileInt(L"HotkeyBite1", 0x33);
    m_hotkeyBite2 = GetPrivateProfileInt(L"HotkeyBite2", 0x34);
    m_hotkeyBite3 = GetPrivateProfileInt(L"HotkeyBite3", 0x35);

    m_nSilentMaxCount = GetPrivateProfileInt(L"SilentMaxCount", 10);
    m_nSoundMinCount = GetPrivateProfileInt(L"SoundMinCount", 20);

    m_bShowConsole = GetPrivateProfileInt(L"ShowConsole", 0);

    m_nThrowCount = GetPrivateProfileInt(L"ThrowCount", 0);
    m_nTimeoutCount = GetPrivateProfileInt(L"TimeoutCount", 0);
    m_nFloatCount = GetPrivateProfileInt(L"FloatCount", 0);

    m_nMaxDtw = GetPrivateProfileInt(L"MaxDtw", 0);
    m_nFishingTime = GetPrivateProfileInt(L"FishingTime", 0);

    return true;
}

void FisherConfig::SaveConfig()
{
    WritePrivateProfileInt(L"AmpMax", m_nAmpMax);
    WritePrivateProfileInt(L"AmpH", m_nAmpH);
    WritePrivateProfileInt(L"AmpL", m_nAmpL);

    WritePrivateProfileInt(L"HotkeyThrow", m_hotkeyThrow);
    WritePrivateProfileInt(L"HotkeyBite1", m_hotkeyBite1);
    WritePrivateProfileInt(L"HotkeyBite2", m_hotkeyBite2);
    WritePrivateProfileInt(L"HotkeyBite3", m_hotkeyBite3);

    WritePrivateProfileInt(L"SilentMaxCount", m_nSilentMaxCount);
    WritePrivateProfileInt(L"SoundMinCount", m_nSoundMinCount);

    WritePrivateProfileInt(L"ShowConsole", m_bShowConsole);

    WritePrivateProfileInt(L"ThrowCount", m_nThrowCount);
    WritePrivateProfileInt(L"TimeoutCount", m_nTimeoutCount);
    WritePrivateProfileInt(L"FloatCount", m_nFloatCount);

    WritePrivateProfileInt(L"MaxDtw", m_nMaxDtw);
    WritePrivateProfileInt(L"FishingTime", m_nFishingTime);
}
