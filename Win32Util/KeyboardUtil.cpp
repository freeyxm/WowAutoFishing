#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "KeyboardUtil.h"

KeyboardUtil::KeyboardUtil()
{
}

KeyboardUtil::~KeyboardUtil()
{
}

void KeyboardUtil::PressKey(int key, int interval)
{
	::keybd_event(key, ::MapVirtualKey(key, 0), 0, 0);
	::Sleep(interval);
	::keybd_event(key, ::MapVirtualKey(key, 0), KEYEVENTF_KEYUP, 0);
}
