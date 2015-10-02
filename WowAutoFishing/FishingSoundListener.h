#pragma once
#include "AudioUtil/AudioExtractor.h"

class Fisher;

class FishingSoundListener :
	public AudioExtractor
{
public:
	typedef bool (Fisher::*CheckTimeoutProc)(void);
	typedef void (Fisher::*NotifyBiteProc)(void);

public:
	FishingSoundListener(Fisher *pFisher);
	virtual ~FishingSoundListener();

	void SetCheckTimeoutProc(CheckTimeoutProc callback);
	void SetNotifyBiteProc(NotifyBiteProc callback);

	virtual bool Init();

	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);
	virtual bool IsDone() const;

protected:
	bool MatchSound(BYTE *pData, UINT32 nFrameCount);

	virtual void StartSegment();
	virtual void EndSegment();
	virtual void AppendSilentFrames();
	virtual void ClearSilentFrames();

	inline virtual void AddFrame(BYTE *pData, UINT32 nFrameCount, float amp);
	inline virtual void AddSilentFrame(BYTE *pData, UINT32 nFrameCount, float amp);

	inline virtual UINT GetCurFrameCount();

private:
	Fisher *m_pFisher;
	FILE *m_pSampleFile;
	CheckTimeoutProc m_procCheckTimeout;
	NotifyBiteProc m_procNotifyBite;

	uint32_t m_nFrameCount;
	uint32_t m_nSlientFrameCount;
	float m_totalAmp;
	float m_slientAmp;
};

