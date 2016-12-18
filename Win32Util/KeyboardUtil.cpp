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
		KeyDown(key, interval);
		KeyUp(key, interval);
	}
}

void KeyboardUtil::PressKey(int key, int modifiers, int interval)
{
	if (modifiers & HOTKEYF_CONTROL)
		KeyDown(VK_CONTROL, interval); // Ctrl
	if (modifiers & HOTKEYF_ALT)
		KeyDown(VK_MENU, interval); // Alt
	if (modifiers & HOTKEYF_SHIFT)
		KeyDown(VK_SHIFT, interval); // Shift

	KeyDown(key, interval);
	KeyUp(key, interval);

	if (modifiers & HOTKEYF_SHIFT)
		KeyUp(VK_SHIFT, interval); // Shift
	if (modifiers & HOTKEYF_ALT)
		KeyUp(VK_MENU, interval); // Alt
	if (modifiers & HOTKEYF_CONTROL)
		KeyUp(VK_CONTROL, interval); // Ctrl
}

/*
pCtl: C is "Ctrl", A is "Alt", S is "Shift".
e.g. "C+A" is Ctrl + Alt.
*/
void KeyboardUtil::PressKey(int key, const char *modifiers, int interval)
{
	const int len = modifiers != NULL ? (int)::strlen(modifiers) : 0;
	for (int i = 0; i < len; ++i)
	{
		KeyEvent(modifiers[i], interval, &KeyboardUtil::KeyDown);
	}

	KeyDown(key, interval);
	KeyUp(key, interval);

	for (int i = len - 1; i >= 0; --i)
	{
		KeyEvent(modifiers[i], interval, &KeyboardUtil::KeyUp);
	}
}

void KeyboardUtil::KeyEvent(char key, int interval, void(KeyboardUtil::*_KeyOp)(int, int))
{
	switch (key)
	{
	case 'C':
		(this->*_KeyOp)(VK_CONTROL, interval); // Ctrl
		break;
	case 'A':
		(this->*_KeyOp)(VK_MENU, interval); // Alt
		break;
	case 'S':
		(this->*_KeyOp)(VK_SHIFT, interval); // Shift
		break;
	default:
		break;
	}
}

void KeyboardUtil::KeyDown(int key, int interval)
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

void KeyboardUtil::KeyUp(int key, int interval)
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
