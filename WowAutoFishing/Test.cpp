#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "Win32Util/ImageUtil.h"
#include "Win32Util/MouseUtil.h"
#include "Win32Util/KeyboardUtil.h"

HHOOK g_kbHook;

HWND test_getwnd();
void test_keyboard(char ch);
void test_mouse(HWND hwnd);

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam);
HHOOK InstallKeyboardHook(HINSTANCE hinst, HOOKPROC proc, DWORD tid);
void UninstallHook(HHOOK hook);

HWND test_getwnd()
{
	HWND hwnd = ::FindWindow(NULL, L"魔兽世界");
	//HWND hwnd = ::FindWindow(L"notepad", NULL);
	printf("hwnd=%ld\n", hwnd);
	if (hwnd)
	{
		char buf[128];
		::GetWindowTextA(hwnd, buf, sizeof(buf));
		printf("title: %s\n", buf);

		//HINSTANCE hInstance = ::GetModuleHandle(0);
		HINSTANCE hInstance = (HINSTANCE)::GetWindowLong(hwnd, GWL_HINSTANCE);
		printf("hInstance=%ld\n", hInstance);

		::GetModuleFileNameA(hInstance, buf, sizeof(buf));
		printf("GetModuleFileNameA: %s\n", buf);

		DWORD tid = 0;
		tid = ::GetWindowThreadProcessId(hwnd, NULL);
		printf("tid=%ld\n", tid);

		g_kbHook = InstallKeyboardHook(hInstance, LowLevelKeyboardProc, 0);
		printf("InstallKeyboardHook %s\n", g_kbHook ? "success" : "failed");
		if (!g_kbHook)
		{
			printf("errcode = %d\n", GetLastError());
		}

		::SetForegroundWindow(hwnd);
		::SetActiveWindow(hwnd);

		test_mouse(hwnd);

		//::PostMessage(hwnd, WM_CHAR, '1', 0);
		//::SendMessage(hwnd, WM_CHAR, VK_F1, 0);
		//printf("SendMessage done.\n", buf);

		//::EnumWindows()

		if (g_kbHook)
		{
			UninstallHook(g_kbHook);
		}
	}
	return hwnd;
}

void test_keyboard(char ch)
{
	::keybd_event(ch, ::MapVirtualKey(ch, 0), 0, 0);
	Sleep(100);
	::keybd_event(ch, ::MapVirtualKey(ch, 0), KEYEVENTF_KEYUP, 0);

	//INPUT input = {0};
	//input.type = INPUT_HARDWARE;
	//input.ki.wVk = VK_F1;
	//::SendInput(1, &input, sizeof(INPUT));
}

void test_mouse(HWND hwnd)
{
	//POINT point;
	//if (::GetCursorPos(&point))
	//{
	//	printf("point %d,%d\n", point.x, point.y);
	//}

	::SetCursorPos(1024 / 2, 768 / 2);
	Sleep(100);

	HCURSOR cur = ::GetCursor();

	CURSORINFO curinfo;
	curinfo.cbSize = sizeof(curinfo);
	if (::GetCursorInfo(&curinfo))
	{
		printf("CURSORINFO: %ld, %ld, %ld, [%ld,%ld]\n", curinfo.cbSize, curinfo.flags, curinfo.hCursor, curinfo.ptScreenPos.x, curinfo.ptScreenPos.y);
	}
	else
	{
		printf("GetCursorInfo errcode = %d\n", GetLastError());
	}

	ICONINFO icon;
	::GetIconInfo(cur, &icon);

	BITMAP bm;
	::GetObject(icon.hbmMask, sizeof(BITMAP), &bm);

	DWORD curlong = ::GetClassLong(hwnd, GCL_HCURSOR);
	printf("GCL_HCURSOR: %ld\n", curlong);

	//::mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	//::Sleep(100);
	//::mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	printf("Keyboard: %d, %d, %d\n", code, wParam, lParam);
	return CallNextHookEx(g_kbHook, code, wParam, lParam);
}

HHOOK InstallKeyboardHook(HINSTANCE hinst, HOOKPROC proc, DWORD tid)
{
	return ::SetWindowsHookEx(WH_KEYBOARD_LL, proc, hinst, tid);
}

void UninstallHook(HHOOK hook)
{
	::UnhookWindowsHookEx(hook);
}

void test(char ch)
{
	switch (ch)
	{
	case 'b':
		break;
	case 'e':
		break;
	default:
		break;
	}
}