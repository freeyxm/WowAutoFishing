#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "AudioCapture.h"
#include <functiondiscoverykeys.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <assert.h>

AudioCapture::AudioCapture(bool bLoopback, bool bDefaultDevice)
	: m_pDevice(NULL), m_pAudioClient(NULL), m_pCaptureClient(NULL), m_pwfx(NULL)
	, m_bInited(false), m_bDone(true)
	, m_bLoopback(bLoopback), m_bDefaultDevice(bDefaultDevice)
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

#define BREAK_ON_ERROR(hres)  \
	if (FAILED(hres)) { break; }
#define SAFE_RELEASE(punk)  \
	if ((punk) != NULL) { (punk)->Release(); (punk) = NULL; }


bool AudioCapture::Init()
{
	HRESULT hr = S_FALSE;
	IMMDeviceEnumerator *pEnumerator = NULL;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	UINT32 bufferFrameCount;
	m_bInited = false;

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
	return SelectDevice(pEnumerator, m_bLoopback ? eRender : eCapture, m_bDefaultDevice, &m_pDevice);
}

bool AudioCapture::SelectDevice(IMMDeviceEnumerator *pEnumerator, EDataFlow eDataFlow, bool bDefault, IMMDevice **ppDevice)
{
	HRESULT hr;
	if (bDefault)
	{
		hr = pEnumerator->GetDefaultAudioEndpoint(eDataFlow, eConsole, ppDevice);
		return SUCCEEDED(hr) ? true : false;
	}
	else
	{
		std::list<DeviceInfo> devices;
		int count = GetDevices(eDataFlow, pEnumerator, devices);

		// Print device name list
		printf("---------------------------------------\n");
		printf("Current Available Devices:\n");
		int index = 1;
		for (std::list<DeviceInfo>::const_iterator it = devices.cbegin(); it != devices.cend(); ++it)
		{
			wprintf_s(L"%d. %ls\n", index, it->sName);
			++index;
		}
		printf("---------------------------------------\n");

		// Select device by index
		index = GetSelectIndex(1, count);
		if (index >= 1 && index <= count)
		{
			std::list<DeviceInfo>::const_iterator it = devices.begin();
			for (int i = 1; i < index; ++i)
			{
				++it;
			}
			*ppDevice = it->pDevice;
		}

		// Release unselected devices
		for (std::list<DeviceInfo>::const_iterator it = devices.cbegin(); it != devices.cend(); ++it)
		{
			if (*ppDevice != it->pDevice)
			{
				IMMDevice *pDevice = it->pDevice;
				SAFE_RELEASE(pDevice);
			}
		}
		devices.clear();

		return count > 0 && *ppDevice != NULL;
	}
}

UINT AudioCapture::GetDevices(EDataFlow eDataFlow, IMMDeviceEnumerator *pEnumerator, std::list<DeviceInfo> &devices)
{
	HRESULT hr;
	IMMDeviceCollection *pCollection = NULL;
	IMMDevice *pDevice = NULL;
	IPropertyStore *pProps = NULL;
	//LPWSTR pwszID = NULL;
	UINT count = 0;

	do
	{
		hr = pEnumerator->EnumAudioEndpoints(eDataFlow, DEVICE_STATE_ACTIVE, &pCollection);
		BREAK_ON_ERROR(hr);

		hr = pCollection->GetCount(&count);
		BREAK_ON_ERROR(hr);

		if (count == 0)
			break;

		for (UINT i = 0; i < count; ++i)
		{
			DeviceInfo devInfo = { 0 };
			do
			{
				hr = pCollection->Item(i, &pDevice);
				BREAK_ON_ERROR(hr);

				// Get the endpoint ID string.
				//hr = pDevice->GetId(&pwszID);
				//BREAK_ON_ERROR(hr);

				hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
				BREAK_ON_ERROR(hr);

				PROPVARIANT varName;
				::PropVariantInit(&varName);

				hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
				if (SUCCEEDED(hr))
				{
					wsprintf(devInfo.sName, L"%ls", varName.pwszVal);
				}
				else
				{
					wsprintf(devInfo.sName, L"Unknown name");
				}

				::PropVariantClear(&varName);

				devInfo.pDevice = pDevice;
			} while (false);

			//if (pwszID)
			//{
			//	CoTaskMemFree(pwszID);
			//	pwszID = NULL;
			//}
			SAFE_RELEASE(pProps);

			if (devInfo.pDevice != 0)
			{
				devices.push_back(devInfo);
			}
		}
	} while (false);

	SAFE_RELEASE(pCollection);

	return count;
}

int AudioCapture::GetSelectIndex(int min, int max)
{
	if (min == max)
		return min;

	int selectIndex = 0;
	bool printTip = true;
	std::string line;
	do
	{
		if (printTip)
			printf("Please select a device (index): ");

		if (!getline(std::cin, line))
			break;

		if (line.empty())
		{
			printTip = false;
			continue;
		}

		selectIndex = ::atoi(line.c_str());
		if (selectIndex >= min && selectIndex <= max)
		{
			break;
		}
		else
		{
			printf("Device index between %d and %d, please re-choose.\n", min, max);
			printTip = true;
		}
	} while (true);

	return selectIndex;
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
	m_midValue = GetMidValue(m_pwfx);

	return S_OK;
}

HRESULT AudioCapture::OnCaptureData(BYTE *pData, UINT32 nFrameCount)
{
	//printf("OnCaptureData: %d\n", nFrameCount);
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

int AudioCapture::GetMidValue(const WAVEFORMATEX *pwfx)
{
	return ((1L << (pwfx->wBitsPerSample - 1)) - 1) >> 1;
}

bool AudioCapture::IsFloatFormat(const WAVEFORMATEX *pwfx)
{
	if (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
	{
		return true;
	}
	else if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		WAVEFORMATEXTENSIBLE *pfwxt = (WAVEFORMATEXTENSIBLE*)pwfx;
		if (pfwxt->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
		{
			return true;
		}
	}
	return false;
}

float AudioCapture::ParseValue(BYTE *pData, UINT index) const
{
	return ParseValue(m_pwfx, pData, index, m_midValue);
}

float AudioCapture::ParseValue(const WAVEFORMATEX *pwfx, const void *pData, UINT index, int midValue)
{
	switch (pwfx->wBitsPerSample)
	{
	case 8:
		return (float)*((INT8*)pData + index) / midValue;
		break;
	case 16:
		return (float)*((INT16*)pData + index) / midValue;
		break;
	case 32:
		if (IsFloatFormat(pwfx))
			return (float)*((float*)pData + index);
		else
			return (float)*((INT32*)pData + index) / midValue;
		break;
	default:
		assert(false);
		break;
	}
	return 0;
}
