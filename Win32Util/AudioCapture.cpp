#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "AudioCapture.h"
#include "AudioUtil.h"
#include <cstdio>

AudioCapture::AudioCapture(bool bLoopback, bool bDefaultDevice)
    : m_pDevice(NULL)
    , m_pAudioClient(NULL)
    , m_pCaptureClient(NULL)
    , m_pwfx(NULL)
    , m_bInited(false)
    , m_bDone(true)
    , m_nBytesPerSample(0)
    , m_nBytesPerFrame(0)
    , m_midValue(0)
    , m_bLoopback(bLoopback)
    , m_bDefaultDevice(bDefaultDevice)
    , m_hnsActualDuration(0)
{
}

AudioCapture::~AudioCapture()
{
	Release();
}

//-----------------------------------------------------------
// Record an audio stream from the default audio capture
// device. The RecordAudioStream function allocates a shared
// buffer big enough to hold one second of PCM audio data.
// The function uses this buffer to stream data from the
// capture device. The main loop runs every 1/2 second.
//-----------------------------------------------------------

// REFERENCE_TIME time units per second and per millisecond
#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

bool AudioCapture::Init()
{
	if (m_bInited)
		return true;

	HRESULT hr = S_FALSE;
	IMMDeviceEnumerator *pEnumerator = NULL;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	UINT32 bufferFrameCount = 0;

	do
	{
		const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
		const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
		const IID IID_IAudioClient = __uuidof(IAudioClient);
		const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

		//hr = ::CoInitialize(NULL);
		//BREAK_ON_ERROR(hr);

		hr = ::CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
		BREAK_ON_ERROR(hr);

		if (!SelectDevice(pEnumerator))
			break;

		hr = m_pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&m_pAudioClient);
		BREAK_ON_ERROR(hr);

		hr = m_pAudioClient->GetMixFormat(&m_pwfx);
		BREAK_ON_ERROR(hr);

		if (m_bLoopback)
			hr = m_pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, m_pwfx, 0);
		else
			hr = m_pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, hnsRequestedDuration, 0, m_pwfx, NULL);
		BREAK_ON_ERROR(hr);

		// Get the size of the allocated buffer.
		hr = m_pAudioClient->GetBufferSize(&bufferFrameCount);
		BREAK_ON_ERROR(hr);

		hr = m_pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&m_pCaptureClient);
		BREAK_ON_ERROR(hr);

		// Notify the audio sink which format to use.
		hr = this->SetFormat(m_pwfx);
		BREAK_ON_ERROR(hr);

		// Calculate the actual duration of the allocated buffer.
		m_hnsActualDuration = (REFERENCE_TIME)((double)REFTIMES_PER_SEC * bufferFrameCount / m_pwfx->nSamplesPerSec);

		hr = S_OK;

	} while (false);

	SAFE_RELEASE(pEnumerator);

	if (FAILED(hr))
	{
		Release();
		::printf("Init failed, error code: 0x%x\n", hr);
		return false;
	}
	else
	{
		m_bInited = true;
		return true;
	}
}

void AudioCapture::Release()
{
	if (m_pwfx != NULL)
	{
		CoTaskMemFree(m_pwfx);
		m_pwfx = NULL;
	}
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pAudioClient);
	SAFE_RELEASE(m_pCaptureClient);
	m_bInited = false;
	m_bDone = true;
}

bool AudioCapture::SelectDevice(IMMDeviceEnumerator *pEnumerator)
{
	return AudioUtil::SelectDevice(pEnumerator, m_bLoopback ? eRender : eCapture, m_bDefaultDevice, &m_pDevice);
}

bool AudioCapture::StartCapture()
{
	if (m_bInited && m_pAudioClient)
	{
		HRESULT hr = m_pAudioClient->Start();  // Start recording.
		return SUCCEEDED(hr);
	}
	return false;
}

bool AudioCapture::StopCapture()
{
	if (m_bInited && m_pAudioClient)
	{
		HRESULT hr = m_pAudioClient->Stop();  // Stop recording.
		return SUCCEEDED(hr);
	}
	return false;
}

HRESULT AudioCapture::Capture()
{
	if (!m_bInited)
	{
		return E_FAIL;
	}

	HRESULT hr = S_OK;
	UINT32 numFramesAvailable;
	UINT32 packetLength = 0;
	BYTE *pData;
	DWORD flags;

	// Each loop fills about half of the shared buffer.
	while (!IsDone())
	{
		// Sleep for half the buffer duration.
		Sleep((DWORD)(m_hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

		hr = m_pCaptureClient->GetNextPacketSize(&packetLength);
		BREAK_ON_ERROR(hr);

		while (packetLength != 0 && !IsDone())
		{
			// Get the available data in the shared buffer.
			hr = m_pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
			BREAK_ON_ERROR(hr);

			if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
			{
				pData = NULL;  // Tell CopyData to write silence.
			}

			if (numFramesAvailable != 0)
			{
				// Copy the available capture data to the audio sink.
				hr = this->OnCaptureData(pData, numFramesAvailable);
				BREAK_ON_ERROR(hr);
			}

			hr = m_pCaptureClient->ReleaseBuffer(numFramesAvailable);
			BREAK_ON_ERROR(hr);

			hr = m_pCaptureClient->GetNextPacketSize(&packetLength);
			BREAK_ON_ERROR(hr);
		}
	}

	return hr;
}

HRESULT AudioCapture::OnCaptureData(BYTE *pData, UINT32 nFrameCount)
{
	//printf("OnCaptureData: %d\n", nFrameCount);
	return S_OK;
}

const WAVEFORMATEX* AudioCapture::GetFormat() const
{
	return m_pwfx;
}

HRESULT AudioCapture::SetFormat(WAVEFORMATEX *pwfx)
{
	//printf("Format:\n");
	//printf("  wFormatTag: %d\n", pwfx->wFormatTag);
	//printf("  nChannels: %d\n", pwfx->nChannels);
	//printf("  nSamplesPerSec: %d\n", pwfx->nSamplesPerSec);
	//printf("  nAvgBytesPerSec: %d\n", pwfx->nAvgBytesPerSec);
	//printf("  nBlockAlign: %d\n", pwfx->nBlockAlign);
	//printf("  wBitsPerSample: %d\n", pwfx->wBitsPerSample);
	//printf("  cbSize: %d\n", pwfx->cbSize);

	m_nBytesPerSample = pwfx->wBitsPerSample / 8;
	m_nBytesPerFrame = m_nBytesPerSample * m_pwfx->nChannels;
	m_midValue = AudioUtil::GetMidValue(m_pwfx);

	return S_OK;
}

bool AudioCapture::IsDone() const
{
	return m_bDone;
}

void AudioCapture::SetDone(bool bDone)
{
	m_bDone = bDone;
}

float AudioCapture::ParseValue(BYTE *pData, size_t index) const
{
	return AudioUtil::ParseValue(m_pwfx, pData, index, m_midValue);
}
