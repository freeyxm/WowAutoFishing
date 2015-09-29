// WowAutoFishing.cpp : Defines the entry point for the console application.
//
//#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <cmath>
#include "Win32Util/ImageUtil.h"
#include "Win32Util/MouseUtil.h"
#include "Win32Util/KeyboardUtil.h"
#include "Win32Util/AudioCapture.h"
#include "Fisher.h"
#include "AudioListener.h"
#include <locale.h>
#include <cstdlib>
#include <ctime>

void test(char ch);

int _tmain(int argc, _TCHAR* argv[])
{
	HWND hwnd = NULL;
	KeyboardUtil *pKeyboard = NULL;
	MouseUtil mouse;

	::srand((unsigned int)::time(NULL));

	::CoInitialize(NULL);

	setlocale(LC_CTYPE, "");

	wprintf(L">开始...\n");

	while (true)
	{
		int ch = getchar();
		if (ch == '`')
			break;
		if (ch == VK_RETURN || ch >= 0x0A && ch <= 0x0B)
			continue;

		if (ch == 'w')
		{
			hwnd = ::FindWindow(NULL, L"魔兽世界");
			if (!hwnd)
				printf("FindWindow failed.\n");
			else
				printf("FindWindow success.\n");
		}
		else if (ch == 'f')
		{
			HWND hwnd = ::FindWindow(NULL, L"魔兽世界");
			if (hwnd)
			{
				int x = 200, y = 20;
				int w = 600, h = 400;
				Fisher fisher(hwnd, x, y, w, h);
				if (fisher.Init())
				{
					fisher.StartFishing();
				}
			}
			else
			{
				printf("FindWindow failed.\n");
			}
		}
		else if (ch == 'p')
		{
			::SetForegroundWindow(hwnd);
			::SetActiveWindow(hwnd);

			int x = 200, y = 20;
			int w = 600, h = 400;
			char *lpBits = (char*)malloc(w * h * 4);

			BITMAPINFOHEADER bi;
			if(ImageUtil::GetWindowSnapshot(hwnd, x, y, w, h, lpBits, &bi))
			{
				wchar_t path[] = L"capture.bmp";
				if (ImageUtil::CreateBMPFile(path, (PBITMAPINFO)&bi, lpBits))
				{
					printf("window image save to %ls\n", path);
				}
				
				std::list<POINT> points;
				ImageUtil::FindGray((char*)lpBits, w, h, 20, 3, points);
				
				POINT p;
				if (ImageUtil::SelectBestPoint(points, 30, p))
				{
					p.x += x;
					p.y = h - p.y + y;
					printf("FindColor: %d, %d\n", p.x, p.y);

					POINT offset = { 20, 30 };
					p.x += offset.x;
					p.y += offset.y;

					mouse.SetCursorPos(p.x, p.y);
					//mouse.MoveCursor(p, 10);
				}
			}

			free(lpBits);
			continue;
		}
		else if (ch == 'o')
		{
			BITMAP bitmap;
			HBITMAP hBitmap = ImageUtil::LoadImage2(L"123.bmp", &bitmap);
			if (hBitmap)
			{
				int w = bitmap.bmWidth;
				int h = bitmap.bmHeight;

				std::list<POINT> points;
				ImageUtil::FindColor((char*)bitmap.bmBits, w, h, RGB(40, 13, 11), RGB(3, 3, 3), points);
				ImageUtil::FindColor((char*)bitmap.bmBits, w, h, RGB(32, 38, 62), RGB(3, 3, 3), points);

				POINT p;
				if (ImageUtil::SelectBestPoint(points, 30, p))
				{
					printf("FindColor: %d, %d\n", p.x, p.y);
				}

				//ImageUtil::Gray(bitmap, L"gray.bmp");

				::DeleteObject(hBitmap);
			}
		}
		else if (ch == 'm')
		{
			POINT tp = { 100, 200 };
			mouse.MoveCursor(tp, 10);
		}
		else if (ch == 's')
		{
			AudioCapture *pAudio = new AudioListener(NULL);
			if (SUCCEEDED(pAudio->Init()))
			{
				if (SUCCEEDED(pAudio->Start()))
				{
					pAudio->Capture();
					pAudio->Stop();
				}
			}
			delete pAudio;
		}
		else if (ch == 'k')
		{
			bool isLeft = true;
			time_t move_time_interval = 5000;
			time_t move_time = 0;

			pKeyboard = new KeyboardUtil(hwnd);

			while (true)
			{
				pKeyboard->PressKey(VK_F9, "A+C");

				DWORD sleepTime = 5000 + rand() % 500;
				if (move_time <= sleepTime)
				{
					if (isLeft)
						pKeyboard->PressKey(VK_F7, "A+C"); // 左转
					else
						pKeyboard->PressKey(VK_F8, "A+C"); // 右转
					isLeft = !isLeft;
					move_time = move_time_interval;
				}
				else
				{
					move_time -= sleepTime;
				}

				::Sleep(sleepTime);
			}
		}
		else
		{
			test(ch);
		}
	}

	::CoUninitialize();

	return 0;
}



