#pragma once
#pragma execution_character_set("utf-8")
#include <Windows.h>

class MouseUtil
{
public:
	MouseUtil();
	~MouseUtil();

	static void SetCursorPos(int x, int y);
	static bool GetCursorPos(POINT *pPoint);

	static void ClickLeftButton(int interval = 0);
	static void ClickRightButton(int interval = 0);

	static bool MoveCursor(POINT pos, int interval = 0);
};

