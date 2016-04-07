#pragma once
#pragma execution_character_set("utf-8")
#pragma comment(lib, "winmm.lib")
#include <Windows.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <list>

class AudioCapture
{
public:
	struct DeviceInfo
	{
		IMMDevice *pDevice;
		wchar_t sName[128];
	};

public:
	AudioCapture(bool bLoopback = true, bool bDefaultDevice = true);
	virtual ~AudioCapture();

	virtual bool Init();

	virtual HRESULT Capture();

	virtual const WAVEFORMATEX* GetFormat() const;
	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);

	virtual HRESULT OnCaptureData(BYTE *pData, UINT32 nFrameCount);
	
	virtual bool IsDone() const;
	virtual void SetDone(bool bDone);

	float ParseValue(BYTE *pData, size_t index) const;
	static float ParseValue(const WAVEFORMATEX *pwfx, const void *pData, size_t index, int midValue);

	static int GetMidValue(const WAVEFORMATEX *pwfx);

	static bool IsFloatFormat(const WAVEFORMATEX *pwfx);

	static bool SelectDevice(IMMDeviceEnumerator *pEnumerator, EDataFlow eDataFlow, bool bDefault, IMMDevice **ppDevice);

protected:
	virtual void Release();
	virtual bool StartCapture();
	virtual bool StopCapture();
	virtual bool SelectDevice(IMMDeviceEnumerator *pEnumerator);

	static UINT GetDevices(EDataFlow eDataFlow, IMMDeviceEnumerator *pEnumerator, std::list<DeviceInfo> &devices);
	static int GetSelectIndex(int min, int max);
	
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
