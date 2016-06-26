#pragma once
#pragma execution_character_set("utf-8")
#include "Win32Util/MouseUtil.h"
#include "Win32Util/KeyboardUtil.h"
#include "Win32Util/ImageUtil.h"
#include "FishingSoundListener.h"

class Fisher
{
	friend static UINT __stdcall FishingTheadProc(LPVOID param);
public:
	Fisher(HWND hwnd, int x, int y, int w, int h);
	~Fisher();

	bool Init();
	void SetWowHWnd(HWND hwnd);
	void SetMainHWnd(HWND hwnd);

	bool Start();
	void Stop();

	void NotifyBite(); // used by FishingSoundListener.
	bool CheckTimeout(); // used by FishingSoundListener.

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

	void SetDebugBite(bool bDebug);

private:
	void ActiveWindow();

	void StartFishing();
	bool CheckBaitTime();
	bool Bait();
	bool ThrowPole();
	bool FindFloat();
	bool WaitBiteStart();
	void WaitBite();
	void WaitBiteEnd();
	bool Shaduf();
	void WaitFloatHide();
	void StateEnd();

	void PrintStatus(LPCWSTR msg);

private:
	enum class FishingState
	{
		State_None,
		State_Start,
		State_CheckState, // 检查状态
		State_Bait, // 上鱼饵
		State_ThrowPole, // 甩竿
		State_FindFloat, // 定位鱼漂
		State_WaitBiteStart,
		State_WaitBite, // 等待咬钩
		State_WaitBiteEnd,
		State_Shaduf, // 提竿
		State_WaitFloatHide,
		State_End,
	};

private:
	HWND m_hWndWOW; // 魔兽世界窗口句柄
	HWND m_hWndMain;
	int m_posX, m_posY;
	int m_width, m_height;
	FishingState m_state;
	time_t m_waitTime; // 等待时间
	time_t m_baitTime; // 上饵时间
	time_t m_throwTime; // 甩竿时间
	bool m_bHasBite; // 是否已咬钩
	bool m_bTimeout;
	bool m_bDebugBite;
	POINT m_floatPoint;

	DWORD m_hotkeyThrow;
	DWORD m_hotkeyBite1;
	DWORD m_hotkeyBite2;
	DWORD m_hotkeyBite3;

	int m_throwCount;
	int m_findFloatFailCount;
	int m_timeoutCount;

	char *m_lpBits; // 窗口位图像素数据
	std::list<POINT> m_points;
	
	MouseUtil m_mouse;
	KeyboardUtil m_keyboard;
	FishingSoundListener m_sound;
	
	HANDLE m_hThreadFishing;

	bool m_bInited;
	BOOL m_bFishing;
};

