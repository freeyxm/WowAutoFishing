#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "KeyboardUtil.h"
#include <cstdlib>

KeyboardUtil::KeyboardUtil(HWND hwnd)
	: m_hwnd(hwnd)
{
}

KeyboardUtil::~KeyboardUtil()
{
}

void KeyboardUtil::PressKey(int key, int interval)
{
	_KeyDown(key, interval);
	_KeyUp(key, interval);
}

/*
pCtl: C is "Ctrl", A is "Alt", S is "Shift".
e.g. "C+A" is Ctrl + Alt.
*/
void KeyboardUtil::PressKey(int key, const char *pCtl, int interval)
{
	const size_t len = pCtl != NULL ? ::strlen(pCtl) : 0;
	for (size_t i = 0; i < len; ++i)
	{
		switch (pCtl[i])
		{
		case 'C':
			_KeyDown(VK_CONTROL, interval); // Ctrl
			break;
		case 'A':
			_KeyDown(VK_MENU, interval); // Alt
			break;
		case 'S':
			_KeyDown(VK_SHIFT, interval); // Shift
			break;
		default:
			break;
		}
	}

	_KeyDown(key, interval);
	_KeyUp(key, interval);

	for (size_t i = 1; i <= len; ++i)
	{
		switch (pCtl[len - i])
		{
		case 'C':
			_KeyUp(VK_CONTROL, interval); // Ctrl
			break;
		case 'A':
			_KeyUp(VK_MENU, interval); // Alt
			break;
		case 'S':
			_KeyUp(VK_SHIFT, interval); // Shift
			break;
		default:
			break;
		}
	}
}

void KeyboardUtil::_KeyDown(int key, int interval)
{
	if (!m_hwnd)
	{
		::keybd_event(key, ::MapVirtualKey(key, 0), 0, 0);
	}
	else
	{
		::PostMessage(m_hwnd, WM_KEYDOWN, key, 0);
	}
	::Sleep(interval > 0 ? interval : (5 + ::rand() % 5));
}

void KeyboardUtil::_KeyUp(int key, int interval)
{
	if (!m_hwnd)
	{
		::keybd_event(key, ::MapVirtualKey(key, 0), KEYEVENTF_KEYUP, 0);
	}
	else
	{
		::PostMessage(m_hwnd, WM_KEYUP, key, 0);
	}
	::Sleep(interval > 0 ? interval : (5 + ::rand() % 5));
}
