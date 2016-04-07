#pragma once
#include <Windows.h>

class DcBuffer
{
public:
	DcBuffer(HWND hwnd, int w, int h);
	~DcBuffer();

	HDC GetDC();
	void Clear();

private:
	void Init();

private:
	HWND m_hwnd;
	HDC m_hdcMem;
	HBITMAP m_hBitMap;
	HBRUSH m_hBrush;
	RECT m_rect;
};

