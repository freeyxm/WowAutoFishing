#pragma once
#include "Win32Util/Audio/AudioRender.h"
#include "WaveUtil/WaveFile.h"
#include "WaveUtil/WaveStreamConverter.h"
#include "AudioFrameStorage.h"

class AudioRenderer :
	public AudioRender
{
public:
	AudioRenderer(bool bDefaultDevice = true);
	virtual ~AudioRenderer();

	void SetSource(const AudioFrameStorage *pStorage);
	bool SetSourceFile(const char *pWaveFile);

	virtual bool Start();
	virtual void Stop();

	virtual bool IsDone() const;

	friend UINT __stdcall RenderTheadProc(LPVOID param);

protected:
	virtual bool UseFormat(WAVEFORMATEX *pwfx);
	virtual HRESULT OnLoadData(BYTE *pData, UINT32 *pFrameCount, DWORD *pFlags);

	HRESULT LoadDataFromStorage(BYTE *pData, UINT32 *pFrameCount, DWORD *pFlags);
	HRESULT LoadDataFromFile(BYTE *pData, UINT32 *pFrameCount, DWORD *pFlags);

private:
	enum class SourceType
	{
		Wave,
		Storage,
	} m_srcType;

	WaveFile *m_pWaveFile;
	WaveStreamConverter *m_pConverter;
	bool m_bConvert;

	const AudioFrameStorage *m_pStorage;
	AudioFrameStorage::AudioFrameCIter m_dataIter;
	UINT m_dataIndex;

	HANDLE m_hThreadRenderer;
	bool m_bPlaying;
};

