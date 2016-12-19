#include "stdafx.h"
#include "WaveConverter.h"
#include "CommUtil/CommUtil.hpp"
#include <cmath>
#include <assert.h>
#include <memory>

using namespace comm_util;

WaveConverter::WaveConverter()
	: m_pSrcBuffer(NULL), m_pSrcPreFrame(NULL)
	, m_srcBufferFrameCount(0)
{
}

WaveConverter::~WaveConverter()
{
	SAFE_DELETE_A(m_pSrcBuffer);
	SAFE_DELETE_A(m_pSrcPreFrame);
}

void WaveConverter::SetFormat(const WAVEFORMATEX *pwfxSrc, const WAVEFORMATEX *pwfxDst, uint32_t bufferFrameCount)
{
	memcpy(&m_wfxSrc, pwfxSrc, sizeof(m_wfxSrc));
	memcpy(&m_wfxDst, pwfxDst, sizeof(m_wfxDst));
	m_sampleRateRatio = (float)m_wfxSrc.nSamplesPerSec / m_wfxDst.nSamplesPerSec;
	m_bitsConvertType = GetBitsConvertType(pwfxSrc, pwfxDst);
	m_channelConvertType = GetChannelConvertType(pwfxSrc, pwfxDst);

	m_srcBytesPerSample = m_wfxSrc.wBitsPerSample / 8;
	m_dstBytesPerSample = m_wfxDst.wBitsPerSample / 8;
	m_srcBytesPerFrame = m_srcBytesPerSample * m_wfxSrc.nChannels;
	m_dstBytesPerFrame = m_dstBytesPerSample * m_wfxDst.nChannels;

	if (bufferFrameCount == 0)
	{
		bufferFrameCount = m_wfxSrc.nSamplesPerSec / 2;
	}
	else
	{
		bufferFrameCount = (uint32_t)::roundf(bufferFrameCount * m_sampleRateRatio) + 1;
	}

	if (m_srcBufferFrameCount != bufferFrameCount)
	{
		m_srcBufferFrameCount = bufferFrameCount;
		SAFE_DELETE_A(m_pSrcBuffer);
		m_pSrcBuffer = new char[m_srcBufferFrameCount * m_srcBytesPerFrame];
	}
	m_srcBufferFrameIndex = 0;

	SAFE_DELETE_A(m_pSrcPreFrame);
	m_pSrcPreFrame = new char[m_srcBytesPerFrame];
	memset(m_pSrcPreFrame, 0, m_srcBytesPerFrame);
}

void WaveConverter::Reset()
{
	m_srcBufferFrameIndex = 0;
	m_srcFrameIndex = 0;
	m_dstFrameIndex = 0;
}

uint32_t WaveConverter::DstToSrcFrameCount(uint32_t dstFrameCount)
{
	if (dstFrameCount == 0)
		return 0;
	else
		return DstToSrcFrameIndex(m_dstFrameIndex + dstFrameCount - 1) - m_srcFrameIndex + 1;
}

uint32_t WaveConverter::DstToSrcFrameIndex(uint32_t dstFrameIndex)
{
	return (uint32_t)::roundf(dstFrameIndex * m_sampleRateRatio);
}

uint32_t WaveConverter::ReadFrame(char *pDataDst, uint32_t dstFrameCount)
{
	if (m_wfxSrc.nSamplesPerSec != m_wfxDst.nSamplesPerSec)
	{
		return ReadFrameSampleRate(pDataDst, dstFrameCount);
	}
	else
	{
		return ReadFrameNormal(pDataDst, dstFrameCount);
	}
}

uint32_t WaveConverter::ReadFrameSampleRate(char *pDataDst, uint32_t dstFrameCount)
{
	uint32_t dstCount = dstFrameCount;
	while (dstCount > 0)
	{
		uint32_t srcCount = DstToSrcFrameCount(dstCount);
		if (srcCount > m_srcBufferFrameCount)
			srcCount = m_srcBufferFrameCount;

		uint32_t rcount;
		if (m_srcBufferFrameIndex > 0)
		{
			if (srcCount > m_srcBufferFrameIndex)
			{
				srcCount -= m_srcBufferFrameIndex;
				rcount = LoadSrcFrame(m_pSrcBuffer + m_srcBufferFrameIndex * m_srcBytesPerFrame, srcCount);
				rcount += m_srcBufferFrameIndex;
			}
			else
			{
				rcount = m_srcBufferFrameIndex;
			}
		}
		else
		{
			rcount = LoadSrcFrame(m_pSrcBuffer, srcCount);
		}

		if (rcount == 0)
			break;

		uint32_t _srcCount = rcount;
		uint32_t wcount = ConvertSampleRate(m_pSrcBuffer, _srcCount, pDataDst, dstCount);
		dstCount -= wcount;
		pDataDst += wcount * m_dstBytesPerFrame;

		if (_srcCount < rcount)
		{
			m_srcBufferFrameIndex = rcount - _srcCount;
			memcpy(m_pSrcBuffer, m_pSrcBuffer + _srcCount * m_srcBytesPerFrame, m_srcBufferFrameIndex * m_srcBytesPerFrame);
		}
		else
		{
			m_srcBufferFrameIndex = 0;
		}

		if (wcount == 0)
			break;
	}
	return dstFrameCount - dstCount;
}

