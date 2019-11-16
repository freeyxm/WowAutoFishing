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

	virtual int Init();

	virtual const WAVEFORMATEX* GetFormat() const;

	virtual int Capture();

	virtual bool IsDone() const;
	virtual void SetDone(bool bDone);

protected:
	virtual void Release();
	virtual  int StartCapture();
	virtual  int StopCapture();
	virtual bool SelectDevice(IMMDeviceEnumerator *pEnumerator);

	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);

	virtual HRESULT OnCaptureData(BYTE *pData, UINT32 nFrameCount);

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
