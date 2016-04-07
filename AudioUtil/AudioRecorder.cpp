#include "stdafx.h"
#include "AudioRecorder.h"
#include <process.h>

static UINT __stdcall CaptureTheadProc(LPVOID param);

AudioRecorder::AudioRecorder(bool bLoopback, bool bDefaultDevice)
	: AudioCapture(bLoopback, bDefaultDevice)
	, m_hThreadCapture(NULL)
{
	InitializeCriticalSection(&m_dataSection);
}

AudioRecorder::~AudioRecorder(void)
{
	Clear();
	DeleteCriticalSection(&m_dataSection);
}

HRESULT AudioRecorder::SetFormat(WAVEFORMATEX *pwfx)
{
	AudioCapture::SetFormat(pwfx);

	m_dataMaxBytes = (UINT)(pwfx->nAvgBytesPerSec * 10.0f);

	return S_OK;
}

HRESULT AudioRecorder::OnCaptureData(BYTE *pData, UINT32 nFrameCount)
{
	Lock();

	bool bSuccess = false;
	do
	{
		UINT32 nDataLen = nFrameCount * m_nBytesPerFrame;
		if (m_dataStorage.GetTotalBytes() + nDataLen > m_dataMaxBytes)
		{
			if (!m_dataStorage.ReplaceFront(pData, nDataLen))
			{
				break;
			}
		}
		else
		{
			if (!m_dataStorage.PushBack(pData, nDataLen))
			{
				break;
			}
		}
		bSuccess = true;
	} while (false);

	//printf("OnCaptureData: %d, %d\n", nDataLen, m_dataList.size());

	Unlock();

	return bSuccess ? S_OK : E_FAIL;
}

bool AudioRecorder::Start()
{
	Clear();

	if (!StartCapture())
		return false;

	SetDone(false);

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

void AudioRecorder::Stop()
{
	SetDone(true);

	StopCapture();

	if (m_hThreadCapture != NULL)
	{
		CloseHandle(m_hThreadCapture);
		m_hThreadCapture = NULL;
	}
}

void AudioRecorder::Clear()
{
	Lock();
	m_dataStorage.Clear();
	Unlock();
}

static UINT __stdcall CaptureTheadProc(LPVOID param)
{
	AudioRecorder *pRecorder = (AudioRecorder*)param;
	return pRecorder->Capture();
}

const AudioFrameStorage* AudioRecorder::GetStorage() const
{
	return &m_dataStorage;
}

void AudioRecorder::Lock()
{
	::EnterCriticalSection(&m_dataSection);
}

void AudioRecorder::Unlock()
{
	::LeaveCriticalSection(&m_dataSection);
}