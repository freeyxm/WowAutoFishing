#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "Fisher.h"
#include "WowFisherDlg.h"
#include "FisherStateMachine.h"
#include "FishingSoundListener.h"
#include "CommUtil/CommUtil.hpp"
#include "CommUtil/FTimer.h"
#include "Win32Util/Utility.h"
#include "Win32Util/MouseBackground.h"
#include "Win32Util/KeyboardBackground.h"
#include "Win32Util/ImageUtil.h"
#include <ctime>


static UINT __stdcall FishingTheadProc(LPVOID param);


Fisher::Fisher(HWND hwnd, int x, int y, int w, int h)
    : m_hWndWOW(hwnd)
    , m_hWndMain(0)
    , m_bInited(false)
    , m_bFishing(false)
    , m_bHasBite(false)
    , m_floatPoint()
    , m_hotkeyThrow(0)
    , m_hotkeyBite1(0)
    , m_hotkeyBite2(0)
    , m_hotkeyBite3(0)
    , m_hThreadFishing(NULL)
{
    m_throwCount = m_timeoutCount = m_findFloatFailCount = 0;
    m_baitTime = 0;
    m_state_machine = new FisherStateMachine(this);
    m_sound = new FishingSoundListener(this);
    m_keyboard = new KeyboardBackground(m_hWndWOW);
    m_mouse = new MouseBackground(m_hWndWOW);
}

Fisher::~Fisher()
{
    Stop();

    SAFE_DELETE(m_state_machine);
    SAFE_DELETE(m_sound);
    SAFE_DELETE(m_keyboard);
    SAFE_DELETE(m_mouse);
}

bool Fisher::Init()
{
    if (m_bInited)
        return true;

    if (!m_sound->Init())
    {
        return false;
    }

    m_state_machine->Init();

    m_bInited = true;
    return true;
}

void Fisher::SetWowHWnd(HWND hwnd)
{
    m_hWndWOW = hwnd;
}

void Fisher::SetMainHWnd(HWND hwnd)
{
    m_hWndMain = hwnd;
}

void Fisher::PressKeyboard(int key)
{
    m_keyboard->PressKey(key & 0xff, (key >> 8) & 0xff);
}

void Fisher::PressKey(int key)
{
    m_keyboard->PressKey(key);
}

bool Fisher::Start()
{
    if (m_mouse->GetHwnd() != m_hWndWOW)
    {
        m_mouse->SetHwnd(m_hWndWOW);
        if (!m_mouse->Init())
        {
            wprintf(L"初始化鼠标失败！\n");
            return false;
        }
    }

    if (m_keyboard->GetHwnd() != m_hWndWOW)
    {
        m_keyboard->SetHwnd(m_hWndWOW);
        if (!m_keyboard->Init())
        {
            wprintf(L"初始化键盘失败！\n");
            return false;
        }
    }

    if (Utility::GetWndExePath(m_hWndWOW, m_wowPath))
    {
        size_t pos = m_wowPath.find_last_of('\\');
        if (pos != std::string::npos)
            m_screenshotPath = m_wowPath.substr(0, pos) + L"\\Screenshots";
        else
            m_screenshotPath = m_wowPath;
    }
    else
    {
        wprintf(L"查找安装路径失败！\n");
        return false;
    }

    m_hThreadFishing = (HANDLE)::_beginthreadex(NULL, 0, &FishingTheadProc, this, 0, NULL);
    if (m_hThreadFishing == NULL)
    {
        wprintf(L"创建线程失败！\n");
        return false;
    }

    m_bFishing = true;
    return true;
}

void Fisher::Stop()
{
    m_bFishing = false;

    if (m_hThreadFishing != NULL)
    {
        CloseHandle(m_hThreadFishing);
        m_hThreadFishing = NULL;
    }
}

static UINT __stdcall FishingTheadProc(LPVOID param)
{
    Fisher* pFisher = (Fisher*)param;
    if (pFisher)
    {
        pFisher->StartFishing();
    }
    return 0;
}

FisherStateMachine& Fisher::GetStateMachine()
{
    return *m_state_machine;
}

void Fisher::StartFishing()
{
    m_bFishing = true;
    m_baitTime = 0;

    wprintf(L"---------------------------------------\n");

    m_state_machine->GotoState(FisherStateType::FisherState_Start);

    comm_util::FTimer timer;
    timer.Start();

    int64_t last_time = timer.Milliseconds();

    while (m_bFishing)
    {
        int64_t cur_time = timer.Milliseconds();
        int dt = (int)(cur_time - last_time);

        m_state_machine->Update(dt);

        last_time = cur_time;
        ::Sleep((DWORD)10);
    }

    timer.Stop();
}

void Fisher::SetBaitTime(time_t time)
{
    m_baitTime = time;
}

time_t Fisher::GetBaitTime() const
{
    return m_baitTime;
}

