#pragma once
#pragma execution_character_set("utf-8")
#include "AudioUtil/AudioRenderer.h"
#include "Win32Util/KeyboardUtil.h"

class NpcScanAlertor
{
public:
	enum class ColorType
	{
		Color_Red = 0x01,
		Color_Yellow = 0x02,
	};

public:
	NpcScanAlertor(HWND hwnd);
	~NpcScanAlertor();

	bool Init();
	void Start(int colorType, bool bRare);

private:
	void PlayAlarm();
	bool CheckNpcHeadIcon();

private:
	HWND m_hwnd;
	AudioFrameStorage m_alarm;
	AudioRenderer m_audioPlayer;
	KeyboardUtil m_keyboard;
	
	char *m_lpBits; // 窗口位图像素数据
	RECT m_targetRect; // 头像矩形框

	bool m_bRare;
	int m_colorType;
};

