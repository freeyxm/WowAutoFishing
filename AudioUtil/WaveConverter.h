#pragma once
#include "WaveFile.h"
#include "WaveUtil.h"

class WaveConverter
{
public:
	WaveConverter();
	~WaveConverter();

	void SetFormat(const WAVEFORMATEX *pwfxSrc, const WAVEFORMATEX *pwfxDst);
	void SetSrcFrameCount(uint32_t count);
	uint32_t Convert(const char *pDataSrc, char *pDataDst, uint32_t frameCount);

private:
	void ConvertSample(const char *pDataSrc, char *pDataDst);

private:
	const WAVEFORMATEX *m_pwfxSrc;
	const WAVEFORMATEX *m_pwfxDst;
	float m_sampleRateRatio;
	uint32_t m_srcFrameIndex;
	uint32_t m_dstFrameIndex;
	uint32_t m_srcBytesPerSample;
	uint32_t m_dstBytesPerSample;
	uint32_t m_srcBytesPerFrame;
	uint32_t m_dstBytesPerFrame;
	uint32_t m_srcFrameCount;
};

