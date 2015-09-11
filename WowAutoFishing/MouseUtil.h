#pragma once
#include <Windows.h>

class MouseUtil
{
public:
	MouseUtil();
	~MouseUtil();

	static void SetCursorPos(int x, int y);
	static bool GetCursorPos(POINT *pPoint);

	static void ClickLeftButton(int interval = 10);
	static void ClickRightButton(int interval = 10);

	static bool MoveCursor(POINT pos, int interval = 10);
};

