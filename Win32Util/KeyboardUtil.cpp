#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "KeyboardUtil.h"
#include <cstdlib>
#include <CommCtrl.h>

KeyboardUtil::KeyboardUtil(HWND hwnd)
	: m_hwnd(hwnd)
{
	::memset(m_singleKeyInputs, 0, sizeof(m_singleKeyInputs));
	m_singleKeyInputs[0].type = INPUT_KEYBOARD;
	m_singleKeyInputs[1].type = INPUT_KEYBOARD;
	m_singleKeyInputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
}

KeyboardUtil::~KeyboardUtil()
{
}

void KeyboardUtil::PressKey(int key, int interval)
{
	if (!m_hwnd)
	{
		m_singleKeyInputs[0].ki.wVk = m_singleKeyInputs[1].ki.wVk = key;
		::SendInput(2, m_singleKeyInputs, sizeof(INPUT));
	}
	else
	{
		_KeyDown(key, interval);
		_KeyUp(key, interval);
	}
}

void KeyboardUtil::PressKey(int key, int modifiers, int interval)
{
	if (modifiers & HOTKEYF_CONTROL)
		_KeyDown(VK_CONTROL, interval); // Ctrl
	if (modifiers & HOTKEYF_ALT)
		_KeyDown(VK_MENU, interval); // Alt
	if (modifiers & HOTKEYF_SHIFT)
		_KeyDown(VK_SHIFT, interval); // Shift

	_KeyDown(key, interval);
	_KeyUp(key, interval);

	if (modifiers & HOTKEYF_SHIFT)
		_KeyUp(VK_SHIFT, interval); // Shift
	if (modifiers & HOTKEYF_ALT)
		_KeyUp(VK_MENU, interval); // Alt
	if (modifiers & HOTKEYF_CONTROL)
		_KeyUp(VK_CONTROL, interval); // Ctrl
}

/*
pCtl: C is "Ctrl", A is "Alt", S is "Shift".
e.g. "C+A" is Ctrl + Alt.
*/
void KeyboardUtil::PressKey(int key, const char *modifiers, int interval)
{
	const size_t len = modifiers != NULL ? ::strlen(modifiers) : 0;
	for (size_t i = 0; i < len; ++i)
	{
		switch (modifiers[i])
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
		switch (modifiers[len - i])
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
