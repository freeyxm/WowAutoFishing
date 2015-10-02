#pragma once
#pragma execution_character_set("utf-8")
#include "Win32Util/MouseUtil.h"
#include "Win32Util/KeyboardUtil.h"
#include "Win32Util/ImageUtil.h"
#include "FishingSoundListener.h"

class Fisher
{
public:
	Fisher(HWND hwnd, int x, int y, int w, int h);
	~Fisher();

	bool Init();

	void StartFishing();

	void NotifyBite(); // used by FishingSoundListener.
	bool CheckTimeout(); // used by FishingSoundListener.

private:
	void ActiveWindow();

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
	HWND m_hwnd; // 魔兽世界窗口句柄
	int m_posX, m_posY;
	int m_width, m_height;
	FishingState m_state;
	time_t m_waitTime; // 等待时间
	time_t m_baitTime; // 上饵时间
	time_t m_throwTime; // 甩竿时间
	bool m_bHasBite; // 是否已咬钩
	bool m_bTimeout;
	POINT m_floatPoint;

	int m_throwCount;
	int m_findFloatFailCount;
	int m_timeoutCount;

	char *m_lpBits; // 窗口位图像素数据
	std::list<POINT> m_points;
	
	MouseUtil m_mouse;
	KeyboardUtil m_keyboard;
	FishingSoundListener m_sound;
};

