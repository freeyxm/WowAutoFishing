#pragma once
#pragma execution_character_set("utf-8")
#include "AudioUtil/AudioRenderer.h"
#include "Win32Util/KeyboardUtil.h"
#include <string>

class NpcScanAlertor
{
public:
	enum class ColorType
	{
		Color_Red = 0x01,
		Color_Yellow = 0x02,
	};

public:
	NpcScanAlertor(HWND hwnd, AudioRenderer *pAudioPlayer);
	~NpcScanAlertor();

	bool Init();
	void Start(int colorType, bool bRare);
	void Start();
	void Stop();
	void Update(int deltaTime);

	bool IsRunning() const;

	long GetPid() const;

	void SetName(const char* name);
	const char* GetName() const;

	void SetColorType(int colorType);
	int GetColorType();

	void SetRare(bool bRare);
	bool IsRare();

private:
	void PlayAlarm();
	bool CheckNpcHeadIcon();

private:
	HWND m_hwnd;
	DWORD m_pid; // 窗口所属进程ID
	AudioRenderer *m_pAudioPlayer;
	KeyboardUtil m_keyboard;
	
	char *m_lpBits; // 窗口位图像素数据
	RECT m_targetRect; // 头像矩形框

	bool m_bRare;
	int m_colorType;

	bool m_bRunning;
	long m_searchTargetTime;
	long m_moveTime;
	bool m_moveLeft;

	std::string m_name;
};

