#include "stdafx.h"
#include "DcBuffer.h"


DcBuffer::DcBuffer(HWND hwnd, int w, int h)
{
	m_hwnd = hwnd;

	m_rect.left = 0;
	m_rect.top = 0;
	m_rect.right = w;
	m_rect.bottom = h;

	Init();
}


DcBuffer::~DcBuffer()
{
}

void DcBuffer::Init()
{
	HDC hdc = ::GetDC(m_hwnd);
	m_hdcMem = ::CreateCompatibleDC(hdc);
	m_hBitMap = ::CreateCompatibleBitmap(hdc, m_rect.right, m_rect.bottom);
	m_hBrush = ::CreateSolidBrush(::GetBkColor(hdc));
	::SelectObject(m_hdcMem, m_hBitMap);
	::ReleaseDC(m_hwnd, hdc);
}

HDC DcBuffer::GetDC()
{
	return m_hdcMem;
}

void DcBuffer::Clear()
{
	::FillRect(m_hdcMem, &m_rect, m_hBrush);
}