#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "MouseUtil.h"
#include <cmath>
#include <cstdlib>

MouseUtil::MouseUtil(HWND hwnd)
	: m_hwnd(hwnd)
{
}

MouseUtil::~MouseUtil()
{
}

inline void MouseUtil::_Sleep(int interval)
{
	::Sleep(interval > 0 ? interval : ::rand() % 5 + 5);
}

void MouseUtil::SetCursorPos(int x, int y)
{
	if (!m_hwnd)
	{
		::SetCursorPos(x, y);
	}
	else
	{
		// below code not work! need to repair!!!
		//::PostMessage(m_hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(x, y));

		RECT rect;
		if (::GetWindowRect(m_hwnd, &rect))
		{
			x += rect.left;
			y += rect.top;
		}
		::SetCursorPos(x, y);
	}
}

bool MouseUtil::GetCursorPos(POINT *pPoint)
{
	return ::GetCursorPos(pPoint) ? true : false;
}

void MouseUtil::ClickLeftButton(int interval)
{
	// need to repair!!!
	::mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	_Sleep(interval);
	::mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void MouseUtil::ClickRightButton(int interval)
{
	// need to repair!!!
	::mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
	_Sleep(interval);
	::mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
}

inline void MouseUtil::MoveCursor(int x, int y)
{
	// need to repair!!!
	::mouse_event(MOUSEEVENTF_MOVE, x, y, 0, 0);
}

bool MouseUtil::MoveCursor(POINT tp, int interval)
{
	POINT sp;
	if (!::GetCursorPos(&sp))
		return false;

	int dif_x = sp.x - tp.x;
	int dif_y = sp.y - tp.y;

	int step = 1;
	bool step_by_x = ::abs(dif_x) < ::abs(dif_y);
	if (step_by_x)
	{
		if (dif_x > 0)
			step = -step;
	}
	else
	{
		if (dif_y > 0)
			step = -step;
	}

	int new_x, new_y;
	while (sp.x != tp.x || sp.y != tp.y)
	{
		if (step_by_x)
		{
			sp.x += step;
			new_y = (sp.x - tp.x) * dif_y / dif_x + tp.y;
			MoveCursor(step, new_y - sp.y);
			sp.y = new_y;
		}
		else
		{
			sp.y += step;
			new_x = (sp.y - tp.y) * dif_x / dif_y + tp.x;
			MoveCursor(new_x - sp.x, step);
			sp.x = new_x;
		}

		//::SetCursorPos(sp.x, sp.y);
		_Sleep(interval);
	}

	return true;
}

