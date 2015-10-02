#pragma once
#pragma execution_character_set("utf-8")
#include <Windows.h>

class KeyboardUtil
{
public:
	KeyboardUtil(HWND hwnd);
	~KeyboardUtil();

	void PressKey(int key, int interval = 0);
	void PressKey(int key, const char *pCtl, int interval = 0);

private:
	void _KeyDown(int key, int interval = 0);
	void _KeyUp(int key, int interval = 0);

private:
	HWND m_hwnd;
	INPUT m_singleKeyInputs[2];
};

