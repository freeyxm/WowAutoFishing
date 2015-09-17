#pragma once
#pragma execution_character_set("utf-8")
#include <Windows.h>
#include <list>

#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define RGB_R(color) (color & 0xff)
#define RGB_G(color) ((color >> 8) & 0xff)
#define RGB_B(color) ((color >> 16) & 0xff)

class ImageUtil
{
public:
	ImageUtil();
	~ImageUtil();

	static bool GetWindowSnapshot(HWND hwnd, int x, int y, int w, int h, char *lpBits, BITMAPINFOHEADER *pbi = NULL);
	static bool CreateBMPFile(LPTSTR pszFile, PBITMAPINFO pbi, char *lpBits);

	static HBITMAP LoadImage2(LPTSTR pszFile, BITMAP *pBitmap);

	static bool TransToGray(BITMAP bitmap, LPTSTR pszFile);

	static void FindColor(char *lpBits, int w, int h, int color, int range, std::list<POINT> &points);
	static void FindGray(char *lpBits, int w, int h, int gray, int range, std::list<POINT> &points, unsigned int maxCount = 0);
	static bool SelectBestPoint(std::list<POINT> points, int radius, POINT &p);
};

