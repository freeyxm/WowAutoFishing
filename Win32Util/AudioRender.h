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

	virtual HRESULT Init();
	virtual HRESULT Start();
	virtual HRESULT Stop();
	virtual HRESULT Render();

	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);
	virtual HRESULT OnLoadData(BYTE *pData, UINT nDataLen, DWORD *pFlags);

protected:
	virtual void Release();
	inline HRESULT LoadData(DWORD *pFlags);

protected:
	WAVEFORMATEX *m_pwfx;
	bool m_bInited;

private:
	IMMDeviceEnumerator *m_pEnumerator;
	IMMDevice *m_pDevice;
	IAudioClient *m_pAudioClient;
	IAudioRenderClient *m_pRenderClient;
	REFERENCE_TIME m_hnsActualDuration;
	UINT m_bufferFrameCount;
};

