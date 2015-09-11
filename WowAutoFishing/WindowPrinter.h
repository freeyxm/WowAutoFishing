#pragma once
#include <Windows.h>

/********************************************************************************
WindowPrinter ��ӡ������
����������
�ṩ�������ڲ�ͨ��Ĭ�ϴ�ӡ�����Զ����о������Ŵ�ӡ

ʹ��˵����
�����������¡�
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
	���ܣ���ȡ��ǰĬ�ϴ�ӡ����DC
	���أ��ɹ����ش�ӡ����DC��ʧ�ܷ���NULL
	*/
	static HDC GetPrinterDC();

	/*
	���ܣ���ӡ���ڿͻ������ݵ���ӡ�����Զ����ž��д�ӡ
	����: hWnd-����ӡ���ڵľ��
	*/
	static void PrintWindowClientArea(HWND hwnd);
};

