#include "stdafx.h"
#include "SoundListener.h"
#include <cstdio>


SoundListener::SoundListener(void)
	: m_pBuf(NULL), m_nBufLen(0)
{
}


SoundListener::~SoundListener(void)
{
	if (m_pBuf)
	{
		delete[] m_pBuf;
		m_pBuf = NULL;
	}
}

bool SoundListener::Init2()
{
	if (FAILED(SoundCapture::Init()))
		return false;

	//BUF_SIZE = (int)(m_pwfx->nSamplesPerSec * 0.1f) & ~((m_pwfx->wBitsPerSample >> 3) - 1); // single channel
	BUF_SIZE = 480;

	m_pBuf = new float[BUF_SIZE];
	if (!m_pBuf)
		return false;

	return true;
}

HRESULT SoundListener::OnCaptureData(BYTE *pData, UINT32 nDataLen, BOOL *bDone)
{
	UINT maxLen = (BUF_SIZE - m_nBufLen) * (m_pwfx->wBitsPerSample >> 3) * 2;
	if (maxLen > nDataLen)
		maxLen = nDataLen;

	UINT32 count = maxLen / (m_pwfx->wBitsPerSample >> 3);
	int maxValue = (1L << (m_pwfx->wBitsPerSample - 1)) - 1;
	int midValue = maxValue >> 1;

	if (count > BUF_SIZE)
		count = BUF_SIZE;

	if (pData == NULL)
	{
		// 只处理1个声道的数据
		for (UINT i = 0; i < count; i += m_pwfx->nChannels)
		{
			m_pBuf[m_nBufLen++] = 0;
		}
	}
	else
	{
		int value;
		// 只处理1个声道的数据
		for (UINT i = 0; i < count; i += m_pwfx->nChannels)
		{
			switch (m_pwfx->wBitsPerSample)
			{
			case 8:
				value = *(pData + i);
				break;
			case 16:
				value = *((INT16*)pData + i);
				break;
			case 32:
				value = *((int*)pData + i);
				break;
			default:
				value = 0;
				break;
			}
			if (value >= 0)
			{
				m_pBuf[m_nBufLen++] = (float)(value > midValue ? value - midValue : midValue - value) / maxValue;
			}
			else
			{
				value = -value;
				m_pBuf[m_nBufLen++] = -(float)(value > midValue ? value - midValue : midValue - value) / maxValue;
			}

			//m_pBuf[m_nBufLen++] = (float)((value >= midValue ? value - midValue : midValue - value)) / maxValue;

			//if(value < 0)
			//	value = -value;
			//m_pBuf[m_nBufLen++] = (float)(value) / maxValue;

		}
	}

	*bDone = maxLen < nDataLen;

	return S_OK;
}

bool SoundListener::LoopWait()
{
	return true;
}

static int g_paintCount = 0;
bool SoundListener::Paint(HWND hwnd, HDC hdc)
{
	if (!m_pBuf)
		return false;

	if (m_nBufLen == 0)
		return false;

	RECT rect;
	if (FAILED(::GetWindowRect(hwnd, &rect)))
		return false;

	printf("paint: %d, %d\n", g_paintCount++, m_nBufLen);

	int w = rect.right - rect.left;
	int h = 200;
	int m = (rect.bottom - rect.top) / 2;

	int h2 = h / 2;

	int x = 10;
	int y = 0;
	int max = INT_MIN, min = INT_MAX;
	float noise = 0.1f; // 噪音阈值
	int scaleY = 10; // Y轴缩放

	::Rectangle(hdc, x - 1, m - h2 - 1, x + m_nBufLen + 1, m + h2 + 1);

	::MoveToEx(hdc, x, m, NULL);
	for (int i = 0; i < m_nBufLen; ++i)
	{
		if (m_pBuf[i] >= noise || m_pBuf[i] <= -noise)
			m_pBuf[i] = 0.0f;

		y = (int)(m_pBuf[i] * h * scaleY / 2);
		::LineTo(hdc, x, m + y);
		x += 1;

		//if(y > max)
		//	max = y;
		//else if(y < min)
		//	min = y;
	}

	//printf("paint: min %d, max %d\n", min, max);

	::MoveToEx(hdc, 0, m, NULL);
	::LineTo(hdc, w, m);

	m_nBufLen = 0;

	return true;
}

bool SoundListener::Record2()
{
	Capture();
	return m_nBufLen > 0;
}
