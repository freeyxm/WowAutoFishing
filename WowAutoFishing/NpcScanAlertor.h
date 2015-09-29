﻿#pragma once
#pragma execution_character_set("utf-8")
#include "AudioUtil/AudioRenderer.h"
#include "Win32Util/KeyboardUtil.h"

class NpcScanAlertor
{
public:
	NpcScanAlertor(HWND hwnd);
	~NpcScanAlertor();

	bool Init();
	void Start();

private:
	void PlayAlarm();
	bool CheckNpcHeadIcon(bool isRare);

private:
	HWND m_hwnd;
	AudioFrameStorage m_alarm;
	AudioRenderer m_audioPlayer;
	KeyboardUtil m_keyboard;
	
	char *m_lpBits; // 窗口位图像素数据
	RECT m_targetRect; // 头像矩形框
};

