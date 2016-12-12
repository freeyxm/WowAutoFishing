#pragma once
#pragma execution_character_set("utf-8")
#include <Windows.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>

class AudioRender
{
public:
	AudioRender(bool bDefaultDevice = true);
	virtual ~AudioRender();

	virtual bool Init();
	virtual bool Init(WAVEFORMATEX *pwfx);

	virtual const WAVEFORMATEX* GetFormat() const;

	virtual HRESULT Render();

	virtual bool IsDone() const;
	virtual void SetDone(bool bDone);

protected:
	virtual void Release();
	virtual bool StartRender();
	virtual bool StopRender();
	virtual bool SelectDevice(IMMDeviceEnumerator *pEnumerator);

	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);
	virtual HRESULT SetSourceFormat(WAVEFORMATEX *pwfx);

	inline HRESULT LoadData(DWORD *pFlags);
	virtual HRESULT OnLoadData(BYTE *pData, UINT32 nFrameCount, DWORD *pFlags);

protected:
	WAVEFORMATEX *m_pwfx;
	WAVEFORMATEX m_srcWfx;
	bool m_bDefaultDevice;
	bool m_bInited;
	bool m_bDone;

	UINT m_nBytesPerSample;
	UINT m_nBytesPerFrame;

private:
	IMMDevice *m_pDevice;
	IAudioClient *m_pAudioClient;
	IAudioRenderClient *m_pRenderClient;
	REFERENCE_TIME m_hnsActualDuration;
	UINT m_bufferFrameCount;
};

