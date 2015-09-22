#pragma once
#pragma execution_character_set("utf-8")
#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>

class SoundCapture
{
public:
	SoundCapture(bool bLoopback = true);
	virtual ~SoundCapture();

	virtual HRESULT Init();
	virtual HRESULT StartCapture();
	virtual HRESULT StopCapture();
	virtual HRESULT Capture();

	virtual HRESULT OnCaptureData(BYTE *pData, UINT32 nDataLen, BOOL *bDone);
	virtual bool LoopDone();

	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);
	const WAVEFORMATEX* GetFormat();

	static void PrintDevices(IMMDeviceEnumerator *pEnumerator);

protected:
	virtual void Release();
	
protected:
	WAVEFORMATEX *m_pwfx;
	bool m_bInited;
	bool m_bLoopback;

private:
	IMMDeviceEnumerator *m_pEnumerator;
	IMMDevice *m_pDevice;
	IAudioClient *m_pAudioClient;
	IAudioCaptureClient *m_pCaptureClient;
	REFERENCE_TIME m_hnsActualDuration;
};

