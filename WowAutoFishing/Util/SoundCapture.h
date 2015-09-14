#pragma once
#pragma execution_character_set("utf-8")
#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>

class SoundCapture
{
public:
	SoundCapture();
	virtual ~SoundCapture();

	HRESULT Init();
	HRESULT Start();
	HRESULT Stop();
	HRESULT Record();

	virtual HRESULT RecordData(BYTE *pData, UINT32 numFramesAvailable, BOOL *bDone);
	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);
	virtual BOOL NotifyLoop();

private:
	void Release();
	void PrintDevices(IMMDeviceEnumerator *pEnumerator);

private:
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDevice *pDevice = NULL;
	IAudioClient *pAudioClient = NULL;
	IAudioCaptureClient *pCaptureClient = NULL;
	WAVEFORMATEX *pwfx = NULL;
	REFERENCE_TIME hnsActualDuration;
};

