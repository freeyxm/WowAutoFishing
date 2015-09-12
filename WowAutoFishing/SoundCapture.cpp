#include "stdafx.h"
#include "SoundCapture.h"
#include <cstdio>

SoundCapture::SoundCapture()
{
}

SoundCapture::~SoundCapture()
{
	Release();
}

HRESULT SoundCapture::CopyData(BYTE *pData, UINT32 numFramesAvailable, BOOL *bDone)
{
	return S_OK;
}

HRESULT SoundCapture::SetFormat(WAVEFORMATEX *pwfx)
{
	return S_OK;
}

BOOL SoundCapture::NotifyLoop()
{
	return false;
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

#define BREAK_ON_ERROR(hres)  \
              if (FAILED(hres)) { break; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL) { (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

HRESULT SoundCapture::Init()
{
	HRESULT hr = S_FALSE;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	UINT32 bufferFrameCount;

	do
	{
		hr = ::CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
		BREAK_ON_ERROR(hr);

		hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice);
		BREAK_ON_ERROR(hr);

		hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);
		BREAK_ON_ERROR(hr);

		hr = pAudioClient->GetMixFormat(&pwfx);
		BREAK_ON_ERROR(hr);

		hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, hnsRequestedDuration, 0, pwfx, NULL);
		BREAK_ON_ERROR(hr);

		// Get the size of the allocated buffer.
		hr = pAudioClient->GetBufferSize(&bufferFrameCount);
		BREAK_ON_ERROR(hr);

		hr = pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&pCaptureClient);
		BREAK_ON_ERROR(hr);

		// Notify the audio sink which format to use.
		hr = this->SetFormat(pwfx);
		BREAK_ON_ERROR(hr);

		// Calculate the actual duration of the allocated buffer.
		hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / pwfx->nSamplesPerSec;

		hr = S_OK;

	} while (false);

	if (FAILED(hr))
	{
		Release();
		::printf("Init failed.\n");
	}

	return hr;
}

void SoundCapture::Release()
{
	if (pwfx != NULL)
	{
		CoTaskMemFree(pwfx);
		pwfx = NULL;
	}
	SAFE_RELEASE(pEnumerator);
	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pAudioClient);
	SAFE_RELEASE(pCaptureClient);
}

HRESULT SoundCapture::Start()
{
	if (pAudioClient)
	{
		return pAudioClient->Start();  // Start recording.
	}
	return S_FALSE;
}

HRESULT SoundCapture::Stop()
{
	if (pAudioClient)
	{
		return pAudioClient->Stop();  // Stop recording.
	}
	return S_FALSE;
}

HRESULT SoundCapture::RecordAudioStream()
{
	HRESULT hr = S_OK;
	UINT32 numFramesAvailable;
	UINT32 packetLength = 0;
	BOOL bDone = FALSE;
	BYTE *pData;
	DWORD flags;

	// Each loop fills about half of the shared buffer.
	while (bDone == FALSE)
	{
		// Sleep for half the buffer duration.
		Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);

		hr = pCaptureClient->GetNextPacketSize(&packetLength);
		BREAK_ON_ERROR(hr);

		while (packetLength != 0)
		{
			// Get the available data in the shared buffer.
			hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
			BREAK_ON_ERROR(hr);

			if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
			{
				pData = NULL;  // Tell CopyData to write silence.
			}

			// Copy the available capture data to the audio sink.
			hr = this->CopyData(pData, numFramesAvailable, &bDone);
			BREAK_ON_ERROR(hr);

			hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
			BREAK_ON_ERROR(hr);

			hr = pCaptureClient->GetNextPacketSize(&packetLength);
			BREAK_ON_ERROR(hr);
		}

		bDone = this->NotifyLoop();
	}

	return hr;
}
