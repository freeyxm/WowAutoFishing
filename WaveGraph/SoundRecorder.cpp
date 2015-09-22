#include "stdafx.h"
#include "SoundRecorder.h"
#include <process.h>

UINT __stdcall CaptureTheadProc(LPVOID param);

SoundRecorder::SoundRecorder(void)
	: m_bDone(false), m_hThreadCapture(NULL), m_dataCurBytes(0)
{
	InitializeCriticalSection(&m_dataSection);
}


SoundRecorder::~SoundRecorder(void)
{
	Clear();
}


HRESULT SoundRecorder::OnCaptureData(BYTE *pData, UINT32 nDataLen, BOOL *bDone)
{
	AudioData data;

	::EnterCriticalSection(&m_dataSection);

	if(m_dataCurBytes + nDataLen > m_dataMaxBytes)
	{
		data = m_dataList.front();
		m_dataCurBytes -= data.nDataLen;
		m_dataList.pop_front();

		data.pData = (BYTE*)::realloc(data.pData, nDataLen);
		data.nDataLen = nDataLen;
	}
	else
	{
		data.nDataLen = nDataLen;
		data.pData = (BYTE*)::malloc(nDataLen);
	}

	if (pData == NULL)
		::memset(data.pData, 0, nDataLen);
	else
		::memcpy(data.pData, pData, nDataLen);

	m_dataList.push_back(data);
	m_dataCurBytes += nDataLen;

	printf("OnCaptureData: %d, %d\n", nDataLen, m_dataList.size());

	::LeaveCriticalSection(&m_dataSection);

	*bDone = m_bDone;

	return S_OK;
}

HRESULT SoundRecorder::SetFormat(WAVEFORMATEX *pwfx)
{
	m_nBytesPerSample = pwfx->wBitsPerSample >> 3;
	m_maxValue = (1L << (pwfx->wBitsPerSample - 1)) - 1;
	m_midValue = m_maxValue >> 1;

	m_dataMaxBytes = (UINT)(pwfx->nAvgBytesPerSec * 1.0f);

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
	m_dataCurBytes = 0;
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

void SoundRecorder::Paint(HWND hwnd, HDC hdc)
{
	RECT rect;
	if (FAILED(::GetWindowRect(hwnd, &rect)))
		return;

	int x = 8;
	int y = 0;

	int w = rect.right - rect.left - x * 4;
	int h = 200;
	int m = (rect.bottom - rect.top) / 2;
	int h2 = h / 2;

	float noise = 0.1f; // ÔëÒôãÐÖµ
	int scaleY = 1; // YÖáËõ·Å

	::MoveToEx(hdc, x, m, NULL);
	::LineTo(hdc, x + w, m);

	::MoveToEx(hdc, x, m - h2, NULL);
	::LineTo(hdc, x + w, m - h2);

	::MoveToEx(hdc, x, m + h2, NULL);
	::LineTo(hdc, x + w, m + h2);

	::EnterCriticalSection(&m_dataSection);
	printf("Paint begin ...\n");

	int step = (m_dataMaxBytes / w);
	int minStep = m_nBytesPerSample * m_pwfx->nChannels;
	if(step < minStep)
		step = minStep;

	int value;
	
	::MoveToEx(hdc, x, m, NULL);

	ResetIter();
	while (GetNext(step, &value) > 0)
	{
		int y = (int)((float)(value) / m_maxValue * h * scaleY / 2);
		::LineTo(hdc, x, m - y);
		++x;
	}

	printf("Paint end.\n");
	::LeaveCriticalSection(&m_dataSection);
}

void SoundRecorder::ResetIter()
{
	m_dataIter = m_dataList.begin();
	m_dataIndex = 0;
}

UINT SoundRecorder::GetNext(UINT range, int *pValue)
{
	UINT count = 0;
	range /= (m_nBytesPerSample * m_pwfx->nChannels);

	float avg = 0.0f;
	int value = 0;
	while (count < range && m_dataIter != m_dataList.end())
	{
		BYTE *pData = m_dataIter->pData;
		UINT maxIndex = m_dataIter->nDataLen / m_nBytesPerSample;

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
			{
				value = -value;
			}
			avg += value;

			++count;
		}

		if (count < range)
		{
			++m_dataIter;
			m_dataIndex = 0;
		}
	}

	if (count > 0)
	{
		*pValue = (int)(avg / count);
	}

	return count;
}
