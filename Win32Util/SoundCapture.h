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
	virtual bool LoopWait();

private:
	void Release();
	void PrintDevices(IMMDeviceEnumerator *pEnumerator);

protected:
	WAVEFORMATEX *m_pwfx;

private:
	IMMDeviceEnumerator *m_pEnumerator;
	IMMDevice *m_pDevice;
	IAudioClient *m_pAudioClient;
	IAudioCaptureClient *m_pCaptureClient;
	REFERENCE_TIME m_hnsActualDuration;
};

