#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "KeyboardUtil.h"
#include <cstdlib>

KeyboardUtil::KeyboardUtil()
{
}

KeyboardUtil::~KeyboardUtil()
{
}

void KeyboardUtil::PressKey(int key, int interval)
{
	::keybd_event(key, ::MapVirtualKey(key, 0), 0, 0);
	::Sleep(interval > 0 ? interval : ::rand() % 10 + 10);
	::keybd_event(key, ::MapVirtualKey(key, 0), KEYEVENTF_KEYUP, 0);
}
