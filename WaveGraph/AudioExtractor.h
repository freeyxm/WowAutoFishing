#pragma once
#include "Win32Util/AudioCapture.h"
#include "AudioFrameStorage.h"
#include <process.h>
#include <list>

class AudioExtractor :
	public AudioCapture
{
public:
	typedef std::list<AudioFrameStorage*>::const_iterator SegmentCIter;

public:
	AudioExtractor();
	~AudioExtractor();

	bool StartExtract();
	void StopExtract();

	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);
	virtual HRESULT OnCaptureData(BYTE *pData, UINT32 nFrameCount);

	void SetSilentLimit(float limit);
	void SetSilentMaxCount(UINT count);

	void Clear();

	UINT GetSegmentCount() const;
	AudioFrameStorage* PopSegment();

	SegmentCIter cbegin() const;
	SegmentCIter cend() const;

private:
	void StartSegment();
	void EndSegment();
	void AppendSilentFrames();

private:
	CRITICAL_SECTION m_segmentSection; // lock
	std::list<AudioFrameStorage*> m_segmentList;
	AudioFrameStorage *m_pCurSegment;
	AudioFrameStorage m_silentFrames;
	bool m_bSegmentStarted;
	UINT m_silentCount;
	UINT m_silentMaxCount;
	float m_silentLimit;
	
	HANDLE m_hThreadCapture;
};