uint32_t WaveConverter::ReadFrameNormal(char *pDataDst, uint32_t frameCount)
{
	if (frameCount <= m_srcBufferFrameCount)
	{
		uint32_t rcount = LoadSrcFrame(m_pSrcBuffer, frameCount);
		uint32_t wcount = ConvertNormal(m_pSrcBuffer, pDataDst, rcount);
		return wcount;
	}
	else
	{
		uint32_t count = frameCount;
		while (count > 0)
		{
			uint32_t rcount = LoadSrcFrame(m_pSrcBuffer, min(count, m_srcBufferFrameCount));
			if (rcount == 0)
				break;
			uint32_t wcount = ConvertNormal(m_pSrcBuffer, pDataDst, rcount);
			count -= wcount;
			pDataDst += wcount * m_dstBytesPerFrame;
		}
		return frameCount - count;
	}
}

uint32_t WaveConverter::ConvertSampleRate(const char *pDataSrc, uint32_t &srcFrameCount, char *pDataDst, uint32_t dstFrameCount)
{
	uint32_t srcIndex = 0;
	uint32_t dstIndex = 0;
	uint32_t srcFrameIndexBak = m_srcFrameIndex;
	bool usePreFrame = false;

	while (dstIndex < dstFrameCount)
	{
		// jump to target frame
		int step = 0;
		uint32_t srcFrameIndex = DstToSrcFrameIndex(m_dstFrameIndex);
		if (srcFrameIndex > m_srcFrameIndex)
		{
			step = srcFrameIndex - m_srcFrameIndex;
		}
		else if (srcFrameIndex < m_srcFrameIndex)
		{
			assert(srcFrameIndex == m_srcFrameIndex - 1);
			if (srcIndex > 0)
			{
				step = -1;
			}
			else
			{
				step = 0;
				usePreFrame = true;
			}
		}

		srcIndex += step;
		if (srcIndex >= srcFrameCount)
			break;

		if (step > 0)
		{
			m_srcFrameIndex += step;
			pDataSrc += step * m_srcBytesPerFrame;
		}
		else if (step < 0)
		{
			step = -step;
			m_srcFrameIndex -= step;
			pDataSrc -= step * m_srcBytesPerFrame;
		}

		if (usePreFrame)
		{
			usePreFrame = false;
			ConvertFrame(m_pSrcPreFrame, pDataDst);
		}
		else
		{
			ConvertFrame(pDataSrc, pDataDst);
			pDataSrc += m_srcBytesPerFrame;
			srcIndex++;
			m_srcFrameIndex++;
		}

		pDataDst += m_dstBytesPerFrame;
		dstIndex++;
		m_dstFrameIndex++;
	}

	srcFrameCount = m_srcFrameIndex - srcFrameIndexBak;
	if (srcFrameCount > 0)
	{
		memcpy(m_pSrcPreFrame, pDataSrc - m_srcBytesPerFrame, m_srcBytesPerFrame);
	}

	return dstIndex;
}

uint32_t WaveConverter::ConvertNormal(const char *pDataSrc, char *pDataDst, uint32_t frameCount)
{
	for (uint32_t i = 0; i < frameCount; ++i)
	{
		ConvertFrame(pDataSrc, pDataDst);
		pDataSrc += m_srcBytesPerFrame;
		pDataDst += m_dstBytesPerFrame;
	}
	return frameCount;
}

