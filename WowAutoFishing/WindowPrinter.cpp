#include "stdafx.h"
#include "WindowPrinter.h"
#include <Winspool.h>

WindowPrinter::WindowPrinter()
{
}

WindowPrinter::~WindowPrinter()
{
}

/*
���ܣ���ȡ��ǰĬ�ϴ�ӡ����DC
���أ��ɹ����ش�ӡ����DC��ʧ�ܷ���NULL
*/
HDC WindowPrinter::GetPrinterDC()
{
	DWORD dwNeeded, dwReturned;
	HDC hdc;
	::EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, NULL, 0, &dwNeeded, &dwReturned);
	PRINTER_INFO_4* pinfo4 = (PRINTER_INFO_4*)malloc(dwNeeded);
	::EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, (BYTE*)pinfo4, dwNeeded, &dwNeeded, &dwReturned);
	hdc = ::CreateDC(NULL, pinfo4->pPrinterName, NULL, NULL);
	free(pinfo4);
	return hdc;
}

/*
���ܣ���ӡ���ڿͻ������ݵ���ӡ�����Զ����ž��д�ӡ
����: hWnd-����ӡ���ڵľ��
*/
void WindowPrinter::PrintWindowClientArea(HWND hWnd)
{
	if (hWnd == NULL) return;

	RECT rectClient;
	::GetClientRect(hWnd, &rectClient);
	int width = rectClient.right - rectClient.left;
	int height = rectClient.bottom - rectClient.top;

	// ͨ���ڴ�DC���ƿͻ�����DDBλͼ
	HDC hdcWnd = ::GetDC(hWnd);
	HBITMAP hbmWnd = ::CreateCompatibleBitmap(hdcWnd, width, height);
	HDC hdcMem = ::CreateCompatibleDC(hdcWnd);
	::SelectObject(hdcMem, hbmWnd);
	::BitBlt(hdcMem, 0, 0, width, height, hdcWnd, 0, 0, SRCCOPY);

	// �Ѵ���DDBתΪDIB
	BITMAP bmpWnd;
	::GetObject(hbmWnd, sizeof(BITMAP), &bmpWnd);
	BITMAPINFOHEADER bi; // ��Ϣͷ
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpWnd.bmWidth;
	bi.biHeight = bmpWnd.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32; // ����ÿ��������32bits��ʾת��
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((bmpWnd.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpWnd.bmHeight; // ÿһ������λ32����
	char *lpbitmap = (char*)malloc(dwBmpSize); // ����λָ��
	::GetDIBits(hdcMem, hbmWnd, 0, (UINT)bmpWnd.bmHeight,
		lpbitmap,
		(BITMAPINFO*)&bi,
		DIB_RGB_COLORS);

	::DeleteDC(hdcMem);
	::DeleteObject(hbmWnd);
	::ReleaseDC(hWnd, hdcWnd);

	// ��Ϊ�ļ�����ѡ��
	BITMAPFILEHEADER bmfHeader; // �ļ�ͷ
	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
	bmfHeader.bfSize = dwSizeofDIB;
	bmfHeader.bfType = 0x4D42;

	FILE* fp = NULL;
	::_wfopen_s(&fp, L"capture.bmp", L"w");
	::fwrite(&bmfHeader, sizeof(BITMAPFILEHEADER), 1, fp); // д���ļ�ͷ
	::fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, fp);        // д����Ϣͷ
	::fwrite(lpbitmap, dwBmpSize, 1, fp);                  // д������λ
	::fclose(fp);
	fp = NULL;

	// StretchDIBits()���Ŵ�ӡDIB
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