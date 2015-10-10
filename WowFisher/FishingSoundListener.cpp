#include "stdafx.h"
#include "FishingSoundListener.h"
#include "WaveGraph/Util/FFT.h"
#include "WaveGraph/Util/FastFourierTransform.h"
#include <functiondiscoverykeys.h>
#include <cstdio>
#include <ctime>
#include <map>


FishingSoundListener::FishingSoundListener(Fisher *pFisher)
	: AudioExtractor(true, false)
	, m_pFisher(pFisher), m_procCheckTimeout(NULL), m_procNotifyBite(NULL)
	, m_pSampleFile(NULL)
{
}

FishingSoundListener::~FishingSoundListener()
{
	if (m_pSampleFile != NULL)
	{
		::fclose(m_pSampleFile);
		m_pSampleFile = NULL;
	}
}

bool FishingSoundListener::Init()
{
	if (false && m_pSampleFile == NULL)
	{
		char buf[128];
		time_t t = ::time(NULL);
		struct tm ti;
		::localtime_s(&ti, &t);
		::sprintf_s(buf, "fishing sample %04d-%02d-%02d %02d.%02d.%02d.txt", (ti.tm_year + 1900), (ti.tm_mon + 1), ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);

		int ret = ::fopen_s(&m_pSampleFile, buf, "a+");
		if (ret != 0)
		{
			printf("Can't open file: %s\n", buf);
			return false;
		}
	}

	return AudioExtractor::Init();
}

void FishingSoundListener::SetCheckTimeoutProc(CheckTimeoutProc callback)
{
	m_procCheckTimeout = callback;
}

void FishingSoundListener::SetNotifyBiteProc(NotifyBiteProc callback)
{
	m_procNotifyBite = callback;
}

HRESULT FishingSoundListener::SetFormat(WAVEFORMATEX *pwfx)
{
	AudioCapture::SetFormat(pwfx);

	SetSilentMaxCount(10); // need to repair!!!
	SetSoundMinCount(20); // need to repair!!!
	SetAmpZcr(480, 0.05f, 2.0f, 0.3f, 0.5f);

	return S_OK;
}

void FishingSoundListener::StartSegment()
{
	m_nFrameCount = 0;
	m_nSlientFrameCount = 0;
	m_totalAmp = 0;
	m_slientAmp = 0;
}

void FishingSoundListener::EndSegment()
{
	if (m_pSampleFile != NULL)
	{
		char buf[128];
		::sprintf_s(buf, "%d, %.2f\n", m_nFrameCount, m_totalAmp / m_nFrameCount);
		::fwrite(buf, strlen(buf), 1, m_pSampleFile);
		::fflush(m_pSampleFile);
	}
		
	//::printf("amp: sum = %f, avg = %f\n", m_totalAmp, m_totalAmp / m_nFrameCount);

	if (m_nFrameCount > 140)
	{
		if (m_procNotifyBite != NULL)
		{
			(m_pFisher->*m_procNotifyBite)();
		}
	}
}

void FishingSoundListener::AppendSilentFrames()
{
	m_nFrameCount += m_nSlientFrameCount;
	m_totalAmp += m_slientAmp;
	m_nSlientFrameCount = 0;
	m_slientAmp = 0;
}

void FishingSoundListener::ClearSilentFrames()
{
	m_nSlientFrameCount = 0;
	m_slientAmp = 0;
}

inline void FishingSoundListener::AddFrame(BYTE *pData, UINT32 nFrameCount, float amp)
{
	++m_nFrameCount;
	m_totalAmp += amp;
}

inline void FishingSoundListener::AddSilentFrame(BYTE *pData, UINT32 nFrameCount, float amp)
{
	++m_nSlientFrameCount;
	m_slientAmp += amp;
}

inline UINT FishingSoundListener::GetCurFrameCount()
{
	return m_nFrameCount;
}

bool FishingSoundListener::IsDone() const
{
	if (!m_bDone && m_pFisher != NULL && m_procCheckTimeout != NULL)
	{
		return (m_pFisher->*m_procCheckTimeout)();
	}
	return m_bDone;
}

void FishingSoundListener::SetAmpL(float ampL)
{
	m_sAmpZcr.ampL = ampL;
}

void FishingSoundListener::SetAmpH(float ampH)
{
	m_sAmpZcr.ampH = ampH;
}