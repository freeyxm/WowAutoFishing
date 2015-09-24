#include "stdafx.h"
#include "AudioExtractor.h"


AudioExtractor::AudioExtractor()
	: m_pCurSegment(NULL), m_bSegmentStart(false), m_bDone(false)
{
}


AudioExtractor::~AudioExtractor()
{
}


HRESULT AudioExtractor::SetFormat(WAVEFORMATEX *pwfx)
{
	AudioCapture::SetFormat(pwfx);
	return S_OK;
}

HRESULT AudioExtractor::OnCaptureData(BYTE *pData, UINT32 nFrameCount, BOOL *bDone)
{
	return S_OK;
}

bool AudioExtractor::LoopDone()
{
	return m_bDone;
}
