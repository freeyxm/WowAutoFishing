#include "stdafx.h"
#include "AudioRender.h"
#include <cstdio>

AudioRender::AudioRender()
	: m_pEnumerator(NULL), m_pDevice(NULL), m_pAudioClient(NULL), m_pRenderClient(NULL), m_pwfx(NULL)
	, m_bInited(false)
{
}


AudioRender::~AudioRender()
{
	Release();
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

#define BREAK_ON_ERROR(hres)  \
	if (FAILED(hres)) { break; }
#define SAFE_RELEASE(punk)  \
	if ((punk) != NULL) { (punk)->Release(); (punk) = NULL; }


HRESULT AudioRender::Init()
{
	HRESULT hr = S_FALSE;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	m_bInited = false;

	do
	{
		const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
		const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
		const IID IID_IAudioClient = __uuidof(IAudioClient);
		const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

		hr = ::CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&m_pEnumerator);
		BREAK_ON_ERROR(hr);

		hr = m_pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_pDevice);
		BREAK_ON_ERROR(hr);

		hr = m_pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&m_pAudioClient);
		BREAK_ON_ERROR(hr);

		hr = m_pAudioClient->GetMixFormat(&m_pwfx);
		BREAK_ON_ERROR(hr);

		hr = m_pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, hnsRequestedDuration, 0, m_pwfx, NULL);
		BREAK_ON_ERROR(hr);

		// Tell the audio source which format to use.
		hr = this->SetFormat(m_pwfx);
		BREAK_ON_ERROR(hr);

		// Get the actual size of the allocated buffer.
		hr = m_pAudioClient->GetBufferSize(&m_bufferFrameCount);
		BREAK_ON_ERROR(hr);

		hr = m_pAudioClient->GetService(IID_IAudioRenderClient, (void**)&m_pRenderClient);
		BREAK_ON_ERROR(hr);

		// Calculate the actual duration of the allocated buffer.
		m_hnsActualDuration = (REFERENCE_TIME)((double)REFTIMES_PER_SEC * m_bufferFrameCount / m_pwfx->nSamplesPerSec);

		hr = S_OK;

	} while (false);

	if (FAILED(hr))
	{
		Release();
		::printf("Init failed, error code: 0x%x\n", hr);
	}
	else
	{
		m_bInited = true;
	}

	return hr;
}

void AudioRender::Release()
{
	if (m_pwfx != NULL)
	{
		CoTaskMemFree(m_pwfx);
		m_pwfx = NULL;
	}
	SAFE_RELEASE(m_pEnumerator);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pAudioClient);
	SAFE_RELEASE(m_pRenderClient);
	m_bInited = false;
}

HRESULT AudioRender::Start()
{
	if (!m_bInited || m_pRenderClient == NULL)
		return E_FAIL;

	return m_pAudioClient->Start();
}

HRESULT AudioRender::Stop()
{
	if (!m_bInited || m_pRenderClient == NULL)
		return E_FAIL;

	return m_pAudioClient->Stop();
}

HRESULT AudioRender::Render()
{
	if (!m_bInited)
	{
		return E_FAIL;
	}

	HRESULT hr;
	DWORD flags = 0;

	do
	{
		hr = LoadData(&flags);
		BREAK_ON_ERROR(hr);

		hr = this->Start();
		BREAK_ON_ERROR(hr);

		while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
		{
			// Sleep for half the buffer duration.
			Sleep((DWORD)(m_hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

			hr = LoadData(&flags);
			BREAK_ON_ERROR(hr);
		}

		// Wait for last data in buffer to play before stopping.
		Sleep((DWORD)(m_hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

		hr = this->Stop();
		BREAK_ON_ERROR(hr);

	} while (false);

	return hr;
}

inline HRESULT AudioRender::LoadData(DWORD *pFlags)
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

		numFramesAvailable = m_bufferFrameCount - numFramesPadding;

		// Grab the entire buffer for the initial fill operation.
		hr = m_pRenderClient->GetBuffer(numFramesAvailable, &pData);
		BREAK_ON_ERROR(hr);

		// Load the initial data into the shared buffer.
		hr = this->OnLoadData(pData, numFramesAvailable, pFlags);
		BREAK_ON_ERROR(hr);

		hr = m_pRenderClient->ReleaseBuffer(numFramesAvailable, *pFlags);
		BREAK_ON_ERROR(hr);

	} while (false);

	return hr;
}

HRESULT AudioRender::SetFormat(WAVEFORMATEX *pwfx)
{
	m_nBytesPerSample = pwfx->wBitsPerSample / 8;
	m_nBytesPerFrame = m_nBytesPerSample * m_pwfx->nChannels;

	return S_OK;
}

HRESULT AudioRender::OnLoadData(BYTE *pData, UINT32 nFrameCount, DWORD *pFlags)
{
	*pFlags = AUDCLNT_BUFFERFLAGS_SILENT;

	return S_OK;
}