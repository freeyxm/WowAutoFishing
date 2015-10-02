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

	virtual bool Init();

	virtual HRESULT Capture();

	virtual const WAVEFORMATEX* GetFormat() const;
	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);

	virtual HRESULT OnCaptureData(BYTE *pData, UINT32 nFrameCount);
	
	virtual bool IsDone() const;
	virtual void SetDone(bool bDone);

	float ParseValue(BYTE *pData, UINT index) const;

	static void PrintDevices(IMMDeviceEnumerator *pEnumerator);

	static bool IsFloatFormat(const WAVEFORMATEX *pfwx);

protected:
	virtual void Release();
	virtual bool StartCapture();
	virtual bool StopCapture();
	
protected:
	WAVEFORMATEX *m_pwfx;
	bool m_bInited;
	bool m_bLoopback;
	bool m_bDone;

	bool m_bFloatFormat;
	UINT m_nBytesPerSample;
	UINT m_nBytesPerFrame;
	int m_maxValue;
	int m_midValue;

private:
	IMMDeviceEnumerator *m_pEnumerator;
	IMMDevice *m_pDevice;
	IAudioClient *m_pAudioClient;
	IAudioCaptureClient *m_pCaptureClient;
	REFERENCE_TIME m_hnsActualDuration;
};
