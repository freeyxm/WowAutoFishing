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

	virtual bool Init(WAVEFORMATEX *pwfx = NULL);

	virtual bool TryFormat(WAVEFORMATEX *pwfx);
	virtual const WAVEFORMATEX* GetFormat() const;

	virtual HRESULT Render();

	virtual bool IsDone() const;
	virtual void SetDone(bool bDone);

protected:
	virtual bool InitAudioClient();
	virtual void ReleaseAudioClient();

	virtual bool InitRenderClient(WAVEFORMATEX *pwfx);
	virtual void ReleaseRenderClient();

	virtual bool SetFormat(WAVEFORMATEX *pwfx);
	virtual bool IsFormatSupported(WAVEFORMATEX *pwfx);

	virtual bool StartRender();
	virtual bool StopRender();
	virtual bool SelectDevice(IMMDeviceEnumerator *pEnumerator);

	inline HRESULT LoadData(DWORD *pFrameCount, DWORD *pFlags);
	virtual HRESULT OnLoadData(BYTE *pData, UINT32 *pFrameCount, DWORD *pFlags);

protected:
	WAVEFORMATEX *m_pwfx;
	WAVEFORMATEX m_wfx;
	bool m_bDefaultDevice;
	bool m_bInited;
	bool m_bDone;

	UINT m_nBytesPerSample;
	UINT m_nBytesPerFrame;
	UINT m_nBufferFrameCount;

private:
	IMMDevice *m_pDevice;
	IAudioClient *m_pAudioClient;
	IAudioRenderClient *m_pRenderClient;
	REFERENCE_TIME m_hnsActualDuration;
};

