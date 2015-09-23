#pragma once
#include "Win32Util/AudioCapture.h"

class Fisher;

typedef bool (Fisher::*Fun_CheckTimeout)(void);
typedef void (Fisher::*Fun_NotifyBite)(void);

class AudioListener :
	public AudioCapture
{
public:
	AudioListener(Fisher *pFisher);
	virtual ~AudioListener();

	void SetCheckTimeout(Fun_CheckTimeout callback);
	void SetNotifyBite(Fun_NotifyBite callback);

	virtual HRESULT OnCaptureData(BYTE *pData, UINT32 nDataLen, BOOL *bDone);
	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);
	virtual bool LoopDone();

private:
	bool MatchSound(BYTE *pData, UINT32 nDataLen);

private:
	Fisher *m_pFisher;
	Fun_CheckTimeout m_funCheckTimeout;
	Fun_NotifyBite m_funNotifyBite;

	UINT m_nBytesPerSample;
	int m_maxValue;
	int m_midValue;

	bool m_waveFormatFloat;
};

