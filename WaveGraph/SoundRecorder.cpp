#include "stdafx.h"
#include "SoundRecorder.h"
#include <process.h>

UINT __stdcall CaptureTheadProc(LPVOID param);

SoundRecorder::SoundRecorder(void)
	: m_bDone(false), m_hThreadCapture(NULL)
{
}


SoundRecorder::~SoundRecorder(void)
{
	Clear();
}


HRESULT SoundRecorder::OnCaptureData(BYTE *pData, UINT32 nDataLen, BOOL *bDone)
{
	AudioData data;
	data.nDataLen = nDataLen;
	data.pData = new BYTE[nDataLen];

	if (pData == NULL)
		::memset(data.pData, 0, nDataLen);
	else
		::memcpy(data.pData, pData, nDataLen);

	m_dataList.push_back(data);
	m_dataCount += nDataLen;

	*bDone = m_bDone;

	return S_OK;
}

HRESULT SoundRecorder::SetFormat(WAVEFORMATEX *pwfx)
{
	m_bytesPerSample = pwfx->wBitsPerSample >> 3;
	m_maxValue = (1L << (m_pwfx->wBitsPerSample - 1)) - 1;
	m_midValue = m_maxValue >> 1;

	return S_OK;
}

bool SoundRecorder::LoopDone()
{
	return m_bDone;
}

void SoundRecorder::SetDone(bool bDone)
{
	m_bDone = bDone;
}

bool SoundRecorder::StartRecord()
{
	Clear();

	HRESULT hr = StartCapture();
	if (FAILED(hr))
		return false;

	m_bDone = false;

	m_hThreadCapture = (HANDLE)::_beginthreadex(NULL, 0, &CaptureTheadProc, this, 0, NULL);
	if (m_hThreadCapture == NULL)
		return false;

	//HANDLE arWaits[1] = { m_hThreadCapture };
	//DWORD dwWaitResult = WaitForMultipleObjects(sizeof(arWaits) / sizeof(arWaits[0]), arWaits, FALSE, INFINITE);
	//if (dwWaitResult != WAIT_OBJECT_0)
	//{
	//	StopRecord();
	//	return false;
	//}

	return true;
}

void SoundRecorder::StopRecord()
{
	m_bDone = true;

	StopCapture();

	if (m_hThreadCapture != NULL)
	{
		CloseHandle(m_hThreadCapture);
		m_hThreadCapture = NULL;
	}
}

void SoundRecorder::Clear()
{
	m_dataCount = 0;
	for (std::list<AudioData>::iterator it = m_dataList.begin(); it != m_dataList.end(); ++it)
	{
		delete it->pData;
	}
	m_dataList.clear();
}

UINT __stdcall CaptureTheadProc(LPVOID param)
{
	CoInitialize(NULL);

	SoundRecorder *pRecorder = (SoundRecorder*)param;
	pRecorder->Capture();

	CoUninitialize();

	return S_OK;
}

bool SoundRecorder::Paint(HWND hwnd, HDC hdc)
{
	RECT rect;
	if (FAILED(::GetWindowRect(hwnd, &rect)))
		return false;

	int w = rect.right - rect.left;
	int h = 200;
	int m = (rect.bottom - rect.top) / 2;
	int h2 = h / 2;

	int x = 10;
	int y = 0;

	float noise = 0.1f; // ‘Î“Ù„–÷µ
	int scaleY = 1; // Y÷·Àı∑≈

	::MoveToEx(hdc, 0, m, NULL);
	::LineTo(hdc, w, m);

	int count = m_dataCount / m_bytesPerSample;
	int step = (count / w);
	if (step < 1)
		step = 1;

	float value;
	UINT range = step;
	UINT num = 0;

	Reset();
	::MoveToEx(hdc, x, m, NULL);
	while ((num = GetNext(range, &value)) > 0)
	{
		int y = (int)(value * h * scaleY / 2);
		::LineTo(hdc, x, m - y);
		++x;
	}
}

void SoundRecorder::Reset()
{
	m_dataListIter = m_dataList.begin();
	m_dataIndex = 0;
}

UINT SoundRecorder::GetNext(UINT range, float *pValue)
{
	UINT count = 0;
	int value = 0, max = INT_MIN;
	while (count < range && m_dataListIter != m_dataList.end())
	{
		BYTE *pData = m_dataListIter->pData;
		UINT maxIndex = m_dataListIter->nDataLen / m_bytesPerSample;

		for (; m_dataIndex < maxIndex; m_dataIndex += m_pwfx->nChannels)
		{
			if (count >= range)
				break;

			switch (m_pwfx->wBitsPerSample)
			{
			case 8:
				value = *(pData + m_dataIndex);
				break;
			case 16:
				value = *((INT16*)pData + m_dataIndex);
				break;
			case 32:
				value = *((INT32*)pData + m_dataIndex);
				break;
			default:
				value = 0;
				break;
			}

			if (value < 0)
				value = 0;
			if (value > max)
				max = value;

			++count;
		}

		if (count < range)
		{
			++m_dataListIter;
			m_dataIndex = 0;
		}
	}

	if (count > 0)
	{
		*pValue = (float)max / m_maxValue;
	}

	return count;
}
