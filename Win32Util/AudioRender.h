#pragma once
#pragma once
#include <Windows.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>

class AudioRender
{
public:
	AudioRender();
	virtual ~AudioRender();

	virtual bool Init();
	
	virtual HRESULT Render();

	virtual const WAVEFORMATEX* GetFormat() const;
	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);
	virtual HRESULT SetSourceFormat(WAVEFORMATEX *pwfx);

	virtual HRESULT OnLoadData(BYTE *pData, UINT32 nFrameCount, DWORD *pFlags);

	virtual bool IsDone() const;
	virtual void SetDone(bool bDone);

protected:
	virtual void Release();
	virtual HRESULT StartRender();
	virtual HRESULT StopRender();
	inline HRESULT LoadData(DWORD *pFlags);

protected:
	WAVEFORMATEX *m_pwfx;
	WAVEFORMATEX m_srcWfx;
	bool m_bInited;
	bool m_bDone;

	UINT m_nBytesPerSample;
	UINT m_nBytesPerFrame;

private:
	IMMDeviceEnumerator *m_pEnumerator;
	IMMDevice *m_pDevice;
	IAudioClient *m_pAudioClient;
	IAudioRenderClient *m_pRenderClient;
	REFERENCE_TIME m_hnsActualDuration;
	UINT m_bufferFrameCount;
};

