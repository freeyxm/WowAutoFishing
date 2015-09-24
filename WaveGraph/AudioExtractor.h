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

private:
	std::list<AudioFrameStorage*> m_segmentList;
	AudioFrameStorage *m_pCurSegment;
	bool m_bSegmentStart;

	bool m_bDone;
	HANDLE m_hThreadCapture;
};

