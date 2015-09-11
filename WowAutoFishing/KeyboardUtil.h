#pragma once
#include <Windows.h>

class KeyboardUtil
{
public:
	KeyboardUtil();
	~KeyboardUtil();

	static void PressKey(int key, int interval = 10);
};

