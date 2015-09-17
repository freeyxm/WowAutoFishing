#pragma once
#include "Util/SoundCapture.h"

class Fisher;

typedef bool (Fisher::*Fun_CheckTimeout)(void);
typedef void (Fisher::*Fun_NotifyBite)(void);

class SoundListener :
	public SoundCapture
{
public:
	SoundListener(Fisher *pFisher);
	virtual ~SoundListener();

	void SetCheckTimeout(Fun_CheckTimeout callback);
	void SetNotifyBite(Fun_NotifyBite callback);

	virtual HRESULT RecordData(BYTE *pData, UINT32 nDataLen, BOOL *bDone);
	virtual BOOL NotifyLoop();

private:
	bool MatchSound(BYTE *pData, UINT32 nDataLen);
	bool MatchSound2(BYTE *pData, UINT32 nDataLen);

private:
	Fisher *m_pFisher;
	Fun_CheckTimeout m_funCheckTimeout;
	Fun_NotifyBite m_funNotifyBite;
};

