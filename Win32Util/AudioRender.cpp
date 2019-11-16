#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "AudioRender.h"
#include "AudioUtil.h"
#include <cstdio>

AudioRender::AudioRender(bool bDefaultDevice)
    : m_pDevice(NULL)
    , m_pAudioClient(NULL)
    , m_pRenderClient(NULL)
    , m_pwfx(NULL)
    , m_wfx()
    , m_bInited(false)
    , m_bDone(true)
    , m_nBytesPerSample(0)
    , m_nBytesPerFrame(0)
    , m_nBufferFrameCount(0)
    , m_bDefaultDevice(bDefaultDevice)
    , m_hnsActualDuration(0)
{
}

AudioRender::~AudioRender()
{
	ReleaseAudioClient();
}

//-----------------------------------------------------------
// Play an audio stream on the default audio rendering
// device. The PlayAudioStream function allocates a shared
// buffer big enough to hold one second of PCM audio data.
// The function uses this buffer to stream data to the
// rendering device. The inner loop runs every 1/2 second.
//-----------------------------------------------------------

// REFERENCE_TIME time units per second and per millisecond
#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

bool AudioRender::InitAudioClient()
{
	if (m_pAudioClient != NULL)
		return true;

	HRESULT hr = S_FALSE;
	IMMDeviceEnumerator *pEnumerator = NULL;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;

	do
	{
		const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
		const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
		const IID IID_IAudioClient = __uuidof(IAudioClient);

		hr = ::CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
		BREAK_ON_ERROR(hr);

		if (!SelectDevice(pEnumerator))
			break;

		hr = m_pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&m_pAudioClient);
		BREAK_ON_ERROR(hr);

		hr = S_OK;
	} while (false);

	SAFE_RELEASE(pEnumerator);

	if (FAILED(hr))
	{
		ReleaseAudioClient();
		::printf("InitAudioClient failed, error code: 0x%x\n", hr);
		return false;
	}
	else
	{
		return true;
	}
}

bool AudioRender::InitRenderClient(WAVEFORMATEX *pwfx)
{
	HRESULT hr = S_FALSE;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;

	do
	{
		if (m_pAudioClient == NULL)
			break;

		if (m_pRenderClient != NULL)
			ReleaseRenderClient();

		if (pwfx != NULL)
		{
			hr = m_pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, pwfx, &m_pwfx);
			if (hr == S_FALSE)
				pwfx = m_pwfx;
			else if (hr != S_OK)
				break;
		}
		else
		{
			hr = m_pAudioClient->GetMixFormat(&m_pwfx);
			BREAK_ON_ERROR(hr);
			pwfx = m_pwfx;
		}

		// Tell the audio source which format to use.
		if (!this->UseFormat(pwfx))
			break;

		hr = m_pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, hnsRequestedDuration, 0, pwfx, NULL);
		BREAK_ON_ERROR(hr);

		// Get the actual size of the allocated buffer.
		hr = m_pAudioClient->GetBufferSize(&m_nBufferFrameCount);
		BREAK_ON_ERROR(hr);

		const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
		hr = m_pAudioClient->GetService(IID_IAudioRenderClient, (void**)&m_pRenderClient);
		BREAK_ON_ERROR(hr);

		// Calculate the actual duration of the allocated buffer.
		m_hnsActualDuration = (REFERENCE_TIME)((double)REFTIMES_PER_SEC * m_nBufferFrameCount / pwfx->nSamplesPerSec);

		hr = S_OK;
	} while (false);

	if (FAILED(hr))
	{
		ReleaseRenderClient();
		::printf("InitRenderClient failed, error code: 0x%x\n", hr);
		return false;
	}
	else
	{
		m_bInited = true;
		return true;
	}
}

bool AudioRender::Init(WAVEFORMATEX *pwfx)
{
	if (!InitAudioClient())
		return false;
	if (pwfx != NULL && !InitRenderClient(pwfx))
		return false;
	return true;
}

void AudioRender::ReleaseRenderClient()
{
	if (m_pwfx != NULL)
	{
		CoTaskMemFree(m_pwfx);
		m_pwfx = NULL;
	}
	SAFE_RELEASE(m_pRenderClient);
	m_bInited = false;
}

void AudioRender::ReleaseAudioClient()
{
	ReleaseRenderClient();
	SAFE_RELEASE(m_pAudioClient);
	SAFE_RELEASE(m_pDevice);
	m_bDone = true;
}

