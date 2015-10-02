#pragma once
#pragma execution_character_set("utf-8")
#include <Windows.h>

class MouseUtil
{
public:
	MouseUtil(HWND hwnd);
	~MouseUtil();

	void SetCursorPos(int x, int y);
	bool GetCursorPos(POINT *pPoint);

	void ClickLeftButton(int interval = 0);
	void ClickRightButton(int interval = 0);

	bool MoveCursor(POINT pos, int interval = 0);

private:
	inline void _Sleep(int interval);
	inline void MoveCursor(int x, int y);

private:
	HWND m_hwnd;
};

