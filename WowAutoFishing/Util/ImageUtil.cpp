#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "ImageUtil.h"
#include <list>
#include <cmath>

ImageUtil::ImageUtil()
{
}

ImageUtil::~ImageUtil()
{
}

/*
截取指定窗口的图像。
lpBits: 返回的图像像素数据（其长度>=w*h*4）
pbi: 图像信息。
*/
bool ImageUtil::GetWindowSnapshot(HWND hWnd, int x, int y, int w, int h, char *lpBits, BITMAPINFOHEADER *pbi)
{
	bool bSuccess = false;

	// 通过内存DC复制客户区到DDB位图  
	HDC hdcWnd = ::GetDC(hWnd);
	HDC hdcMem = NULL;
	HBITMAP hbmWnd = NULL;

	do
	{
		if (w == 0 || h == 0)
		{
			RECT rect;
			if (!::GetClientRect(hWnd, &rect))
			{
				printf("GetClientRect has failed.\n");
				break;
			}
			w = rect.right - rect.left;
			h = rect.bottom - rect.top;
		}

		hdcMem = ::CreateCompatibleDC(hdcWnd);
		if (!hdcMem)
		{
			printf("CreateCompatibleDC failed.\n");
			break;
		}

		hbmWnd = ::CreateCompatibleBitmap(hdcWnd, w, h);
		if (!hbmWnd)
		{
			printf("CreateCompatibleBitmap failed.\n");
			break;
		}

		::SelectObject(hdcMem, hbmWnd);

		if (!::BitBlt(hdcMem, 0, 0, w, h, hdcWnd, x, y, SRCCOPY))
		{
			printf("BitBlt has failed.\n");
			break;
		}

		BITMAP bmpWnd;
		::GetObject(hbmWnd, sizeof(BITMAP), &bmpWnd);

		// 信息
		BITMAPINFOHEADER bi = { 0 };
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

		//DWORD dwBmpSize = ((bmpWnd.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpWnd.bmHeight; // 每一行像素位32对齐

		// Gets the "bits" from the bitmap and copies them into a buffer 
		// which is pointed to by lpbitmap.
		if (!GetDIBits(hdcMem, hbmWnd, 0, (WORD)bi.biHeight, lpBits, (BITMAPINFO*)&bi, DIB_RGB_COLORS))
		{
			printf("GetDIBits has failed.\n");
			break;
		}

		if (pbi != NULL)
		{
			::memcpy_s(pbi, sizeof(BITMAPINFOHEADER), &bi, sizeof(BITMAPINFOHEADER));
		}

		//CreateBMPFile(L"capture.bmp", (BITMAPINFO*)&bi, lpBits);

		bSuccess = true;
	} while (false);

	::DeleteObject(hbmWnd);
	::DeleteObject(hdcMem);
	::ReleaseDC(hWnd, hdcWnd);

	return bSuccess;
}

/*
创建位图文件。
*/
bool ImageUtil::CreateBMPFile(LPTSTR pszFile, PBITMAPINFO pbi, char *lpBits)
{
	bool bSuccess = false;
	HANDLE hf;                  // file handle  
	BITMAPFILEHEADER hdr;       // bitmap file-header  
	PBITMAPINFOHEADER pbih;     // bitmap info-header  
	DWORD dwTotal;              // total count of bytes  
	DWORD dwTmp;

	do
	{
		pbih = (PBITMAPINFOHEADER)pbi;

		// Create the .BMP file.  
		hf = CreateFile(pszFile, GENERIC_READ | GENERIC_WRITE, (DWORD)0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
		if (hf == INVALID_HANDLE_VALUE)
		{
			printf("CreateFile failed.\n");
			break;
		}

		// init file-header
		{
			hdr.bfType = 0x4d42; // 0x42 = "B" 0x4d = "M"  
			// Compute the size of the entire file.  
			hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER)
				+ pbih->biSize
				+ pbih->biClrUsed * sizeof(RGBQUAD)
				+ pbih->biSizeImage);
			hdr.bfReserved1 = 0;
			hdr.bfReserved2 = 0;
			// Compute the offset to the array of color indices.  
			hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER)
				+ pbih->biSize
				+ pbih->biClrUsed * sizeof(RGBQUAD);
		}
		// Copy the BITMAPFILEHEADER into the .BMP file.  
		if (!WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER), (LPDWORD)&dwTmp, NULL))
		{
			printf("WriteFile failed.\n");
			break;
		}
		// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
		dwTotal = sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof(RGBQUAD);
		if (!WriteFile(hf, (LPVOID)pbih, dwTotal, (LPDWORD)&dwTmp, (NULL)))
		{
			printf("WriteFile failed.\n");
			break;
		}
		// Copy the array of color indices into the .BMP file.  
		dwTotal = pbih->biSizeImage;
		if (!WriteFile(hf, (LPVOID)lpBits, dwTotal, (LPDWORD)&dwTmp, NULL))
		{
			printf("WriteFile failed. [%d]\n", ::GetLastError());
			break;
		}

		bSuccess = true;
	} while (false);

	// Close the .BMP file.  
	if (hf)
	{
		CloseHandle(hf);
	}

	return bSuccess;
}

