// WowAutoFishing.cpp : Defines the entry point for the console application.
//
#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <cmath>
#include "Util/ImageUtil.h"
#include "Util/MouseUtil.h"
#include "Util/KeyboardUtil.h"
#include "Util/SoundCapture.h"

void test(char ch);

int _tmain(int argc, _TCHAR* argv[])
{
	HWND hwnd = NULL;
	KeyboardUtil keyboard;
	MouseUtil mouse;

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
			SoundCapture sound;
			if (SUCCEEDED(sound.Init()))
			{
				if (SUCCEEDED(sound.Start()))
				{
					sound.Record();
					sound.Stop();
				}
			}
		}
		else
		{
			test(ch);
		}
	}

	return 0;
}



