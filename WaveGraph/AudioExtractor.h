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

	void SetSilentMaxCount(UINT count);
	void SetSoundMinCount(UINT count);
	void SetAmpZcr(UINT frameCount, float ampL, float ampH, float zcrL, float zcrH);

	void Clear();

	UINT GetSegmentCount() const;
	AudioFrameStorage* PopSegment();

	SegmentCIter cbegin() const;
	SegmentCIter cend() const;

private:
	void StartSegment();
	void EndSegment();
	void CancelSegment();
	void AppendSilentFrames();

private:
	CRITICAL_SECTION m_segmentSection; // lock
	std::list<AudioFrameStorage*> m_segmentList;
	AudioFrameStorage *m_pCurSegment;
	AudioFrameStorage m_silentFrames;
	
	UINT m_silentCount;
	UINT m_silentMaxCount;
	UINT m_soundMinCount;

	enum class SoundState
	{
		Silent,
		Sound,
	} m_eSoundState;
	struct
	{
		UINT frameCount; // 短时能量参考帧数。
		float ampL; // 短时能量低门限
		float ampH; // 短时能量高门限
		float zcrL; // 过零率低门限
		float zcrH; // 过零率高门限
	} m_sAmpZcr;
	
	HANDLE m_hThreadCapture;
};

