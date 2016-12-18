#pragma once
#include "Win32Util/AudioRender.h"
#include "WaveUtil/WaveFile.h"
#include "AudioFrameStorage.h"

class AudioRenderer :
	public AudioRender
{
public:
	AudioRenderer(bool bDefaultDevice = true);
	virtual ~AudioRenderer();

	void SetSource(const AudioFrameStorage *pStorage);
	void SetSource(WaveFile *pWaveFile);

	virtual bool Start();
	virtual void Stop();

	virtual bool IsDone() const;

	friend UINT __stdcall RenderTheadProc(LPVOID param);

protected:
	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);
	virtual HRESULT OnLoadData(BYTE *pData, UINT32 *pFrameCount, DWORD *pFlags);

	HRESULT LoadDataFromStorage(BYTE *pData, UINT32 *pFrameCount, DWORD *pFlags);
	HRESULT LoadDataFromFile(BYTE *pData, UINT32 *pFrameCount, DWORD *pFlags);

private:
	enum SourceType
	{
		Wave,
		Storage,
	} m_srcType;

	WaveFile *m_pWaveFile;

	const AudioFrameStorage *m_pStorage;
	AudioFrameStorage::AudioFrameCIter m_dataIter;
	UINT m_dataIndex;

	HANDLE m_hThreadRenderer;
	bool m_bPlaying;
};

