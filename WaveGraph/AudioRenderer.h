#pragma once
#include "Win32Util/AudioRender.h"
#include "AudioFrameStorage.h"

class AudioRenderer :
	public AudioRender
{
public:
	AudioRenderer();
	virtual ~AudioRenderer();

	void SetStorage(AudioFrameStorage *pStorage);

	virtual bool StartRender();
	virtual bool StopRender();

	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);
	virtual HRESULT OnLoadData(BYTE *pData, UINT nDataLen, DWORD *pFlags);

private:
	AudioFrameStorage *m_pStorage;
	AudioFrameCIter m_dataIter;
	UINT m_dataIndex;

	bool m_bDone;
	HANDLE m_hThreadCapture;
};