void WaveConverter::ConvertFrame(const char *pDataSrc, char *pDataDst)
{
	switch (m_channelConvertType)
	{
	case WaveConverter::ChannelConvertType::CCT_1_1:
	case WaveConverter::ChannelConvertType::CCT_2_1:
		{
			ConvertSample(pDataSrc, pDataDst);
		}
		break;
	case WaveConverter::ChannelConvertType::CCT_1_2:
		{
			ConvertSample(pDataSrc, pDataDst);
			ConvertSample(pDataSrc, pDataDst + m_dstBytesPerSample);
		}
		break;
	case WaveConverter::ChannelConvertType::CCT_2_2:
		{
			ConvertSample(pDataSrc, pDataDst);
			ConvertSample(pDataSrc + m_srcBytesPerSample, pDataDst + m_dstBytesPerSample);
		}
		break;
	case WaveConverter::ChannelConvertType::CCT_EQ:
	case WaveConverter::ChannelConvertType::CCT_GT:
		{
			for (int c = 0; c < m_wfxDst.nChannels; ++c)
			{
				ConvertSample(pDataSrc + c * m_srcBytesPerSample, pDataDst + c * m_dstBytesPerSample);
			}
		}
		break;
	case WaveConverter::ChannelConvertType::CCT_LT:
		{
			for (int index = 0; index < m_wfxDst.nChannels; index += m_wfxSrc.nChannels)
			{
				for (int c = 0; c < m_wfxSrc.nChannels; ++c)
				{
					ConvertSample(pDataSrc + c * m_srcBytesPerSample, pDataDst + (index + c) * m_dstBytesPerSample);
				}
			}
		}
		break;
	default:
		assert(false);
		break;
	}
}

static inline int8_t Parse08BitsValue(const char *pDataSrc)
{
	return *(int8_t*)pDataSrc + 128;
}

static inline void Write08BitsValue(char *pDataDst, int8_t dst)
{
	*(int8_t*)pDataDst = dst;
}

static inline uint16_t Parse16BitsValue(const char *pDataSrc)
{
	return *(uint16_t*)pDataSrc;
}

static inline void Write16BitsValue(char *pDataDst, uint16_t dst)
{
	*(uint16_t*)pDataDst = dst;
}

static inline uint32_t Parse24BitsValue(const char *pDataSrc)
{
	uint32_t src = *(uint8_t*)pDataSrc;
	src |= (uint32_t)(*(uint8_t*)(pDataSrc + 1)) << 8;
	src |= (uint32_t)(*(uint8_t*)(pDataSrc + 2)) << 16;
	return src;
}

static inline void Write24BitsValue(char *pDataDst, uint32_t dst)
{
	*(uint8_t*)(pDataDst + 0) = (dst) & 0xFF;
	*(uint8_t*)(pDataDst + 1) = (dst >> 8) & 0xFF;
	*(uint8_t*)(pDataDst + 2) = (dst >> 16) & 0xFF;
}

static inline uint32_t Parse32BitsValue(const char *pDataSrc, WORD formatTag)
{
	uint32_t src;
	if (formatTag == 3)
		src = (uint32_t)(*(float*)pDataSrc * (1U << 31));
	else
		src = *(uint32_t*)pDataSrc;
	return src;
}

static inline void Write32BitsValue(char *pDataDst, uint32_t dst, WORD formatTag)
{
	if (formatTag == 3)
		*(float*)pDataDst = (float)dst / (1U << 31);
	else
		*(uint32_t*)pDataDst = dst;
}

