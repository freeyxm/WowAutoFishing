#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "NpcScanAlertor.h"
#include "Win32Util/ImageUtil.h"
#include "AudioUtil/WaveCreator.h"


NpcScanAlertor::NpcScanAlertor(HWND hwnd)
	: m_hwnd(hwnd), m_keyboard(hwnd)
	, m_bRare(true), m_bRed(false)
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
	
	m_lpBits = new char[w * h * 4];
	if (!m_lpBits)
		return false;

	WAVEFORMATEX wfx;
	AudioFrameData *pFrame = new AudioFrameData();
	if (!pFrame)
		return false;
	if (!WaveCreator::LoadWave("f:/npcscan.wav", &wfx, &pFrame->pData, &pFrame->nDataLen, NULL))
		return false;

	m_alarm.PushBack(pFrame);

	m_audioPlayer.SetSource(&m_alarm);
	m_audioPlayer.SetSourceFormat(&wfx);
	if (FAILED(m_audioPlayer.Init()))
	{
		printf("audio player init failed.\n");
		return false;
	}

	printf("init success.\n");
	return true;
}

void NpcScanAlertor::Start(bool bRare, bool bRed)
{
	bool isLeft = true;
	time_t move_time_interval = 10000;
	time_t move_time = 0;

	m_bRare = bRare;
	m_bRed = bRed;

	while (true)
	{
		m_keyboard.PressKey(VK_F9, "A+C");

		DWORD sleepTime = 2000 + rand() % 500;
		if (move_time <= sleepTime)
		{
			if (isLeft)
				m_keyboard.PressKey(VK_F7, "A+C"); // 左转
			else
				m_keyboard.PressKey(VK_F8, "A+C"); // 右转
			isLeft = !isLeft;
			move_time = move_time_interval;
		}
		else
		{
			move_time -= sleepTime;
		}

		if (CheckNpcHeadIcon())
		{
			PlayAlarm();
		}

		::Sleep(sleepTime);
	}
}

void NpcScanAlertor::PlayAlarm()
{
	if (m_audioPlayer.IsDone())
	{
		m_audioPlayer.StartRender();
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
		printf("GetWindowSnapshot failed!\n");
		return false;
	}

	const int color_yellow = RGB(177, 175, 0); // 中立怪血条颜色 (300, 33)
	const int color_yellow_range = RGB(20, 20, 10);

	const int color_red = RGB(181, 0, 0); // 敌对怪血条颜色 (300, 33)
	const int color_red_range = RGB(20, 10, 10);

	// 根据血条颜色判断是否选中了目标
	int yellow_count = 0, red_count = 0;
	int min_x = 8, max_x = 108, step_x = 2;
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
		//printf("hasTarget failed\n");
		return false;
	}
	bool isRed = red_count > yellow_count;
	if (m_bRed != isRed)
		return false;


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

			if (count * 1.0f / (rects[i].right * rects[i].bottom) > 0.5f)
			{
				++rare_count;
			}
		}
		if (rare_count <= total_count / 2)
		{
			printf("isRare failed.\n");
			return false;
		}
	}

	printf("CheckNpcHeadIcon pass, rare = %d, color = %s\n", m_bRare, isRed ? "red" : "yellow");
	return true;
}