#pragma once
#pragma execution_character_set("utf-8")
#include <Windows.h>

class KeyboardUtil
{
public:
	KeyboardUtil();
	~KeyboardUtil();

	static void PressKey(int key, int interval = 0);
	static void PressKey(int key, const char *pCtl, int interval = 0);

private:
	static void _KeyDown(int key, int interval = 0);
	static void _KeyUp(int key, int interval = 0);
};

