#pragma once
#include <Windows.h>

/********************************************************************************
WindowPrinter 打印窗口类
功能描述：
提供截屏窗口并通过默认打印机，自动进行居中缩放打印

使用说明：
样例代码如下。
HWND hwnd = this->GetSafeWnd();
WindowPrinter::PrintWindowClientArea(hwnd);
********************************************************************************/
class WindowPrinter
{
public:
	WindowPrinter();
	~WindowPrinter();
public:
	/*
	功能：获取当前默认打印机的DC
	返回：成功返回打印机的DC，失败返回NULL
	*/
	static HDC GetPrinterDC();

	/*
	功能：打印窗口客户区内容到打印机，自动缩放居中打印
	参数: hWnd-被打印窗口的句柄
	*/
	static void PrintWindowClientArea(HWND hwnd);
};

