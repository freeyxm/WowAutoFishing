#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "WindowPrinter.h"
#include <Winspool.h>
#include <cstdio>
#include <cstdlib>

WindowPrinter::WindowPrinter()
{
}

WindowPrinter::~WindowPrinter()
{
}

/*
功能：获取当前默认打印机的DC
返回：成功返回打印机的DC，失败返回NULL
*/
HDC WindowPrinter::GetPrinterDC()
{
	DWORD dwNeeded, dwReturned;
	HDC hdc = 0;
	::EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, NULL, 0, &dwNeeded, &dwReturned);
	PRINTER_INFO_4* pinfo4 = (PRINTER_INFO_4*)malloc(dwNeeded);
    if (pinfo4)
    {
        ::EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, (BYTE*)pinfo4, dwNeeded, &dwNeeded, &dwReturned);
        hdc = ::CreateDC(NULL, pinfo4->pPrinterName, NULL, NULL);
    }
	free(pinfo4);
	return hdc;
}

/*
功能：打印窗口客户区内容到打印机，自动缩放居中打印
参数: hWnd-被打印窗口的句柄
*/
void WindowPrinter::PrintWindowClientArea(HWND hWnd)
{
	if (hWnd == NULL) return;

	RECT rectClient;
	::GetClientRect(hWnd, &rectClient);
	int width = rectClient.right - rectClient.left;
	int height = rectClient.bottom - rectClient.top;

	// 通过内存DC复制客户区到DDB位图
	HDC hdcWnd = ::GetDC(hWnd);
	HBITMAP hbmWnd = ::CreateCompatibleBitmap(hdcWnd, width, height);
	HDC hdcMem = ::CreateCompatibleDC(hdcWnd);
	::SelectObject(hdcMem, hbmWnd);
	::BitBlt(hdcMem, 0, 0, width, height, hdcWnd, 0, 0, SRCCOPY);

	// 把窗口DDB转为DIB
	BITMAP bmpWnd;
	::GetObject(hbmWnd, sizeof(BITMAP), &bmpWnd);
	BITMAPINFOHEADER bi; // 信息头
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpWnd.bmWidth;
	bi.biHeight = bmpWnd.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32; // 按照每个像素用32bits表示转换
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((bmpWnd.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpWnd.bmHeight; // 每一行像素位32对齐
	char *lpbitmap = (char*)malloc(dwBmpSize); // 像素位指针
	::GetDIBits(hdcMem, hbmWnd, 0, (UINT)bmpWnd.bmHeight, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	::DeleteDC(hdcMem);
	::DeleteObject(hbmWnd);
	::ReleaseDC(hWnd, hdcWnd);

	// 存为文件（可选）
	BITMAPFILEHEADER bmfHeader; // 文件头
	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
	bmfHeader.bfSize = dwSizeofDIB;
	bmfHeader.bfType = 0x4D42;

	FILE* fp = NULL;
	::_wfopen_s(&fp, L"capture.bmp", L"w");
    if (fp)
    {
        ::fwrite(&bmfHeader, sizeof(BITMAPFILEHEADER), 1, fp); // 写入文件头
        ::fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, fp);        // 写入信息头
        ::fwrite(lpbitmap, dwBmpSize, 1, fp);                  // 写入像素位
        ::fclose(fp);
        fp = NULL;
    }

	// StretchDIBits()缩放打印DIB
	HDC hdcPrinter = WindowPrinter::GetPrinterDC();
	if (hdcPrinter == NULL)
		return;

	int pageWidth = ::GetDeviceCaps(hdcPrinter, HORZRES);
	int pageHeight = ::GetDeviceCaps(hdcPrinter, VERTRES);

	float scaleX = (float)pageWidth / (float)bmpWnd.bmWidth;
	float scaleY = (float)pageHeight / (float)bmpWnd.bmHeight;
	float scale = scaleX < scaleY ? scaleX : scaleY;

	int xDst, yDst, cxDst, cyDst;
	cxDst = (int)((float)bmpWnd.bmWidth * scale);
	cyDst = (int)((float)bmpWnd.bmHeight * scale);
	xDst = (int)((pageWidth - cxDst) / 2);
	yDst = (int)((pageHeight - cyDst) / 2);

	static DOCINFO di = { sizeof(DOCINFO), L"PRINTJOBNAME" };
	if (::StartDoc(hdcPrinter, &di) > 0)
	{
		if (::StartPage(hdcPrinter) > 0)
		{
			::StretchDIBits(hdcPrinter,
				xDst, yDst, cxDst, cyDst,
				0, 0, bmpWnd.bmWidth, bmpWnd.bmHeight,
				lpbitmap,
				(BITMAPINFO*)&bi,
				DIB_RGB_COLORS,
				SRCCOPY);
			::EndPage(hdcPrinter);
		}
		::EndDoc(hdcPrinter);
	}
	::DeleteDC(hdcPrinter);
	::free(lpbitmap);
}