// 甩竿
bool Fisher::ThrowPole()
{
    m_throwCount++;
    PressKeyboard(m_hotkeyThrow);
    ::SendMessage(m_hWndMain, WMU_UPDATE_STATISTICS, 0, 0);
    return true;
}

static bool MatchFloatColor(char _r, char _g, char _b)
{
    unsigned r = (unsigned char)_r;
    unsigned g = (unsigned char)_g;
    unsigned b = (unsigned char)_b;
    if (r > g && r > b)
    {
        if (g == 0)
            g = 1;
        if (b == 0)
            b = 1;
        float r_g = (float)r / g;
        float r_b = (float)r / b;
        if (r_g > 2.0f && r_b > 2.0f)
        {
            return true;
        }
    }
    return false;
}

// 寻找鱼漂
bool Fisher::FindFloat()
{
    std::wstring path;
    if (!Utility::FindLatestFile(m_screenshotPath, path))
    {
        wprintf(L"未找到截图: %s\n", m_screenshotPath.c_str());
        return false;
    }

    CImage image;
    HRESULT res = image.Load(path.c_str());
    if (res != S_OK)
    {
        wprintf(L"图片读取失败: %s\n", path.c_str());
        return false;
    }

    ::DeleteFileW(path.c_str());

    m_points.clear();
    ImageUtil::FindColor(image, MatchFloatColor, m_points);
    image.Destroy();

    {
        if (m_points.size() >= 2000) // 找多太多点，可能是UI开着或水域不合适，无法定位鱼漂
            m_points.clear();

        if (m_points.size() == 0)
            wprintf(L"寻找鱼漂失败。\n");

        POINT p;
        if (ImageUtil::SelectBestPoint(m_points, 30, p)) // 根据鱼漂的大概半径选择最优的点
        {
            wprintf(L"找到鱼漂: %d, %d\n", p.x, p.y);

            m_floatPoint = p;

            ::ClientToScreen(m_hWndWOW, &p);
            m_mouse->SetCursorPos(p.x, p.y);

            return true;
        }
    }

    ++m_findFloatFailCount;
    ::SendMessage(m_hWndMain, WMU_UPDATE_STATISTICS, 0, 0);

    return false;
}

// 等待上钩
bool Fisher::StartListenBite()
{
    m_bHasBite = false;
    return m_sound->Start();
}

void Fisher::StopListenBite()
{
    m_sound->Stop();

    if (!m_bHasBite)
    {
        ++m_timeoutCount;
        ::SendMessage(m_hWndMain, WMU_UPDATE_STATISTICS, 0, 0);
    }
}

bool Fisher::HasBite() const
{
    return m_bHasBite;
}

// 提竿
bool Fisher::Shaduf()
{
    bool res = true;

    POINT p = m_floatPoint;
    ::ClientToScreen(m_hWndWOW, &p);

    m_mouse->SetCursorPos(p.x, p.y); // 重新设定鼠标，防止中间移动而在错误的位置。
    m_mouse->ClickRightButton();

    return res;
}

void Fisher::TurnEnd()
{
    wprintf(L"throw: %d, timeout: %d, float: %d\n", m_throwCount, m_timeoutCount, m_findFloatFailCount);
    wprintf(L"---------------------------------------\n");
}

void Fisher::NotifyBite()
{
    PrintStatus(L"咬钩了！\n");
    m_bHasBite = true;
}

void Fisher::PrintStatus(LPCWSTR msg)
{
    wprintf(L"%ls", msg);
    ::SendMessage(m_hWndMain, WMU_UPDATE_STATUS, (WPARAM)msg, 0);
}

void Fisher::SetAmpL(float ampL)
{
    m_sound->SetAmpL(ampL);
}

void Fisher::SetAmpH(float ampH)
{
    m_sound->SetAmpH(ampH);
}

void Fisher::SetSilentMax(int count)
{
    m_sound->SetSilentMaxCount(count);
}

void Fisher::SetSoundMin(int count)
{
    m_sound->SetSoundMinCount(count);
}

void Fisher::SetHotkeyThrow(DWORD hotkey)
{
    m_hotkeyThrow = hotkey;
}

void Fisher::SetHotkeyBite1(DWORD hotkey)
{
    m_hotkeyBite1 = hotkey;
}
void Fisher::SetHotkeyBite2(DWORD hotkey)
{
    m_hotkeyBite2 = hotkey;
}

void Fisher::SetHotkeyBite3(DWORD hotkey)
{
    m_hotkeyBite3 = hotkey;
}

int Fisher::GetThrowCount()
{
    return m_throwCount;
}

int Fisher::GetFindFloatFailCount()
{
    return m_findFloatFailCount;
}

int Fisher::GetTimeoutCount()
{
    return m_timeoutCount;
}

void Fisher::SetThrowCount(int count)
{
    m_throwCount = count;
}

void Fisher::SetFindFloatFailCount(int count)
{
    m_findFloatFailCount = count;
}

void Fisher::SetTimeoutCount(int count)
{
    m_timeoutCount = count;
}
