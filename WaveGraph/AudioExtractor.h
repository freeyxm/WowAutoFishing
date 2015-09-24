#pragma once
#include "Win32Util/AudioCapture.h"
#include "AudioFrameStorage.h"
#include <process.h>
#include <list>

class AudioExtractor :
	public AudioCapture
{
public:
	AudioExtractor();
	~AudioExtractor();

	bool StartListen();
	void StopListen();

	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);
	virtual HRESULT OnCaptureData(BYTE *pData, UINT32 nFrameCount, BOOL *bDone);
	virtual bool LoopDone();

	void SetSilentLimit(float limit);
	void SetSilentMaxCount(UINT count);

	void Clear();

private:
	void StartSegment();
	void EndSegment();
	void AppendSilentFrames();

private:
	std::list<AudioFrameStorage*> m_segmentList;
	AudioFrameStorage *m_pCurSegment;
	AudioFrameStorage m_silentFrames;
	bool m_bSegmentStarted;
	UINT m_silentCount;
	UINT m_silentMaxCount;
	float m_silentLimit;
	
	bool m_bDone;
	HANDLE m_hThreadCapture;
};

