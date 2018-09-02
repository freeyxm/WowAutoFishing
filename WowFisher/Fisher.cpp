#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "Fisher.h"
#include "WowFisherDlg.h"
#include "CommUtil/FTimer.h"
#include "FisherStateMachine.h"
#include <ctime>


static UINT __stdcall FishingTheadProc(LPVOID param);


Fisher::Fisher(HWND hwnd, int x, int y, int w, int h)
    : m_hWndWOW(hwnd), m_keyboard(hwnd), m_mouse(hwnd), m_hWndMain(0)
    , m_posX(x), m_posY(y), m_width(w), m_height(h), m_sound(this)
    , m_bInited(false), m_hThreadFishing(NULL)
{
    m_throwCount = m_timeoutCount = m_findFloatFailCount = 0;
    m_baitTime = 0;
    m_state_machine = new FisherStateMachine(this);
}

Fisher::~Fisher()
{
    Stop();

    if (m_state_machine != NULL)
    {
        delete m_state_machine;
        m_state_machine = NULL;
    }

    if (m_lpBits != NULL)
    {
        ::free(m_lpBits);
        m_lpBits = NULL;
    }
}

bool Fisher::Init()
{
    if (m_bInited)
        return true;

    m_lpBits = (char*)malloc(m_width * m_height * 4);
    if (!m_lpBits)
    {
        return false;
    }

    m_sound.SetNotifyBiteProc(&Fisher::NotifyBite);
    if (!m_sound.Init())
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

void Fisher::ActiveWindow()
{
    m_hWndLast = ::GetForegroundWindow();
    if (m_hWndLast != m_hWndWOW)
    {
        ::SetForegroundWindow(m_hWndWOW);
        ::SetActiveWindow(m_hWndWOW);
    }
}

void Fisher::InActiveWindow()
{
    if (m_hWndLast != m_hWndWOW)
    {
        ::SetForegroundWindow(m_hWndLast);
        ::SetActiveWindow(m_hWndLast);
    }
}

void Fisher::PressKeyboard(int key)
{
    ActiveWindow();
    m_keyboard.PressKey(key & 0xff, (key >> 8) & 0xff);
    InActiveWindow();
}

bool Fisher::Start()
{
    m_mouse.SetHwnd(m_hWndWOW);
    if (!m_mouse.Init())
    {
        return false;
    }

    m_keyboard.SetHwnd(m_hWndWOW);

    m_hThreadFishing = (HANDLE)::_beginthreadex(NULL, 0, &FishingTheadProc, this, 0, NULL);
    if (m_hThreadFishing == NULL)
        return false;

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
    ActiveWindow();

    BITMAPINFOHEADER bi;
    if (ImageUtil::GetWindowSnapshot(m_hWndWOW, m_posX, m_posY, m_width, m_height, m_lpBits, &bi))
    {
        m_points.clear();
        unsigned int maxCount = 10000;
        //ImageUtil::FindGray((char*)m_lpBits, m_width, m_height, 20, 3, m_points, maxCount); // 用灰度图寻找鱼漂
        //ImageUtil::FindColor((char*)m_lpBits, m_width, m_height, RGB(180, 55, 40), RGB(20, 15, 10), m_points); // 砮皂寺
        //ImageUtil::FindColor((char*)m_lpBits, m_width, m_height, RGB(60, 10, 10), RGB(10, 10, 10), m_points); // AG
        ImageUtil::FindColor((char*)m_lpBits, m_width, m_height, MatchFloatColor, m_points);

        if (m_points.size() >= maxCount) // 找多太多点，可能是UI开着或水域不合适，无法定位鱼漂
            m_points.clear();
        else if (m_points.size() == 0)
            wprintf(L"寻找鱼漂失败。\n");

        POINT p;
        if (ImageUtil::SelectBestPoint(m_points, 30, p)) // 根据鱼漂的大概半径选择最优的点
        {
            p.x += m_posX;
            p.y = m_height - p.y + m_posY;

            wprintf(L"找到鱼漂: %d, %d\n", p.x, p.y);

            m_floatPoint = p;

            ::ClientToScreen(m_hWndWOW, &p);
            ::SetCursorPos(p.x, p.y); // todo for debug!!!

            m_mouse.SetCursorPos(p.x, p.y);

            //InActiveWindow();
            return true;
        }
    }

    ++m_findFloatFailCount;
    ::SendMessage(m_hWndMain, WMU_UPDATE_STATISTICS, 0, 0);

    //InActiveWindow();
    return false;
}

// 等待上钩
bool Fisher::StartListenBite()
{
    m_bHasBite = false;
    return m_sound.Start();
}

void Fisher::StopListenBite()
{
    m_sound.Stop();

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
    ActiveWindow();

    do
    {
        POINT p = m_floatPoint;
        ::ClientToScreen(m_hWndWOW, &p);
        ::SetCursorPos(p.x, p.y); // todo for debug!!!

        m_mouse.SetCursorPos(p.x, p.y); // 重新设定鼠标，防止中间移动而在错误的位置。
        Sleep(10);
        m_mouse.ClickRightButton();
    } while (false);

    //InActiveWindow();
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
    m_sound.SetAmpL(ampL);
}

void Fisher::SetAmpH(float ampH)
{
    m_sound.SetAmpH(ampH);
}

void Fisher::SetSilentMax(int count)
{
    m_sound.SetSilentMaxCount(count);
}

void Fisher::SetSoundMin(int count)
{
    m_sound.SetSoundMinCount(count);
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