// little-endian !!!
void WaveConverter::ConvertSample(const char *pDataSrc, char *pDataDst)
{
	switch (m_bitsConvertType)
	{
	case WaveConverter::BitsConvertType::Bit_Equal:
		{
			memcpy(pDataDst, pDataSrc, m_srcBytesPerSample);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_08_08:
		{
			uint8_t src = Parse08BitsValue(pDataSrc);
			Write08BitsValue(pDataDst, src);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_08_16:
		{
			uint32_t src = Parse08BitsValue(pDataSrc);
			uint16_t dst = src << 8;
			Write16BitsValue(pDataDst, dst);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_08_24:
		{
			uint32_t src = Parse08BitsValue(pDataSrc);
			uint32_t dst = src << 16;
			Write24BitsValue(pDataDst, dst);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_08_32:
		{
			uint32_t src = Parse08BitsValue(pDataSrc);
			uint32_t dst = src << 24;
			Write32BitsValue(pDataDst, dst, m_wfxDst.wFormatTag);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_16_08:
		{
			uint32_t src = Parse16BitsValue(pDataSrc);
			int8_t dst = (src >> 8) - 128;
			Write08BitsValue(pDataDst, dst);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_16_16:
		{
			uint16_t src = Parse16BitsValue(pDataSrc);
			Write16BitsValue(pDataDst, src);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_16_24:
		{
			uint32_t src = Parse16BitsValue(pDataSrc);
			uint32_t dst = src << 8;
			Write24BitsValue(pDataDst, dst);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_16_32:
		{
			uint32_t src = Parse16BitsValue(pDataSrc);
			uint32_t dst = src << 16;
			Write32BitsValue(pDataDst, dst, m_wfxDst.wFormatTag);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_24_08:
		{
			uint32_t src = Parse24BitsValue(pDataSrc);
			int8_t dst = (src >> 16) - 128;
			Write08BitsValue(pDataDst, dst);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_24_16:
		{
			uint32_t src = Parse24BitsValue(pDataSrc);
			uint16_t dst = src >> 8;
			Write16BitsValue(pDataDst, dst);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_24_24:
		{
			uint32_t src = Parse24BitsValue(pDataSrc);
			Write24BitsValue(pDataDst, src);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_24_32:
		{
			uint32_t src = Parse24BitsValue(pDataSrc);
			uint32_t dst = src << 8;
			Write32BitsValue(pDataDst, dst, m_wfxDst.wFormatTag);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_32_08:
		{
			uint32_t src = Parse32BitsValue(pDataSrc, m_wfxSrc.wFormatTag);
			int8_t dst = (src >> 24) - 128;
			Write08BitsValue(pDataDst, dst);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_32_16:
		{
			uint32_t src = Parse32BitsValue(pDataSrc, m_wfxSrc.wFormatTag);
			uint16_t dst = src >> 16;
			Write16BitsValue(pDataDst, dst);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_32_24:
		{
			uint32_t src = Parse32BitsValue(pDataSrc, m_wfxSrc.wFormatTag);
			uint32_t dst = src >> 8;
			Write24BitsValue(pDataDst, dst);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_32_32:
		{
			uint32_t src = Parse32BitsValue(pDataSrc, m_wfxSrc.wFormatTag);
			Write32BitsValue(pDataDst, src, m_wfxDst.wFormatTag);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_Undefined:
	default:
		assert(false);
		break;
	}
}

WaveConverter::BitsConvertType WaveConverter::GetBitsConvertType(const WAVEFORMATEX *pwfxSrc, const WAVEFORMATEX *pwfxDst)
{
	switch (pwfxSrc->wBitsPerSample)
	{
	case 8:
		switch (pwfxDst->wBitsPerSample)
		{
		case 8:
			return BitsConvertType::Bit_08_08;
		case 16:
			return BitsConvertType::Bit_08_16;
		case 24:
			return BitsConvertType::Bit_08_24;
		case 32:
			return BitsConvertType::Bit_08_32;
		}
		break;
	case 16:
		switch (pwfxDst->wBitsPerSample)
		{
		case 8:
			return BitsConvertType::Bit_16_08;
		case 16:
			return BitsConvertType::Bit_16_16;
		case 24:
			return BitsConvertType::Bit_16_24;
		case 32:
			return BitsConvertType::Bit_16_32;
		}
		break;
	case 24:
		switch (pwfxDst->wBitsPerSample)
		{
		case 8:
			return BitsConvertType::Bit_24_08;
		case 16:
			return BitsConvertType::Bit_24_16;
		case 24:
			return BitsConvertType::Bit_24_24;
		case 32:
			return BitsConvertType::Bit_24_32;
		}
		break;
	case 32:
		switch (pwfxDst->wBitsPerSample)
		{
		case 8:
			return BitsConvertType::Bit_32_08;
		case 16:
			return BitsConvertType::Bit_32_16;
		case 24:
			return BitsConvertType::Bit_32_24;
		case 32:
			return BitsConvertType::Bit_32_32;
		}
		break;
	}

	if (pwfxSrc->wBitsPerSample == pwfxDst->wBitsPerSample &&
	    pwfxSrc->wFormatTag == pwfxDst->wFormatTag)
		return BitsConvertType::Bit_Equal;
	else
		return BitsConvertType::Bit_Undefined;
}

WaveConverter::ChannelConvertType WaveConverter::GetChannelConvertType(const WAVEFORMATEX *pwfxSrc, const WAVEFORMATEX *pwfxDst)
{
	switch (pwfxSrc->nChannels)
	{
	case 1:
		{
			switch (pwfxDst->nChannels)
			{
			case 1:
				return ChannelConvertType::CCT_1_1;
			case 2:
				return ChannelConvertType::CCT_1_2;
			}
		}
		break;
	case 2:
		{
			switch (pwfxDst->nChannels)
			{
			case 1:
				return ChannelConvertType::CCT_2_1;
			case 2:
				return ChannelConvertType::CCT_2_2;
			}
		}
		break;
	}

	if (pwfxSrc->nChannels == pwfxDst->nChannels)
		return ChannelConvertType::CCT_EQ;
	else if (pwfxSrc->nChannels < pwfxDst->nChannels)
		return ChannelConvertType::CCT_LT;
	else
		return ChannelConvertType::CCT_GT;
}