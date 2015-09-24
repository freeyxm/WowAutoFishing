#include "stdafx.h"
#include "AudioExtractor.h"


AudioExtractor::AudioExtractor()
	: m_pCurSegment(NULL), m_bSegmentStarted(false), m_bDone(false)
{
	m_silentLimit = 0.0001f;
	m_silentCount = 0;
	m_silentMaxCount = 5;
}


AudioExtractor::~AudioExtractor()
{
	Clear();
}

void AudioExtractor::Clear()
{
	for(std::list<AudioFrameStorage*>::iterator it = m_segmentList.begin(); it != m_segmentList.end();)
	{
		AudioFrameStorage *pStorage = *it;
		if(pStorage != NULL)
		{
			pStorage->Clear();
			delete pStorage;
		}
		it = m_segmentList.erase(it);
	}
	if(m_pCurSegment != NULL)
	{
		m_pCurSegment->Clear();
		delete m_pCurSegment;
		m_pCurSegment = NULL;
	}
	m_silentFrames.Clear();
}

HRESULT AudioExtractor::SetFormat(WAVEFORMATEX *pwfx)
{
	AudioCapture::SetFormat(pwfx);
	return S_OK;
}

HRESULT AudioExtractor::OnCaptureData(BYTE *pData, UINT32 nFrameCount, BOOL *bDone)
{
	float value, min = 0, max = 0, avg = 0;
	for(UINT32 i = 0; i < nFrameCount; ++i)
	{
		for(int k = 0; k < m_pwfx->nChannels; ++k)
		{
			value = ParseValue(pData, i*2);
			if(value < min)
				min = value;
			else if(value > max)
				max = value;
		}
	}
	if(max > m_silentLimit || min < -m_silentLimit)
	{
		if(!m_bSegmentStarted)
		{
			StartSegment();
		}
		if(m_silentCount > 0)
		{
			AppendSilentFrames();
		}
		m_pCurSegment->PushBack(pData, nFrameCount * m_nBytesPerFrame);
	}
	else
	{
		if(m_bSegmentStarted)
		{
			if(++m_silentCount < m_silentMaxCount)
			{
				m_silentFrames.PushBack(pData, nFrameCount * m_nBytesPerFrame);
			}
			else
			{
				EndSegment();
				m_silentFrames.Clear();
			}
		}
	}
	
	*bDone = m_bDone;

	return S_OK;
}

bool AudioExtractor::LoopDone()
{
	return m_bDone;
}

void AudioExtractor::SetSilentLimit(float limit)
{
	m_silentLimit = limit;
}

void AudioExtractor::SetSilentMaxCount(UINT count)
{
	m_silentMaxCount = count;
}

void AudioExtractor::StartSegment()
{
	if(m_bSegmentStarted)
		return;

	if(m_pCurSegment == NULL)
	{
		m_pCurSegment = new AudioFrameStorage();
	}
	m_silentCount = 0;
	m_bSegmentStarted = true;
}

void AudioExtractor::EndSegment()
{
	if(!m_bSegmentStarted)
		return;

	if(m_pCurSegment != NULL)
	{
		m_segmentList.push_back(m_pCurSegment);
		m_pCurSegment = NULL;
	}
	m_bSegmentStarted = false;
}

void AudioExtractor::AppendSilentFrames()
{
	if(m_pCurSegment == NULL)
		return;

	while(m_silentFrames.GetSize() > 0)
	{
		AudioFrameData *pFrame = m_silentFrames.PopFront();
		if(pFrame != NULL)
		{
			m_pCurSegment->PushBack(pFrame);
		}
	}
	m_silentCount = 0;
}