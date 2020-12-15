#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "NpcScanAlertor.h"
#include "Win32Util/Image/ImageUtil.h"
#include "Win32Util/Util/Utility.h"


NpcScanAlertor::NpcScanAlertor(HWND hwnd, AudioRenderer* pAudioPlayer)
    : m_hwnd(hwnd)
    , m_pid(0)
    , m_pAudioPlayer(pAudioPlayer)
    , m_keyboard(hwnd)
    , m_lpBits(NULL)
    , m_bRare(true)
    , m_colorType(0)
    , m_bRunning(false)
    , m_searchTargetTime(0)
    , m_moveTime(0)
    , m_moveLeft(false)
{
	//RECT rect;
	//::GetWindowRect(hwnd, &rect);

	// 头像矩形框
	m_targetRect.left = 280;
	m_targetRect.right = m_targetRect.left + 190;
	m_targetRect.top = 0;
	m_targetRect.bottom = m_targetRect.top + 90;
}


NpcScanAlertor::~NpcScanAlertor()
{
	if (m_lpBits)
	{
		delete[] m_lpBits;
		m_lpBits = NULL;
	}
}

bool NpcScanAlertor::Init()
{
	int w = m_targetRect.right - m_targetRect.left;
	int h = m_targetRect.bottom - m_targetRect.top;
	
	DWORD tid = ::GetWindowThreadProcessId(m_hwnd, &m_pid);
	if (tid == 0)
	{
		printf("GetWindowThreadProcessId faield.\n");
		return false;
	}

	m_lpBits = new char[w * h * 4];
	if (!m_lpBits)
		return false;

	Utility::printf_t("Alerter[%d] \"%s\": Init success.\n", m_pid, m_name.c_str());
	return true;
}

void NpcScanAlertor::Start(int colorType, bool bRare)
{
	m_colorType = colorType;
	m_bRare = bRare;

	Start();
}

void NpcScanAlertor::Start()
{
	m_searchTargetTime = 0;
	m_moveTime = 0;
	m_moveLeft = true;
	m_bRunning = true;
}

void NpcScanAlertor::Stop()
{
	m_bRunning = false;
}

bool NpcScanAlertor::IsRunning() const
{
	return m_bRunning;
}

long NpcScanAlertor::GetPid() const
{
	return m_pid;
}

void NpcScanAlertor::SetName(const char* name)
{
	m_name = name;
}

const char* NpcScanAlertor::GetName() const
{
	return m_name.c_str();
}

void NpcScanAlertor::SetColorType(int colorType)
{
	m_colorType = colorType;
}

int NpcScanAlertor::GetColorType()
{
	return m_colorType;
}

void NpcScanAlertor::SetRare(bool bRare)
{
	m_bRare = bRare;
}

bool NpcScanAlertor::IsRare()
{
	return m_bRare;
}

void NpcScanAlertor::Update(int deltaTime)
{
	if (!m_bRunning)
		return;

	m_searchTargetTime -= deltaTime;
	if (m_searchTargetTime <= 0)
	{
		m_keyboard.PressKey(VK_F9, "A+C");

		if (CheckNpcHeadIcon())
		{
			PlayAlarm();
		}

		m_searchTargetTime = 2000 + ::rand() % 500;
	}

	m_moveTime -= deltaTime;
	if (m_moveTime <= 0)
	{
		// 定时转向，防止掉线
		if (m_moveLeft)
			m_keyboard.PressKey(VK_F7, "A+C"); // 左转
		else
			m_keyboard.PressKey(VK_F8, "A+C"); // 右转
		m_moveLeft = !m_moveLeft;

		m_moveTime = 10000 + ::rand() % 5000; // 10s - 15s
	}
}

void NpcScanAlertor::PlayAlarm()
{
	if (m_pAudioPlayer && m_pAudioPlayer->IsDone())
	{
		m_pAudioPlayer->Start();
	}
}

bool NpcScanAlertor::CheckNpcHeadIcon()
{
	int x = m_targetRect.left;
	int y = m_targetRect.top;
	int w = m_targetRect.right - m_targetRect.left;
	int h = m_targetRect.bottom - m_targetRect.top;

	if (!ImageUtil::GetWindowSnapshot(m_hwnd, x, y, w, h, m_lpBits))
	{
		Utility::printf_t("Alerter[%d] \"%s\" : GetWindowSnapshot failed!\n", m_pid, m_name.c_str());
		Stop();
		return false;
	}

	const int color_yellow = RGB(175, 175, 0); // 中立怪血条颜色 (300, 33)
	const int color_yellow_range = RGB(30, 30, 10);

	const int color_red = RGB(181, 0, 0); // 敌对怪血条颜色 (300, 33)
	const int color_red_range = RGB(20, 10, 10);

	// 根据血条颜色判断是否选中了目标
	int yellow_count = 0, red_count = 0;
	int min_x = 8, max_x = 108, step_x = 2; // 目标头像面板位置
	int ty = 33;
	for (int tx = min_x; tx <= max_x; tx += step_x)
	{
		int color = ImageUtil::GetColor(m_lpBits, w, h, tx, h - ty);
		if (ImageUtil::IsColorAlike(color, color_yellow, color_yellow_range))
			++yellow_count;
		else if(ImageUtil::IsColorAlike(color, color_red, color_red_range))
			++red_count;
	}
	bool hasTarget = max(yellow_count, red_count) * step_x * 1.0f / (max_x - min_x) > 0.4f; // 大于40%颜色符合，判定为已选择目标。
	if (!hasTarget)
	{
		//Utility::printf_t("hasTarget failed\n");
		return false;
	}

	bool isRed = red_count > yellow_count;
	bool isYellow = yellow_count > red_count;
	if (isRed && !(m_colorType & (int)ColorType::Color_Red)
		&& isYellow && !(m_colorType & (int)ColorType::Color_Yellow))
	{
		return false;
	}

	if (m_bRare)
	{
		// 稀有精英，银龙标识采样区域
		RECT rects[] = {
			{ 104, 12, 28, 12 }, // left, top, w, h
			{ 132, 9, 27, 6 },
			{ 170, 28, 12, 30 },
		};
		const int rare_range = 10;
		int total_count = sizeof(rects) / sizeof(RECT);
		int rare_count = 0;
		for (int i = 0; i < total_count; ++i)
		{
			int min_x = rects[i].left;
			int max_x = min_x + rects[i].right;
			int min_y = rects[i].top;
			int max_y = min_y + rects[i].bottom;

			int count = 0;
			for (int x = min_x; x < max_x; ++x)
			{
				for (int y = min_y; y < max_y; ++y)
				{
					int color = ImageUtil::GetColor(m_lpBits, w, h, x, y);
					char r = RGB_R(color);
					char g = RGB_G(color);
					char b = RGB_B(color);
					if (r >= 40 && r <= 90 && abs(r - g) < rare_range && abs(r - b) < rare_range) // 银龙标识为近似灰色
						++count;
				}
			}

			if (count * 1.0f / (rects[i].right * rects[i].bottom) > 0.4f)
			{
				++rare_count;
			}
		}
		if (rare_count <= total_count / 2)
		{
			//Utility::printf_t("isRare failed.\n");
			return false;
		}
	}

	Utility::printf_t("Alerter[%d] \"%s\" : Find target, rare = %d, color = %s\n",
		               m_pid, m_name.c_str(), m_bRare, isRed ? "red" : "yellow");
	return true;
}
