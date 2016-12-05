#pragma once
#pragma execution_character_set("utf-8")
#pragma comment(lib, "winmm.lib")
#include <Windows.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>

class AudioCapture
{
public:
	AudioCapture(bool bLoopback = true, bool bDefaultDevice = true);
	virtual ~AudioCapture();

	virtual bool Init();

	virtual const WAVEFORMATEX* GetFormat() const;
	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);

	virtual HRESULT Capture();
	virtual HRESULT OnCaptureData(BYTE *pData, UINT32 nFrameCount);

	virtual bool IsDone() const;
	virtual void SetDone(bool bDone);

protected:
	virtual void Release();
	virtual bool StartCapture();
	virtual bool StopCapture();
	virtual bool SelectDevice(IMMDeviceEnumerator *pEnumerator);

	float ParseValue(BYTE *pData, size_t index) const;

protected:
	WAVEFORMATEX *m_pwfx;
	bool m_bInited;
	bool m_bLoopback;
	bool m_bDefaultDevice;
	bool m_bDone;

	UINT m_nBytesPerSample;
	UINT m_nBytesPerFrame;
	int m_midValue;

private:
	IMMDevice *m_pDevice;
	IAudioClient *m_pAudioClient;
	IAudioCaptureClient *m_pCaptureClient;
	REFERENCE_TIME m_hnsActualDuration;
};
