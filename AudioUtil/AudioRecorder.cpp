#include "stdafx.h"
#include "AudioRecorder.h"
#include <process.h>

static UINT __stdcall CaptureTheadProc(LPVOID param);

AudioRecorder::AudioRecorder(bool bLoopback, bool bDefaultDevice)
    : AudioCapture(bLoopback, bDefaultDevice)
    , m_hThreadCapture(NULL)
    , m_dataMaxBytes(0)
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

	if (m_hThreadCapture != NULL)
		return false;

	if (FAILED(StartCapture()))
		return false;

	SetDone(false);

	m_hThreadCapture = (HANDLE)::_beginthreadex(NULL, 0, &CaptureTheadProc, this, 0, NULL);
	if (m_hThreadCapture == NULL)
		return false;

	return true;
}

void AudioRecorder::Stop()
{
	SetDone(true);

	if (m_hThreadCapture != NULL)
	{
		::WaitForSingleObject(m_hThreadCapture, INFINITE);
		::CloseHandle(m_hThreadCapture);
		m_hThreadCapture = NULL;
	}

	StopCapture();
}

void AudioRecorder::Clear()
{
	Lock();
	m_dataStorage.Reset();
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