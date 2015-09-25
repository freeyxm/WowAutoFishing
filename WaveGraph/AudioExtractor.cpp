#include "stdafx.h"
#include "AudioExtractor.h"

static UINT __stdcall CaptureTheadProc(LPVOID param);

AudioExtractor::AudioExtractor()
	: m_pCurSegment(NULL), m_bSegmentStarted(false)
{
	m_silentLimit = 0.0001f;
	m_silentCount = 0;
	m_silentMaxCount = 5;
	InitializeCriticalSection(&m_segmentSection);
}


AudioExtractor::~AudioExtractor()
{
	Clear();
	DeleteCriticalSection(&m_segmentSection);
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

HRESULT AudioExtractor::OnCaptureData(BYTE *pData, UINT32 nFrameCount)
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

	return S_OK;
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
		::EnterCriticalSection(&m_segmentSection);
		m_segmentList.push_back(m_pCurSegment);
		::LeaveCriticalSection(&m_segmentSection);
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

bool AudioExtractor::StartListen()
{
	HRESULT hr = Start();
	if (FAILED(hr))
		return false;

	SetDone(false);

	m_hThreadCapture = (HANDLE)::_beginthreadex(NULL, 0, &CaptureTheadProc, this, 0, NULL);
	if (m_hThreadCapture == NULL)
		return false;

	return true;
}

void AudioExtractor::StopListen()
{
	SetDone(true);

	Stop();

	if (m_hThreadCapture != NULL)
	{
		CloseHandle(m_hThreadCapture);
		m_hThreadCapture = NULL;
	}
}

static UINT __stdcall CaptureTheadProc(LPVOID param)
{
	AudioExtractor *pAudio = (AudioExtractor*)param;
	return pAudio->Capture();
}

UINT AudioExtractor::GetSegmentCount() const
{
	return m_segmentList.size();
}

AudioFrameStorage* AudioExtractor::PopSegment()
{
	if (m_segmentList.size() > 0)
	{
		::EnterCriticalSection(&m_segmentSection);
		AudioFrameStorage *pStorage = m_segmentList.front();
		m_segmentList.pop_front();
		::LeaveCriticalSection(&m_segmentSection);
		return pStorage;
	}
	return NULL;
}