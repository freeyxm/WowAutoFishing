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
	uint32_t ReadFrameResample(char *pDataDst, uint32_t frameCount);
	uint32_t ReadFrameNormal(char *pDataDst, uint32_t frameCount);

	uint32_t Resample(char *pDataDst, uint32_t frameCount);
	uint32_t ResampleSingle(char *pDataDst, uint32_t frameCount);
	uint32_t ConvertNormal(const char *pDataSrc, char *pDataDst, uint32_t frameCount);

	void ConvertFrame(const char *pDataSrc, char *pDataDst);
	void ConvertSample(const char *pDataSrc, char *pDataDst);

	uint32_t ParseSample(const char *pDataSrc);
	void WriteSample(char *pDataDst, uint32_t value);

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
	uint32_t m_srcBufferFrameCount;
	float m_srcFrameIndexFloat;
	int m_wndWidth;
	int m_wndWidth2;

	struct Buffer
	{
		char *pData;
		uint32_t index;
		uint32_t count;
	};

	Buffer m_buffer1;
	Buffer m_buffer2;

private:
	enum class BitsConvertType
	{
		Bit_Equal,
		Bit_08_08,
		Bit_08_16,
		Bit_08_24,
		Bit_08_32,
		Bit_16_08,
		Bit_16_16,
		Bit_16_24,
		Bit_16_32,
		Bit_24_08,
		Bit_24_16,
		Bit_24_24,
		Bit_24_32,
		Bit_32_08,
		Bit_32_16,
		Bit_32_24,
		Bit_32_32,
		Bit_32_32f,
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

