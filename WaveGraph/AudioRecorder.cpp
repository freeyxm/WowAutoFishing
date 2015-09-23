#include "stdafx.h"
#include "AudioRecorder.h"
#include <process.h>

UINT __stdcall CaptureTheadProc(LPVOID param);

AudioRecorder::AudioRecorder(void)
	: m_bDone(false), m_hThreadCapture(NULL), m_scaleY(1.0f)
{
	InitializeCriticalSection(&m_dataSection);
}

AudioRecorder::~AudioRecorder(void)
{
	Clear();
}

HRESULT AudioRecorder::SetFormat(WAVEFORMATEX *pwfx)
{
	m_nBytesPerSample = pwfx->wBitsPerSample >> 3;
	m_maxValue = (1L << (pwfx->wBitsPerSample - 1)) - 1;
	m_midValue = m_maxValue >> 1;

	m_dataMaxBytes = (UINT)(pwfx->nAvgBytesPerSec * 1.0f);

	m_waveFormatFloat = false;
	if (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
	{
		m_waveFormatFloat = true;
		printf("Format: WAVE_FORMAT_IEEE_FLOAT \n");
	}
	else if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		WAVEFORMATEXTENSIBLE *p = (WAVEFORMATEXTENSIBLE*)pwfx;
		if (p->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
		{
			printf("Format: KSDATAFORMAT_SUBTYPE_PCM \n");
		}
		else if (p->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
		{
			m_waveFormatFloat = true;
			printf("Format: KSDATAFORMAT_SUBTYPE_IEEE_FLOAT \n");
		}
	}

	return S_OK;
}

HRESULT AudioRecorder::OnCaptureData(BYTE *pData, UINT32 nDataLen, BOOL *bDone)
{
	::EnterCriticalSection(&m_dataSection);

	bool bSuccess = false;
	do 
	{
		if(m_dataStorage.GetTotalBytes() + nDataLen > m_dataMaxBytes)
		{
			if(!m_dataStorage.ReplaceFront(pData, nDataLen))
			{
				break;
			}
		}
		else
		{
			if(!m_dataStorage.PushBack(pData, nDataLen))
			{
				break;
			}
		}
		bSuccess = true;
	} while (false);

	//printf("OnCaptureData: %d, %d\n", nDataLen, m_dataList.size());

	::LeaveCriticalSection(&m_dataSection);

	*bDone = m_bDone;

	return bSuccess ? S_OK : E_FAIL;
}

bool AudioRecorder::LoopDone()
{
	return m_bDone;
}

void AudioRecorder::SetDone(bool bDone)
{
	m_bDone = bDone;
}

bool AudioRecorder::StartRecord()
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

void AudioRecorder::StopRecord()
{
	m_bDone = true;

	StopCapture();

	if (m_hThreadCapture != NULL)
	{
		CloseHandle(m_hThreadCapture);
		m_hThreadCapture = NULL;
	}
}

void AudioRecorder::Clear()
{
	m_dataStorage.Clear();
}

UINT __stdcall CaptureTheadProc(LPVOID param)
{
	CoInitialize(NULL);

	AudioRecorder *pRecorder = (AudioRecorder*)param;
	pRecorder->Capture();

	CoUninitialize();

	return S_OK;
}

void AudioRecorder::Paint(HWND hwnd, HDC hdc)
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

	::MoveToEx(hdc, x, m, NULL);
	::LineTo(hdc, x + w, m);

	::MoveToEx(hdc, x, m - h2, NULL);
	::LineTo(hdc, x + w, m - h2);

	::MoveToEx(hdc, x, m + h2, NULL);
	::LineTo(hdc, x + w, m + h2);

	::EnterCriticalSection(&m_dataSection);
	//printf("Paint begin ...\n");

	int step = (m_dataMaxBytes / w);
	int minStep = m_nBytesPerSample * m_pwfx->nChannels;
	if(step < minStep)
		step = minStep;

	float min, max;
	ResetIter();
	while (GetNext(step, &min, &max) > 0)
	{
		y = (int)(min * m_scaleY * h2);
		::MoveToEx(hdc, x, m, NULL);
		::LineTo(hdc, x, m + y);

		y = (int)(max * m_scaleY * h2);
		::MoveToEx(hdc, x, m, NULL);
		::LineTo(hdc, x, m + y);

		++x;
	}

	//printf("Paint end.\n");
	::LeaveCriticalSection(&m_dataSection);
}

void AudioRecorder::ResetIter()
{
	m_dataIter = m_dataStorage.begin();
	m_dataIndex = 0;
}

UINT AudioRecorder::GetNext(UINT range, float *pMin, float *pMax)
{
	UINT count = 0;
	range /= (m_nBytesPerSample * m_pwfx->nChannels);

	float value = 0, min = 0, max = 0;
	while (count < range && m_dataIter != m_dataStorage.end())
	{
		BYTE *pData = (*m_dataIter)->pData;
		UINT maxIndex = (*m_dataIter)->nDataLen / m_nBytesPerSample;

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
				if(m_waveFormatFloat)
					value = *((float*)pData + m_dataIndex);
				else
					value = *((int*)pData + m_dataIndex);
				break;
			default:
				value = 0;
				break;
			}

			if(value < min)
				min = value;
			if(value > max)
				max = value;

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
		*pMin = min;
		*pMax = max;
	}

	return count;
}

void AudioRecorder::AddScale(float scale)
{
	m_scaleY += scale;

	if (m_scaleY < 0)
		m_scaleY = 0;
}

float AudioRecorder::GetScale()
{
	return m_scaleY;
}
