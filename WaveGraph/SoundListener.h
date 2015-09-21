#pragma once

#include "Win32Util/SoundCapture.h"

class SoundListener :
	public SoundCapture
{
public:
	SoundListener(void);
	~SoundListener(void);

	bool Init();
	bool Paint(HWND hwnd, HDC hdc);
	bool Record2();

	virtual HRESULT RecordData(BYTE *pData, UINT32 numFramesAvailable, BOOL *bDone);
	virtual bool LoopWait();

private:
	UINT BUF_SIZE;
	float *m_pBuf;
	int m_nBufLen;
};

