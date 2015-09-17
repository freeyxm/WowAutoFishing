#pragma once
#pragma execution_character_set("utf-8")

#include "Util\MouseUtil.h"
#include "Util\KeyboardUtil.h"
#include "Util\ImageUtil.h"
#include "SoundListener.h"

enum class FishingState
{
	State_None,
	State_Start,
	State_CheckState, // 检查状态
	State_Bait, // 上鱼饵
	State_ThrowPole, // 甩竿
	State_FindFloat, // 定位鱼漂
	State_WaitBite, // 等待咬钩
	State_Shaduf, // 提竿
	State_End,
};

const int SEC_PER_MINUTE = 60;
const int MAX_BAIT_TIME = 10 * SEC_PER_MINUTE; // 鱼饵持续时间，单位秒
const int MAX_WAIT_TIME = 20; // 最长等待咬钩时间，单位秒

const int SWITCH_TIME_MIN = 100; // 状态切换间隔，单位毫秒
const int SWITCH_TIME_MAX = 900; // 状态切换间隔，单位毫秒

const POINT FLOAT_OFFSET = { 10, 25 }; // 鱼漂偏移，以便鼠标居中（1024x768窗口模式）

class Fisher
{
public:
	Fisher(HWND hwnd, int x, int y, int w, int h);
	~Fisher();

	bool Init();

	void StartFishing();

	void NotifyBite();
	bool CheckTimeout();

private:
	void ActiveWindow();

	bool CheckBaitTime();
	bool DoBait();
	bool DoThrowPole();
	bool DoFindFloat();
	bool DoWaitBite();
	bool DoShaduf();

private:
	HWND m_hwnd; // 魔兽世界窗口句柄
	int m_posX, m_posY;
	int m_width, m_height;
	FishingState m_state;
	time_t m_waitTime; // 等待时间
	time_t m_baitTime; // 上饵时间
	time_t m_throwTime; // 甩竿时间
	bool m_hasBite; // 是否已咬钩
	POINT m_floatPoint;

	int m_throwCount;
	int m_findFloatFailCount;
	int m_timeoutCount;

	char *m_lpBits; // 窗口位图像素数据
	std::list<POINT> m_points;
	
	MouseUtil m_mouse;
	KeyboardUtil m_keyboard;
	SoundListener m_sound;
};

