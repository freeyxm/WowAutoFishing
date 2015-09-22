#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "MouseUtil.h"
#include <cmath>
#include <cstdlib>

MouseUtil::MouseUtil()
{
}

MouseUtil::~MouseUtil()
{
}

void MouseUtil::SetCursorPos(int x, int y)
{
	::SetCursorPos(x, y);
}

bool MouseUtil::GetCursorPos(POINT *pPoint)
{
	return ::GetCursorPos(pPoint) ? true : false;
}

void MouseUtil::ClickLeftButton(int interval)
{
	::mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	::Sleep(interval > 0 ? interval : ::rand() % 10 + 10);
	::mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void MouseUtil::ClickRightButton(int interval)
{
	::mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
	::Sleep(interval > 0 ? interval : ::rand() % 10 + 10);
	::mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
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
			::mouse_event(MOUSEEVENTF_MOVE, step, new_y - sp.y, 0, 0);
			sp.y = new_y;
		}
		else
		{
			sp.y += step;
			new_x = (sp.y - tp.y) * dif_x / dif_y + tp.x;
			::mouse_event(MOUSEEVENTF_MOVE, new_x - sp.x, step, 0, 0);
			sp.x = new_x;
		}

		//::SetCursorPos(sp.x, sp.y);
		::Sleep(interval > 0 ? interval : ::rand() % 5 + 5);
	}

	return true;
}

