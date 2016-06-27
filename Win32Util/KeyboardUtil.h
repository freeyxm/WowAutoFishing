#pragma once
#pragma execution_character_set("utf-8")
#include <Windows.h>

class KeyboardUtil
{
public:
	KeyboardUtil(HWND hwnd);
	~KeyboardUtil();

	void PressKey(int key, int interval = 0);
	void PressKey(int key, int modifiers, int interval = 0);
	void PressKey(int key, const char *modifiers, int interval = 0);

private:
	void KeyDown(int key, int interval = 0);
	void KeyUp(int key, int interval = 0);
	void KeyEvent(char key, int interval, void(KeyboardUtil::*)(int, int));

private:
	HWND m_hwnd;
	INPUT m_singleKeyInputs[2];
};

