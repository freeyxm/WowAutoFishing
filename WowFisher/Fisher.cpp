#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "Fisher.h"
#include "WowFisherDlg.h"
#include <ctime>

static UINT __stdcall FishingTheadProc(LPVOID param);

const int SEC_PER_MINUTE = 60;
const int MAX_BAIT_TIME = 10 * SEC_PER_MINUTE; // 鱼饵持续时间，单位秒
const int MAX_WAIT_TIME = 23; // 最长等待咬钩时间，单位秒

const POINT FLOAT_OFFSET = { 10, 25 }; // 鱼漂偏移，以便鼠标居中（1024x768窗口模式）


Fisher::Fisher(HWND hwnd, int x, int y, int w, int h)
	: m_hWndWOW(hwnd), m_keyboard(hwnd), m_mouse(hwnd), m_hWndMain(0)
	, m_posX(x), m_posY(y), m_width(w), m_height(h), m_sound(this)
	, m_bInited(false), m_hThreadFishing(NULL)
{
	ResetStatistics();
}

Fisher::~Fisher()
{
	Stop();

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
	m_sound.SetCheckTimeoutProc(&Fisher::CheckTimeout);

	if (!m_sound.Init())
	{
		return false;
	}

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
	::SetForegroundWindow(m_hWndWOW);
	::SetActiveWindow(m_hWndWOW);
}

bool Fisher::Start()
{
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

void Fisher::StartFishing()
{
	m_state = FishingState::State_CheckState;
	m_bFishing = true;

	wprintf(L"---------------------------------------\n");

	while (m_bFishing)
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
			Bait();
			m_state = FishingState::State_CheckState;
			break;
		case FishingState::State_ThrowPole:
			ThrowPole();
			m_state = FishingState::State_FindFloat;
			break;
		case FishingState::State_FindFloat:
			if (FindFloat())
				m_state = FishingState::State_WaitBiteStart;
			else
				m_state = FishingState::State_WaitFloatHide;
			break;
		case FishingState::State_WaitBiteStart:
			if (WaitBiteStart())
				m_state = FishingState::State_WaitBite;
			else
				m_state = FishingState::State_WaitFloatHide;
			break;
		case FishingState::State_WaitBite:
			WaitBite();
			break;
		case FishingState::State_WaitBiteEnd:
			WaitBiteEnd();
			break;
		case FishingState::State_Shaduf:
			Shaduf();
			m_state = FishingState::State_WaitFloatHide;
			break;
		case FishingState::State_WaitFloatHide:
			WaitFloatHide();
			m_state = FishingState::State_End;
			break;
		case FishingState::State_End:
			StateEnd();
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
bool Fisher::Bait()
{
	PrintStatus(L"上饵...\n");
	ActiveWindow();
	if (m_hotkeyBite3 != 0)
	{
		m_keyboard.PressKey(m_hotkeyBite3 & 0xff, (m_hotkeyBite3 >> 8) & 0xff, 0);
		Sleep(2100);
	}
	if (m_hotkeyBite2 != 0)
	{
		m_keyboard.PressKey(m_hotkeyBite2 & 0xff, (m_hotkeyBite2 >> 8) & 0xff, 0);
		Sleep(2100);
	}
	if (m_hotkeyBite1 != 0)
	{
		m_keyboard.PressKey(m_hotkeyBite1 & 0xff, (m_hotkeyBite1 >> 8) & 0xff, 0);
		m_waitTime += 2000; // 上饵施法2秒
	}
	m_baitTime = time(NULL);
	return true;
}

// 甩竿
bool Fisher::ThrowPole()
{
	PrintStatus(L"甩竿...\n");
	++m_throwCount;
	ActiveWindow();
	m_keyboard.PressKey(m_hotkeyThrow & 0xff, (m_hotkeyThrow >> 8) & 0xff, 0);
	m_waitTime += 1500; // 甩竿完延迟1.5秒再寻找鱼漂
	m_throwTime = time(NULL);
	::SendMessage(m_hWndMain, WMU_UPDATE_STATISTICS, 0, 0);
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
bool Fisher::FindFloat()
{
	PrintStatus(L"寻找鱼漂...\n");
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
	::SendMessage(m_hWndMain, WMU_UPDATE_STATISTICS, 0, 0);
	return false;
}

// 等待上钩
bool Fisher::WaitBiteStart()
{
	PrintStatus(L"等待上钩...\n");
	m_bHasBite = false;
	m_bTimeout = false;
	
	return m_sound.Start();
}

void Fisher::WaitBite()
{
	if (m_bHasBite || m_bTimeout)
	{
		m_state = FishingState::State_WaitBiteEnd;
	}
}

void Fisher::WaitBiteEnd()
{
	m_sound.Stop();

	if (m_bHasBite)
	{
		m_state = FishingState::State_Shaduf;
	}
	else
	{
		PrintStatus(L"没有鱼儿上钩！\n");
		++m_timeoutCount;
		m_state = FishingState::State_WaitFloatHide;
		::SendMessage(m_hWndMain, WMU_UPDATE_STATISTICS, 0, 0);
	}
}

// 提竿
bool Fisher::Shaduf()
{
	PrintStatus(L"提竿...\n");
	ActiveWindow();

	RECT rect;
	if (!::GetWindowRect(m_hWndWOW, &rect))
	{
		printf("GetWindowRect has failed.");
		return false;
	}

	m_mouse.SetCursorPos(m_floatPoint.x + rect.left, m_floatPoint.y + rect.top); // 重新设定鼠标，防止中间移动而在错误的位置。
	Sleep(10);
	m_mouse.ClickRightButton();
	return true;
}

void Fisher::WaitFloatHide()
{
	m_waitTime += 2500; // 等待物品进包及旧鱼漂消失
}

void Fisher::StateEnd()
{
	wprintf(L"throw: %d, timeout: %d, float: %d\n", m_throwCount, m_timeoutCount, m_findFloatFailCount);
	wprintf(L"---------------------------------------\n");
}

void Fisher::NotifyBite()
{
	PrintStatus(L"咬钩了！\n");
	m_bHasBite = true;
}

bool Fisher::CheckTimeout()
{
	time_t now = time(NULL);
	bool isTimeout = now - m_throwTime >= MAX_WAIT_TIME;
	if (isTimeout && m_state == FishingState::State_WaitBite)
	{
		m_bTimeout = true;
	}
	return isTimeout;
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

void Fisher::ResetStatistics()
{
	m_throwCount = m_timeoutCount = m_findFloatFailCount = 0;
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
