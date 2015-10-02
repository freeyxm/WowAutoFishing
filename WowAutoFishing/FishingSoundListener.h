#pragma once
#include "AudioUtil/AudioExtractor.h"

class Fisher;

class FishingSoundListener :
	public AudioExtractor
{
public:
	typedef bool (Fisher::*Fun_CheckTimeout)(void);
	typedef void (Fisher::*Fun_NotifyBite)(void);

public:
	FishingSoundListener(Fisher *pFisher);
	virtual ~FishingSoundListener();

	void SetCheckTimeout(Fun_CheckTimeout callback);
	void SetNotifyBite(Fun_NotifyBite callback);

	virtual HRESULT Init();

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
	Fun_CheckTimeout m_funCheckTimeout;
	Fun_NotifyBite m_funNotifyBite;

	uint32_t m_nFrameCount;
	uint32_t m_nSlientFrameCount;
	float m_totalAmp;
	float m_slientAmp;
};

