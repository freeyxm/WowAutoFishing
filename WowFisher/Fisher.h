#pragma once
#pragma execution_character_set("utf-8")
#include "FisherStateDefine.h"
#include <Windows.h>
#include <list>
#include <string>

const int MAX_BAIT_TIME = 10 * 60; // 鱼饵持续时间，单位秒
const int MAX_BITE_TIME = 23; // 最长等待咬钩时间，单位秒

class MouseBackground;
class KeyboardBackground;
class FishingSoundListener;
class FisherStateMachine;

class Fisher
{
    friend static UINT __stdcall FishingTheadProc(LPVOID param);
    friend class FisherStateCheckBait;
    friend class FisherStateBait;
    friend class FisherStateThrowPole;
    friend class FisherStateFindFloat;
    friend class FisherStateWaitBite;
    friend class FisherStateShaduf;
    friend class FisherStateWaitFloatHide;
public:
    Fisher(HWND hwnd, int x, int y, int w, int h);
    ~Fisher();

    bool Init();
    void SetWowHWnd(HWND hwnd);
    void SetMainHWnd(HWND hwnd);

    bool Start();
    void Stop();

    void NotifyBite(); // used by FishingSoundListener.

    void SetAmpL(float ampL);
    void SetAmpH(float ampH);
    void SetSilentMax(int count);
    void SetSoundMin(int count);

    void SetHotkeyThrow(DWORD hotkey);
    void SetHotkeyBite1(DWORD hotkey);
    void SetHotkeyBite2(DWORD hotkey);
    void SetHotkeyBite3(DWORD hotkey);

    int GetThrowCount();
    int GetFindFloatFailCount();
    int GetTimeoutCount();
    void SetThrowCount(int count);
    void SetFindFloatFailCount(int count);
    void SetTimeoutCount(int count);

private:
    void PressKeyboard(int key);
    void PressKey(int key);

    FisherStateMachine& GetStateMachine();

    void SetBaitTime(time_t time);
    time_t GetBaitTime() const;

    void StartFishing();
    bool ThrowPole();
    bool FindFloat();
    bool StartListenBite();
    void StopListenBite();
    bool HasBite() const;
    bool Shaduf();
    void TurnEnd();

    void PrintStatus(LPCWSTR msg);

private:
    HWND m_hWndWOW; // 魔兽世界窗口句柄
    HWND m_hWndMain;

    time_t m_baitTime; // 上饵时间
    bool m_bHasBite; // 是否已咬钩
    POINT m_floatPoint;

    DWORD m_hotkeyThrow;
    DWORD m_hotkeyBite1;
    DWORD m_hotkeyBite2;
    DWORD m_hotkeyBite3;

    int m_throwCount;
    int m_findFloatFailCount;
    int m_timeoutCount;

    std::list<POINT> m_points;

    MouseBackground* m_mouse;
    KeyboardBackground* m_keyboard;
    FishingSoundListener* m_sound;
    FisherStateMachine* m_state_machine;

    HANDLE m_hThreadFishing;

    bool m_bInited;
    BOOL m_bFishing;

    std::wstring m_wowPath;
    std::wstring m_screenshotPath;
};

