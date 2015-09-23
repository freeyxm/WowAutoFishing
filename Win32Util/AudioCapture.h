#pragma once
#pragma execution_character_set("utf-8")
#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>

class AudioCapture
{
public:
	AudioCapture(bool bLoopback = true);
	virtual ~AudioCapture();

	virtual HRESULT Init();
	virtual HRESULT Start();
	virtual HRESULT Stop();
	virtual HRESULT Capture();

	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);
	virtual HRESULT OnCaptureData(BYTE *pData, UINT32 nDataLen, BOOL *bDone);
	virtual bool LoopDone();

	float ParseValue(BYTE *pData, UINT index);

	static void PrintDevices(IMMDeviceEnumerator *pEnumerator);

	static bool IsFloatFormat(WAVEFORMATEX *pfwx);

protected:
	virtual void Release();
	
protected:
	WAVEFORMATEX *m_pwfx;
	bool m_bInited;
	bool m_bLoopback;

	bool m_bFloatFormat;
	UINT m_nBytesPerSample;
	int m_maxValue;
	int m_midValue;

private:
	IMMDeviceEnumerator *m_pEnumerator;
	IMMDevice *m_pDevice;
	IAudioClient *m_pAudioClient;
	IAudioCaptureClient *m_pCaptureClient;
	REFERENCE_TIME m_hnsActualDuration;
};