bool AudioRender::SelectDevice(IMMDeviceEnumerator *pEnumerator)
{
	return AudioUtil::SelectDevice(pEnumerator, eRender, m_bDefaultDevice, &m_pDevice);
}

bool AudioRender::StartRender()
{
	if (m_bInited && m_pAudioClient)
	{
		return SUCCEEDED(m_pAudioClient->Start());
	}
	return false;
}

bool AudioRender::StopRender()
{
	if (m_bInited && m_pAudioClient)
	{
		return SUCCEEDED(m_pAudioClient->Stop());
	}
	return false;
}

HRESULT AudioRender::Render()
{
	if (!m_bInited)
	{
		return E_FAIL;
	}

	HRESULT hr;
	DWORD frameCount = 0;
	DWORD flags = 0;

	do
	{
		hr = LoadData(&frameCount, &flags);
		BREAK_ON_ERROR(hr);

		hr = this->StartRender() ? S_OK : E_FAIL;
		BREAK_ON_ERROR(hr);

		while (!m_bDone)
		{
			// Sleep for half the buffer duration.
			DWORD time = (DWORD)(m_hnsActualDuration * frameCount / m_nBufferFrameCount / REFTIMES_PER_MILLISEC / 2);
			if (time < 10)
				time = 10; // Sleep at least 10 ms.
			Sleep(time);

			hr = LoadData(&frameCount, &flags);
			BREAK_ON_ERROR(hr);
		}

		// Wait for last data in buffer to play before stopping.
		Sleep((DWORD)(m_hnsActualDuration * frameCount / m_nBufferFrameCount / REFTIMES_PER_MILLISEC));

		hr = this->StopRender() ? S_OK : E_FAIL;
		BREAK_ON_ERROR(hr);

	} while (false);

	return hr;
}

inline HRESULT AudioRender::LoadData(DWORD *pFrameCount, DWORD *pFlags)
{
	HRESULT hr;
	UINT32 numFramesPadding;
	UINT32 numFramesAvailable;
	BYTE *pData;

	do
	{
		// See how much buffer space is available.
		hr = m_pAudioClient->GetCurrentPadding(&numFramesPadding);
		BREAK_ON_ERROR(hr);

		numFramesAvailable = m_nBufferFrameCount - numFramesPadding;

		// Grab the entire buffer for the initial fill operation.
		hr = m_pRenderClient->GetBuffer(numFramesAvailable, &pData);
		BREAK_ON_ERROR(hr);

		// Load the initial data into the shared buffer.
		hr = this->OnLoadData(pData, &numFramesAvailable, pFlags);
		BREAK_ON_ERROR(hr);

		hr = m_pRenderClient->ReleaseBuffer(numFramesAvailable, *pFlags);
		BREAK_ON_ERROR(hr);

		// return available frame count.
		*pFrameCount = numFramesPadding + numFramesAvailable;

	} while (false);

	return hr;
}

HRESULT AudioRender::OnLoadData(BYTE *pData, UINT32 *pFrameCount, DWORD *pFlags)
{
	*pFlags = AUDCLNT_BUFFERFLAGS_SILENT;

	return S_OK;
}

const WAVEFORMATEX* AudioRender::GetFormat() const
{
	return &m_wfx;
}

bool AudioRender::SetFormat(WAVEFORMATEX *pwfx)
{
	return InitRenderClient(pwfx);
}

bool AudioRender::UseFormat(WAVEFORMATEX *pwfx)
{
	memcpy(&m_wfx, pwfx, sizeof(m_wfx));

	m_nBytesPerSample = pwfx->wBitsPerSample / 8;
	m_nBytesPerFrame = m_nBytesPerSample * pwfx->nChannels;

	return true;
}

bool AudioRender::IsFormatSupported(WAVEFORMATEX *pwfx)
{
	if (m_pAudioClient == NULL)
		return false;

	WAVEFORMATEX *pClosestMatch = NULL;
	auto hr = m_pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, pwfx, &pClosestMatch);
	if (pClosestMatch != NULL)
	{
		CoTaskMemFree(pClosestMatch);
	}
	return hr == S_OK;
}

bool AudioRender::IsDone() const
{
	return m_bDone;
}

void AudioRender::SetDone(bool bDone)
{
	m_bDone = bDone;
}