/*
加载位图文件。
*/
HBITMAP ImageUtil::LoadImage2(LPTSTR pszFile, BITMAP *pBitmap)
{
	HBITMAP hBitmap = (HBITMAP)::LoadImage(NULL, pszFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if (hBitmap)
	{
		if (!::GetObject(hBitmap, sizeof(BITMAP), pBitmap))
		{
			printf("GetObject has failed.\n");
		}
	}
	return hBitmap;
}

/*
将指定的位图转换为灰度图。
*/
bool ImageUtil::TransToGray(BITMAP bitmap, LPTSTR pszFile)
{
	int w = bitmap.bmWidth;
	int h = bitmap.bmHeight;
	unsigned char *lpBits = (unsigned char *)bitmap.bmBits;
	unsigned char *buff = (unsigned char *)malloc(bitmap.bmWidth * bitmap.bmHeight * 4);

	for (int i = 0; i < h; ++i)
	{
		int basei = i * w * 4;
		for (int j = 0; j < w; ++j)
		{
			int index = basei + j * 4;
			unsigned char a = lpBits[index + 3];
			unsigned char r = lpBits[index + 2];
			unsigned char g = lpBits[index + 1];
			unsigned char b = lpBits[index + 0];
			unsigned char gray = (r * 38 + g * 75 + b * 15) >> 7;

			buff[index] = buff[index + 1] = buff[index + 2] = gray;
			buff[index + 3] = a;
			if (gray <= 17)
			{
				printf("%d,%d\n", j, i);
			}
		}
	}

	BITMAPINFOHEADER bi = { 0 }; // 信息头  
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bitmap.bmWidth;
	bi.biHeight = bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32; // 按照每个像素用32bits表示转换  
	bi.biCompression = BI_RGB;
	bi.biSizeImage = bi.biWidth * bi.biHeight * 4;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	bool ret = CreateBMPFile(pszFile, (BITMAPINFO*)&bi, (char*)buff);

	::free(buff);

	return ret;
}

/*
寻找图像中的指定颜色的像素点。
color: 目标颜色。
range: 阈值范围。
*/
void ImageUtil::FindColor(char *lpBits, int w, int h, int color, int range, std::list<POINT> &points)
{
	char r = RGB_R(color);
	char g = RGB_G(color);
	char b = RGB_B(color);
	char tr = RGB_R(range);
	char tg = RGB_G(range);
	char tb = RGB_B(range);

	POINT p;
	for (int i = 0; i < h; ++i)
	{
		int basei = i * w * 4;
		for (int j = 0; j < w; ++j)
		{
			int index = basei + j * 4;
			char dr = lpBits[index + 2] - r;
			char dg = lpBits[index + 1] - g;
			char db = lpBits[index + 0] - b;
			if (dr < tr && dr > -tr && dg < tg && dg >-tg && db < tb && db >-tb)
			{
				p.x = j;
				p.y = i;
				points.push_back(p);
				//printf("%d,%d\n", p.x, p.y);
			}
		}
	}
}

/*
寻找图像中的指定灰度值的像素点。
gray: 目标灰度值。
range: 阈值范围。
*/
void ImageUtil::FindGray(char *lpBits, int w, int h, int gray, int range, std::list<POINT> &points)
{
	POINT p;
	for (int i = 0; i < h; ++i)
	{
		int basei = i * w * 4;
		for (int j = 0; j < w; ++j)
		{
			int index = basei + j * 4;

			char r = lpBits[index + 2];
			char g = lpBits[index + 1];
			char b = lpBits[index + 0];
			char gg = (r * 38 + g * 75 + b * 15) >> 7;

			int diff = gg - gray;
			if (diff <= range && diff >= -range)
			{
				p.x = j;
				p.y = i;
				points.push_back(p);
				//printf("%d,%d\n", p.x, p.y);
			}
		}
	}
}

/*
将指定的坐标按照radius半径分组，并返回最大分组的中心点。
*/
bool ImageUtil::SelectBestPoint(std::list<POINT> points, int radius, POINT &p)
{
	radius = radius * radius;

	struct PointGroup
	{
		POINT center;
		std::list<POINT> points;
		
		bool operator < (PointGroup& b) {
			return points.size() > b.points.size(); // sort size by desc.
		}
	};

	// 按照radius对坐标进行分组
	std::list<PointGroup> groups;
	for (std::list<POINT>::iterator it = points.begin(); it != points.end(); ++it)
	{
		bool find = false;
		for (std::list<PointGroup>::iterator it2 = groups.begin(); it2 != groups.end(); ++it2)
		{
			int dx = it2->center.x - it->x;
			int dy = it2->center.y - it->y;
			int s = dx * dx + dy * dy;
			if (s <= radius)
			{
				it2->center.x = (it2->center.x + it->x)/2;
				it2->center.y = (it2->center.y + it->y)/2;
				it2->points.push_back(*it);
				find = true;
			}
		}
		if (!find)
		{
			PointGroup group;
			group.center = *it;
			group.points.push_back(*it);
			groups.push_back(group);
		}
	}
	groups.sort();

	if (groups.size() > 0)
	{
		PointGroup group = *(groups.begin()); // 选择元素最多的分组
		// 选择离中心最近的点
		int min = INT_MAX;
		std::list<POINT>::iterator minIt = group.points.end();
		for (std::list<POINT>::iterator it = group.points.begin(); it != group.points.end(); ++it)
		{
			int dx = group.center.x - it->x;
			int dy = group.center.y - it->y;
			int s = dx * dx + dy * dy;
			if (s < min)
			{
				min = s;
				minIt = it;
			}
		}
		if (minIt != group.points.end())
		{
			p = *minIt;
			return true;
		}
	}
	return false;
}

