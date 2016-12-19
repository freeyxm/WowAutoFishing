﻿#pragma once
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
	WAVEFORMATEX m_wfxSrc;
	WAVEFORMATEX m_wfxDst;
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

private:
	enum class BitsConvertType
	{
		Bit_Equal,
		Bit_08_16,
		Bit_08_24,
		Bit_08_32,
		Bit_16_08,
		Bit_16_24,
		Bit_16_32,
		Bit_24_08,
		Bit_24_16,
		Bit_24_32,
		Bit_32_08,
		Bit_32_16,
		Bit_32_24,
		Bit_Undefined,
	};

	enum class ChannelConvertType
	{
		CCT_1_1,
		CCT_1_2,
		CCT_2_2,
		CCT_2_1,
		CCT_EQ,
		CCT_LT,
		CCT_GT,
	};

	BitsConvertType GetBitsConvertType(const WAVEFORMATEX *pwfxSrc, const WAVEFORMATEX *pwfxDst);
	ChannelConvertType GetChannelConvertType(const WAVEFORMATEX *pwfxSrc, const WAVEFORMATEX *pwfxDst);

	BitsConvertType m_bitsConvertType;
	ChannelConvertType m_channelConvertType;
};

