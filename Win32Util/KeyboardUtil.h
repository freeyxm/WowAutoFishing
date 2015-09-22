#pragma once
#pragma execution_character_set("utf-8")
#include <Windows.h>

class KeyboardUtil
{
public:
	KeyboardUtil();
	~KeyboardUtil();

	static void PressKey(int key, int interval = 0);
};

