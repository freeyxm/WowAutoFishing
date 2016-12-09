#pragma once
#include "Win32Util/AudioRender.h"
#include "AudioFrameStorage.h"

class AudioRenderer :
	public AudioRender
{
public:
	AudioRenderer(bool bDefaultDevice = true);
	virtual ~AudioRenderer();

	void SetSource(const AudioFrameStorage *pStorage);

	virtual bool Start();
	virtual void Stop();

	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);

	virtual bool IsDone() const;

	friend UINT __stdcall RenderTheadProc(LPVOID param);

protected:
	virtual HRESULT OnLoadData(BYTE *pData, UINT32 nFrameCount, DWORD *pFlags);

private:
	const AudioFrameStorage *m_pStorage;
	AudioFrameStorage::AudioFrameCIter m_dataIter;
	UINT m_dataIndex;

	HANDLE m_hThreadRenderer;
	bool m_bPlaying;
};

