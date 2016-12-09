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
	AudioExtractor(bool bLoopback = true, bool bDefaultDevice = true);
	virtual ~AudioExtractor();

	bool Start();
	void Stop();

	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);

	void SetSilentMaxCount(UINT count);
	void SetSoundMinCount(UINT count);
	void SetSoundMaxCount(UINT count);
	void SetAmpZcr(UINT frameCount, float ampL, float ampH, float zcrL, float zcrH);

	void Clear();

	size_t GetSegmentCount() const;
	void SetSegmentMaxCount(UINT count);
	AudioFrameStorage* PopSegment();

	SegmentCIter cbegin() const;
	SegmentCIter cend() const;

	inline void Lock();
	inline void Unlock();

protected:
	virtual void StartSegment();
	virtual void EndSegment();
	virtual void CancelSegment();
	virtual void AppendSilentFrames();
	virtual void ClearSilentFrames();

	inline virtual void AddFrame(BYTE *pData, UINT32 nFrameCount, float amp);
	inline virtual void AddSilentFrame(BYTE *pData, UINT32 nFrameCount, float amp);
	inline virtual void PopSilentFrame(BYTE *pData, UINT32 nFrameCount, float amp);

	inline virtual size_t GetCurFrameCount();

	virtual HRESULT OnCaptureData(BYTE *pData, UINT32 nFrameCount);

protected:
	CRITICAL_SECTION m_segmentSection; // lock
	std::list<AudioFrameStorage*> m_segmentList;
	AudioFrameStorage *m_pCurSegment;
	AudioFrameStorage m_silentFrames;
	
	UINT m_silentCount;
	UINT m_silentMaxCount;
	UINT m_soundMinCount;
	UINT m_soundMaxCount;
	UINT m_segmentMaxCount;

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

