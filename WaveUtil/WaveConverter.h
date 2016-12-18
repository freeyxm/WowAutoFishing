#pragma once
#include "WaveFile.h"
#include "WaveUtil.h"

class WaveConverter
{
public:
	WaveConverter();
	~WaveConverter();

	void SetFormat(const WAVEFORMATEX *pwfxSrc, const WAVEFORMATEX *pwfxDst, uint32_t bufferFrameCount = 0);
	void Reset();

	uint32_t ReadFrame(char *pData, uint32_t frameCount);

protected:
	virtual uint32_t LoadSrcFrame(char *pData, uint32_t frameCount) = 0;

private:
	uint32_t DstToSrcFrameCount(uint32_t dstFrameCount);
	uint32_t DstToSrcFrameIndex(uint32_t dstFrameIndex);

	uint32_t ReadFrameSampleRate(char *pData, uint32_t frameCount);
	uint32_t ReadFrameNormal(char *pData, uint32_t frameCount);

	uint32_t ConvertSampleRate(const char *pDataSrc, uint32_t &srcFrameCount, char *pDataDst, uint32_t dstFrameCount);
	uint32_t ConvertNormal(const char *pDataSrc, char *pDataDst, uint32_t frameCount);

	void ConvertFrame(const char *pDataSrc, char *pDataDst);
	void ConvertSample(const char *pDataSrc, char *pDataDst);

protected:
	const WAVEFORMATEX *m_pwfxSrc;
	const WAVEFORMATEX *m_pwfxDst;
	float m_sampleRateRatio;
	uint32_t m_srcBytesPerSample;
	uint32_t m_dstBytesPerSample;
	uint32_t m_srcBytesPerFrame;
	uint32_t m_dstBytesPerFrame;
private:
	uint32_t m_srcFrameIndex;
	uint32_t m_dstFrameIndex;
	uint32_t m_srcBufferFrameIndex;
	uint32_t m_srcBufferFrameCount;
	char *m_pSrcBuffer;
	char *m_pSrcPreFrame;
};

