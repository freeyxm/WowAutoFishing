#pragma once
#include "Util/SoundCapture.h"

class Fisher;

typedef bool (Fisher::*FUN_CheckTimeout)(void);
typedef void (Fisher::*FUN_NotifyBite)(void);

class SoundListener :
	public SoundCapture
{
public:
	SoundListener(Fisher *pFisher);
	virtual ~SoundListener();

	void SetCheckTimeout(FUN_CheckTimeout callback);
	void SetNotifyBite(FUN_NotifyBite callback);

	virtual HRESULT RecordData(BYTE *pData, UINT32 nDataLen, BOOL *bDone);
	virtual BOOL NotifyLoop();

private:
	bool MatchSound(BYTE *pData, UINT32 nDataLen);
	bool MatchSound2(BYTE *pData, UINT32 nDataLen);

private:
	Fisher *m_pFisher;
	FUN_CheckTimeout m_cbCheckTimeout = NULL;
	FUN_NotifyBite m_cbNotifyBite = NULL;
};

