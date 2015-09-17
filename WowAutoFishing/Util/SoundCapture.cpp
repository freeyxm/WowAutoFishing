#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "SoundCapture.h"
#include <functiondiscoverykeys.h>
#include <cstdio>

SoundCapture::SoundCapture()
	: m_pEnumerator(NULL), m_pDevice(NULL), m_pAudioClient(NULL), m_pCaptureClient(NULL), m_pwfx(NULL)
{
}

SoundCapture::~SoundCapture()
{
	Release();
}

HRESULT SoundCapture::RecordData(BYTE *pData, UINT32 numFramesAvailable, BOOL *bDone)
{
	//printf("RecordData: %d\n", numFramesAvailable);
	*bDone = false;
	return S_OK;
}

HRESULT SoundCapture::SetFormat(WAVEFORMATEX *pwfx)
{
	//printf("Format:\n");
	//printf("  wFormatTag: %d\n", pwfx->wFormatTag);
	//printf("  nChannels: %d\n", pwfx->nChannels);
	//printf("  nSamplesPerSec: %d\n", pwfx->nSamplesPerSec);
	//printf("  nAvgBytesPerSec: %d\n", pwfx->nAvgBytesPerSec);
	//printf("  nBlockAlign: %d\n", pwfx->nBlockAlign);
	//printf("  wBitsPerSample: %d\n", pwfx->wBitsPerSample);
	//printf("  cbSize: %d\n", pwfx->cbSize);

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
		hr = ::CoInitialize(NULL);
		BREAK_ON_ERROR(hr);

		hr = ::CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&m_pEnumerator);
		BREAK_ON_ERROR(hr);

		//PrintDevices(m_pEnumerator);

		//hr = m_pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &m_pDevice);
		hr = m_pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_pDevice);
		BREAK_ON_ERROR(hr);

		hr = m_pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&m_pAudioClient);
		BREAK_ON_ERROR(hr);

		hr = m_pAudioClient->GetMixFormat(&m_pwfx);
		BREAK_ON_ERROR(hr);

		// Notify the audio sink which format to use.
		hr = this->SetFormat(m_pwfx);
		BREAK_ON_ERROR(hr);

		hr = m_pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, m_pwfx, NULL);
		BREAK_ON_ERROR(hr);

		//hr = m_pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, hnsRequestedDuration, 0, pwfx, NULL);
		hr = m_pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, m_pwfx, 0);
		BREAK_ON_ERROR(hr);

		// Get the size of the allocated buffer.
		hr = m_pAudioClient->GetBufferSize(&bufferFrameCount);
		BREAK_ON_ERROR(hr);

		hr = m_pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&m_pCaptureClient);
		BREAK_ON_ERROR(hr);

		// Calculate the actual duration of the allocated buffer.
		m_hnsActualDuration = (REFERENCE_TIME)((double)REFTIMES_PER_SEC * bufferFrameCount / m_pwfx->nSamplesPerSec);

		hr = S_OK;

	} while (false);

	
	if (FAILED(hr))
	{
		Release();
		::printf("Init failed, error code: 0x%x\n", hr);
	}

	return hr;
}

void SoundCapture::Release()
{
	if (m_pwfx != NULL)
	{
		CoTaskMemFree(m_pwfx);
		m_pwfx = NULL;
	}
	SAFE_RELEASE(m_pEnumerator);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pAudioClient);
	SAFE_RELEASE(m_pCaptureClient);
}

void SoundCapture::PrintDevices(IMMDeviceEnumerator *pEnumerator)
{
	HRESULT hr;
	IMMDeviceCollection *pCollection = NULL;
	IMMDevice *pEndpoint = NULL;
	IPropertyStore *pProps = NULL;
	LPWSTR pwszID = NULL;

	do
	{
		hr = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pCollection);
		BREAK_ON_ERROR(hr);

		UINT count = 0;
		hr = pCollection->GetCount(&count);
		BREAK_ON_ERROR(hr);

		for (UINT i = 0; i < count; ++i)
		{
			do
			{
				hr = pCollection->Item(i, &pEndpoint);
				BREAK_ON_ERROR(hr);

				// Get the endpoint ID string.
				hr = pEndpoint->GetId(&pwszID);
				BREAK_ON_ERROR(hr);

				hr = pEndpoint->OpenPropertyStore(STGM_READ, &pProps);
				BREAK_ON_ERROR(hr);

				PROPVARIANT varName;
				::PropVariantInit(&varName);

				hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
				if(SUCCEEDED(hr))
				{
					wprintf(L"Endpoint %d: \"%ls\" (%ls)\n", i, varName.pwszVal, pwszID);
				}

				::PropVariantClear(&varName);
			} while (false);

			if (pwszID)
			{
				CoTaskMemFree(pwszID);
				pwszID = NULL;
			}
			SAFE_RELEASE(pProps);
			SAFE_RELEASE(pEndpoint);
		}
	} while (false);
}

HRESULT SoundCapture::Start()
{
	if (m_pAudioClient)
	{
		return m_pAudioClient->Start();  // Start recording.
	}
	return S_FALSE;
}

HRESULT SoundCapture::Stop()
{
	if (m_pAudioClient)
	{
		return m_pAudioClient->Stop();  // Stop recording.
	}
	return S_FALSE;
}

HRESULT SoundCapture::Record()
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
		Sleep((DWORD)(m_hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

		hr = m_pCaptureClient->GetNextPacketSize(&packetLength);
		BREAK_ON_ERROR(hr);

		while (packetLength != 0)
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
				hr = this->RecordData(pData, numFramesAvailable, &bDone);
				BREAK_ON_ERROR(hr);
			}

			hr = m_pCaptureClient->ReleaseBuffer(numFramesAvailable);
			BREAK_ON_ERROR(hr);

			hr = m_pCaptureClient->GetNextPacketSize(&packetLength);
			BREAK_ON_ERROR(hr);
		}

		if (!bDone)
		{
			bDone = this->NotifyLoop();
		}
	}

	return hr;
}
