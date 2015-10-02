#include "stdafx.h"
#include "AudioExtractor.h"

static UINT __stdcall CaptureTheadProc(LPVOID param);

AudioExtractor::AudioExtractor()
	: m_pCurSegment(NULL), m_eSoundState(SoundState::Silent), m_silentCount(0)
{
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

	// reset state:
	m_eSoundState = SoundState::Silent;
	m_silentCount = 0;
}

HRESULT AudioExtractor::SetFormat(WAVEFORMATEX *pwfx)
{
	AudioCapture::SetFormat(pwfx);

	SetSilentMaxCount(5); // need to repair!!!
	SetSoundMinCount(20); // need to repair!!!
	SetAmpZcr(480, 0.001f, 0.01f, 0.3f, 0.5f);

	return S_OK;
}

HRESULT AudioExtractor::OnCaptureData(BYTE *pData, UINT32 nFrameCount)
{
	float amp = 0, zcr = 0;
	char pre_sign = 0;
	for(UINT32 i = 0; i < nFrameCount; ++i)
	{
		UINT32 baseIndex = i * m_pwfx->nChannels;
		float value = ParseValue(pData, baseIndex);
		char sign;
		if (value < 0)
		{
			sign = -1;
			value = -value;
		}
		else
		{
			sign = 1;
		}
		for(int k = 1; k < m_pwfx->nChannels; ++k)
		{
			float tmp = ParseValue(pData, baseIndex + k);
			if (tmp < 0)
				tmp = -tmp;
			if (tmp > value)
				value = tmp;
		}
		amp += value * value;
		if(value > 1E-6) // need to repair!!!
		{
			zcr += (sign != pre_sign) ? 1 : 0;
			pre_sign = sign;
		}
	}
	amp = amp * nFrameCount / m_sAmpZcr.frameCount;
	zcr = zcr / nFrameCount;

	//if(amp > 1E-6 || zcr > 1E-6 || m_eSoundState == AudioExtractor::SoundState::Sound)
	//{
	//	printf("Segment: amp = %f, zcr = %f\n", amp, zcr);
	//}

	switch (m_eSoundState)
	{
	case AudioExtractor::SoundState::Silent:
		{
			if(amp > m_sAmpZcr.ampH || zcr > m_sAmpZcr.zcrH)
			{
				//printf("StartSegment: amp = %f, zcr = %f\n", amp, zcr);
				StartSegment();
				if(m_silentCount > 0)
				{
					AppendSilentFrames();
				}
				AddFrame(pData, nFrameCount, amp);
				m_eSoundState = SoundState::Sound;
			}
			else if(amp > m_sAmpZcr.ampL || zcr > m_sAmpZcr.zcrL)
			{
				if(m_silentCount < m_silentMaxCount)
				{
					++m_silentCount;
					AddSilentFrame(pData, nFrameCount, amp);
				}
				else
				{
					PopSilentFrame(pData, nFrameCount, amp);
				}
			}
			else
			{
				ClearSilentFrames();
			}
		}
		break;
	case AudioExtractor::SoundState::Sound:
		{
			if(amp > m_sAmpZcr.ampL || zcr > m_sAmpZcr.zcrL)
			{
				if(m_silentCount > 0)
				{
					AppendSilentFrames();
				}
				AddFrame(pData, nFrameCount, amp);
			}
			else
			{
				if(m_silentCount < m_silentMaxCount)
				{
					++m_silentCount;
					AddSilentFrame(pData, nFrameCount, amp);
				}
				else
				{
					if (GetCurFrameCount() >= m_soundMinCount)
					{
						//printf("EndSegment: amp = %f, zcr = %f, frameCount = %d\n", amp, zcr, GetCurFrameCount());
						EndSegment();
					}
					else
					{
						//printf("CancelSegment: amp = %f, zcr = %f\n", amp, zcr);
						CancelSegment();
					}
					ClearSilentFrames();
					m_eSoundState = SoundState::Silent;
				}
			}
		}
		break;
	default:
		break;
	}

	return S_OK;
}

void AudioExtractor::SetSilentMaxCount(UINT count)
{
	m_silentMaxCount = count;
	m_silentFrames.SetCacheSize(count);
}

void AudioExtractor::SetSoundMinCount(UINT count)
{
	m_soundMinCount = count;
}

void AudioExtractor::SetAmpZcr(UINT frameCount, float ampL, float ampH, float zcrL, float zcrH)
{
	m_sAmpZcr.frameCount = frameCount;
	m_sAmpZcr.ampL = ampL;
	m_sAmpZcr.ampH = ampH;
	m_sAmpZcr.zcrL = zcrL;
	m_sAmpZcr.zcrH = zcrH;
}

void AudioExtractor::StartSegment()
{
	if(m_pCurSegment == NULL)
	{
		m_pCurSegment = new AudioFrameStorage();
		m_pCurSegment->SetCacheSize(m_soundMinCount);
	}
}

void AudioExtractor::EndSegment()
{
	if(m_pCurSegment != NULL)
	{
		::EnterCriticalSection(&m_segmentSection);
		m_segmentList.push_back(m_pCurSegment);
		::LeaveCriticalSection(&m_segmentSection);
		m_pCurSegment = NULL;
	}
}

void AudioExtractor::CancelSegment()
{
	if(m_pCurSegment != NULL)
	{
		m_pCurSegment->Clear();
	}
}

void AudioExtractor::AppendSilentFrames()
{
	if(m_pCurSegment == NULL)
		return;

	while(m_silentFrames.GetSize() > 0)
	{
		m_pCurSegment->PushBack(m_silentFrames.PopFront());
	}
	m_silentCount = 0;
}

void AudioExtractor::ClearSilentFrames()
{
	m_silentFrames.Clear();
	m_silentCount = 0;
}

inline void AudioExtractor::AddFrame(BYTE *pData, UINT32 nFrameCount, float amp)
{
	m_pCurSegment->PushBack(pData, nFrameCount * m_nBytesPerFrame);
}

inline void AudioExtractor::AddSilentFrame(BYTE *pData, UINT32 nFrameCount, float amp)
{
	m_silentFrames.PushBack(pData, nFrameCount * m_nBytesPerFrame);
}

inline void AudioExtractor::PopSilentFrame(BYTE *pData, UINT32 nFrameCount, float amp)
{
	m_silentFrames.ReplaceFront(pData, nFrameCount * m_nBytesPerFrame);
}

inline UINT AudioExtractor::GetCurFrameCount()
{
	return m_pCurSegment->GetSize();
}

bool AudioExtractor::Start()
{
	if (!StartCapture())
		return false;

	SetDone(false);

	m_hThreadCapture = (HANDLE)::_beginthreadex(NULL, 0, &CaptureTheadProc, this, 0, NULL);
	if (m_hThreadCapture == NULL)
		return false;

	return true;
}

void AudioExtractor::Stop()
{
	SetDone(true);

	StopCapture();

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

AudioExtractor::SegmentCIter AudioExtractor::cbegin() const
{
	return m_segmentList.cbegin();
}

AudioExtractor::SegmentCIter AudioExtractor::cend() const
{
	return m_segmentList.cend();
}