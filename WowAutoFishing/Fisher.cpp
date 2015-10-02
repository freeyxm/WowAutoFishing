﻿#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "Fisher.h"
#include <ctime>

Fisher::Fisher(HWND hwnd, int x, int y, int w, int h)
	: m_hwnd(hwnd), m_keyboard(hwnd), m_mouse(hwnd)
	, m_posX(x), m_posY(y), m_width(w), m_height(h), m_sound(this)
{
	Init();
}

Fisher::~Fisher()
{
}

bool Fisher::Init()
{
	m_lpBits = (char*)malloc(m_width * m_height * 4);
	if (!m_lpBits)
	{
		return false;
	}

	m_sound.SetNotifyBiteProc(&Fisher::NotifyBite);
	m_sound.SetCheckTimeoutProc(&Fisher::CheckTimeout);

	if (FAILED(m_sound.Init()))
	{
		return false;
	}

	return true;
}

void Fisher::ActiveWindow()
{
	::SetForegroundWindow(m_hwnd);
	::SetActiveWindow(m_hwnd);
}


void Fisher::StartFishing()
{
	m_state = FishingState::State_CheckState;

	m_throwCount = 0;
	m_timeoutCount = 0;
	m_findFloatFailCount = 0;

	wprintf(L"---------------------------------------\n");

	while (true)
	{
		m_waitTime = 0;

		switch (m_state)
		{
		case FishingState::State_Start:
		case FishingState::State_CheckState:
			if (CheckBaitTime())
				m_state = FishingState::State_Bait;
			else
				m_state = FishingState::State_ThrowPole;
			break;
		case FishingState::State_Bait:
			DoBait();
			m_state = FishingState::State_CheckState;
			break;
		case FishingState::State_ThrowPole:
			if(DoThrowPole())
				m_state = FishingState::State_FindFloat;
			else
				m_state = FishingState::State_End;
			break;
		case FishingState::State_FindFloat:
			if (DoFindFloat())
				m_state = FishingState::State_WaitBite;
			else
				m_state = FishingState::State_End;
			break;
		case FishingState::State_WaitBite:
			if (DoWaitBite())
				m_state = FishingState::State_Shaduf;
			else
				m_state = FishingState::State_End;
			break;
		case FishingState::State_Shaduf:
			DoShaduf();
			m_state = FishingState::State_End;
			break;
		case FishingState::State_End:
			wprintf(L"throw: %d, timeout: %d, float: %d\n", m_throwCount, m_timeoutCount, m_findFloatFailCount);
			wprintf(L"---------------------------------------\n");
			m_state = FishingState::State_Start;
			break;
		default:
			break;
		}

		m_waitTime += 20 + ::rand() % 30; // 公共延迟时间。
		::Sleep((DWORD)m_waitTime);
	}
}

// 检查鱼饵是否超时
bool Fisher::CheckBaitTime()
{
	if (m_baitTime == 0)
		return true;

	time_t now = time(NULL);
	return (now - m_baitTime) > (MAX_BAIT_TIME - MAX_WAIT_TIME);
}

// 上饵
bool Fisher::DoBait()
{
	wprintf(L"上饵...\n");
	ActiveWindow();
	m_keyboard.PressKey(0x34); // KEY_4, 特殊鱼饵
	Sleep(10);
	m_keyboard.PressKey(0x33); // KEY_3
	m_waitTime += 2000; // 上饵施法2秒
	m_baitTime = time(NULL);
	return true;
}

// 甩竿
bool Fisher::DoThrowPole()
{
	wprintf(L"甩竿...\n");
	++m_throwCount;
	ActiveWindow();
	m_keyboard.PressKey(0x31); // KEY_1
	m_waitTime += 1500; // 甩竿完延迟1.5秒再寻找鱼漂
	m_throwTime = time(NULL);
	return true;
}

static bool MatchFloatColor(char _r, char _g, char _b)
{
	unsigned r = (unsigned) _r;
	unsigned g = (unsigned) _g;
	unsigned b = (unsigned) _b;
	if (r > g && r > b)
	{
		float r_g = (float)r / g;
		float r_b = (float)r / b;
		if (r_g > 1.7f && r_b > 1.7f)
		{
			return true;
		}
	}
	return false;
}

// 寻找鱼漂
bool Fisher::DoFindFloat()
{
	wprintf(L"寻找鱼漂...\n");
	ActiveWindow();

	BITMAPINFOHEADER bi;
	if (ImageUtil::GetWindowSnapshot(m_hwnd, m_posX, m_posY, m_width, m_height, m_lpBits, &bi))
	{
		m_points.clear();
		unsigned int maxCount = 10000;
		//ImageUtil::FindGray((char*)m_lpBits, m_width, m_height, 20, 3, m_points, maxCount); // 用灰度图寻找鱼漂
		//ImageUtil::FindColor((char*)m_lpBits, m_width, m_height, RGB(180, 55, 40), RGB(20, 15, 10), m_points); // 砮皂寺
		//ImageUtil::FindColor((char*)m_lpBits, m_width, m_height, RGB(60, 10, 10), RGB(10, 10, 10), m_points); // AG
		ImageUtil::FindColor((char*)m_lpBits, m_width, m_height, MatchFloatColor, m_points);

		if(m_points.size() >= maxCount) // 找多太多点，可能是UI开着或水域不合适，无法定位鱼漂
			m_points.clear();

		POINT p;
		if (ImageUtil::SelectBestPoint(m_points, 30, p)) // 根据鱼漂的大概半径选择最优的点
		{
			p.x += m_posX;
			p.y = m_height - p.y + m_posY;
			
			p.x += FLOAT_OFFSET.x;
			p.y += FLOAT_OFFSET.y;

			wprintf(L"找到鱼漂: %d, %d\n", p.x, p.y);

			m_floatPoint = p;
			m_mouse.SetCursorPos(p.x, p.y);
			return true;
		}
	}
	m_waitTime += 1500; // 寻找鱼漂失败，等现鱼漂消失再重新抛竿。
	++m_findFloatFailCount;
	return false;
}

// 等待上钩
bool Fisher::DoWaitBite()
{
	wprintf(L"等待上钩...\n");
	m_hasBite = false;

	m_sound.SetDone(false);
	m_sound.Start();
	m_sound.Capture();
	m_sound.Stop();

	if (m_hasBite)
	{
		m_waitTime += 100 + rand() % 100; // 咬钩后，延迟100-200毫秒提竿
	}

	return m_hasBite;
}

// 提竿
bool Fisher::DoShaduf()
{
	wprintf(L"提竿...\n");
	ActiveWindow();

	RECT rect;
	if (!::GetWindowRect(m_hwnd, &rect))
	{
		printf("GetWindowRect has failed.");
		return false;
	}

	m_mouse.SetCursorPos(m_floatPoint.x + rect.left, m_floatPoint.y + rect.top); // 重新设定鼠标，防止中间移动而在错误的位置。
	Sleep(10);
	m_mouse.ClickRightButton();
	m_waitTime += 2500; // 等待物品进包及旧鱼漂消失
	return true;
}

void Fisher::NotifyBite()
{
	wprintf(L"咬钩了！\n");
	m_hasBite = true;
}

bool Fisher::CheckTimeout()
{
	time_t now = time(NULL);
	bool timeout = now - m_throwTime >= MAX_WAIT_TIME;
	if (timeout)
	{
		wprintf(L"没有鱼儿上钩！\n");
		++m_timeoutCount;
		m_waitTime += 2500; // 等待旧鱼漂消失
	}
	return timeout;
}
