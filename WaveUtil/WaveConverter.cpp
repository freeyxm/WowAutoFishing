﻿#include "stdafx.h"
#include "WaveConverter.h"
#include "CommUtil/CommUtil.hpp"
#include <cmath>
#include <assert.h>
#include <memory>

using namespace comm_util;

#define PI 3.14159265358979323846
#define PI_2 (PI * 2)

WaveConverter::WaveConverter()
    : m_sampleRateRatio(0)
    , m_srcBytesPerSample(0)
    , m_dstBytesPerSample(0)
    , m_srcBytesPerFrame(0)
    , m_dstBytesPerFrame(0)
    , m_srcMaxValue(0)
    , m_dstMaxValue(0)
    , m_srcFrameIndex(0)
    , m_dstFrameIndex(0)
    , m_srcBufferFrameCount(0)
    , m_srcFrameIndexFloat(0)
    , m_wndWidth(0)
    , m_wndWidth2(0)
    , m_bitsConvertType()
    , m_channelConvertType()
    , m_wfxSrc()
    , m_wfxDst()
{
	m_buffer1.pData = NULL;
	m_buffer2.pData = NULL;
}

WaveConverter::~WaveConverter()
{
	SAFE_DELETE_A(m_buffer1.pData);
	SAFE_DELETE_A(m_buffer2.pData);
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
	m_srcMaxValue = (1UL << m_wfxSrc.wBitsPerSample) - 1;
	m_dstMaxValue = (1UL << m_wfxDst.wBitsPerSample) - 1;

	// Quality is half the window width
	m_wndWidth2 = 1;
	m_wndWidth = m_wndWidth2 * 2 + 1;

	if (bufferFrameCount == 0)
	{
		bufferFrameCount = m_wfxSrc.nSamplesPerSec / 2;
	}
	else if (m_wfxSrc.nSamplesPerSec != m_wfxDst.nSamplesPerSec)
	{
		bufferFrameCount = bufferFrameCount * m_wfxSrc.nSamplesPerSec / m_wfxDst.nSamplesPerSec + m_wndWidth;
	}

	if (m_srcBufferFrameCount != bufferFrameCount)
	{
		m_srcBufferFrameCount = bufferFrameCount;
		SAFE_DELETE_A(m_buffer1.pData);
		SAFE_DELETE_A(m_buffer2.pData);
		m_buffer1.pData = new char[m_srcBufferFrameCount * m_srcBytesPerFrame];
		m_buffer2.pData = new char[m_srcBufferFrameCount * m_srcBytesPerFrame];
	}
	m_buffer1.index = 0;
	m_buffer1.count = 0;
	m_buffer2.index = 0;
	m_buffer2.count = 0;
}

void WaveConverter::Reset()
{
	m_srcFrameIndexFloat = 0;
	m_srcFrameIndex = 0;
	m_dstFrameIndex = 0;
}

uint32_t WaveConverter::ReadFrame(char *pDataDst, uint32_t dstFrameCount)
{
	if (m_wfxSrc.nSamplesPerSec != m_wfxDst.nSamplesPerSec)
	{
		return ReadFrameResample(pDataDst, dstFrameCount);
	}
	else
	{
		return ReadFrameNormal(pDataDst, dstFrameCount);
	}
}

uint32_t WaveConverter::ReadFrameResample(char *pDataDst, uint32_t dstFrameCount)
{
	uint32_t dstCount = dstFrameCount;
	while (dstCount > 0)
	{
		if (m_srcFrameIndex + m_wndWidth2 < m_buffer1.index + m_buffer1.count)
		{
			uint32_t nwrite = ResampleSingle(pDataDst, dstCount);
			dstCount -= nwrite;
			pDataDst += nwrite * m_dstBytesPerFrame;
		}
		else
		{
			swap(m_buffer1, m_buffer2);
			m_buffer1.index = m_buffer2.index + m_buffer2.count;
			m_buffer1.count = LoadSrcFrame(m_buffer1.pData, m_srcBufferFrameCount);
			if (m_buffer1.count == 0)
				break;
		}
	}
	return dstFrameCount - dstCount;
}

uint32_t WaveConverter::ReadFrameNormal(char *pDataDst, uint32_t frameCount)
{
	if (frameCount <= m_srcBufferFrameCount)
	{
		uint32_t rcount = LoadSrcFrame(m_buffer1.pData, frameCount);
		uint32_t wcount = ConvertNormal(m_buffer1.pData, pDataDst, rcount);
		return wcount;
	}
	else
	{
		uint32_t dstCount = frameCount;
		while (dstCount > 0)
		{
			uint32_t rcount = LoadSrcFrame(m_buffer1.pData, min(dstCount, m_srcBufferFrameCount));
			if (rcount == 0)
				break;
			uint32_t wcount = ConvertNormal(m_buffer1.pData, pDataDst, rcount);
			dstCount -= wcount;
			pDataDst += wcount * m_dstBytesPerFrame;
		}
		return frameCount - dstCount;
	}
}

// need to repair!!!
uint32_t WaveConverter::Resample(char *pDataDst, uint32_t frameCount)
{
	// fmax : nyqist half of destination sampleRate
	// fmax / fsr = 0.5;
	float fmaxDivSR = 0.5;
	float r_g = 2 * fmaxDivSR;

	const uint32_t maxSrcIndex = m_buffer1.index + m_buffer1.count - m_wndWidth2;
	uint32_t index = 0;
	for (; index < frameCount; ++index)
	{
		if (m_srcFrameIndex >= maxSrcIndex)
			break;

		for (int c = 0; c < m_wfxDst.nChannels; ++c)
		{
			float r_y = 0;
			for (int tau = -m_wndWidth2; tau <= m_wndWidth2; ++tau)
			{
				// input sample index
				int j = (int)(m_srcFrameIndexFloat + tau);
				if (j < 0)
					continue;

				// Hann Window. Scale and calculate sinc
				//float r_w = 0.5 * (1 - cosf(PI_2 * (tau + m_wndWidth2) / m_wndWidth));
				float r_w = (float)(0.5 - 0.5 * cos(PI_2 *(0.5 + (j - m_srcFrameIndexFloat) / m_wndWidth)));
				float r_a = (float)(PI_2 * ((double)j - m_srcFrameIndexFloat) * fmaxDivSR);
				float r_snc = 1.0;
				if (r_a != 0)
				{
					r_snc = sinf(r_a) / r_a;
				}

				int32_t value;
				if ((uint32_t)j < m_buffer1.index)
				{
					value = ParseSample(m_buffer2.pData + (j - m_buffer2.index) * m_srcBytesPerFrame + c * m_srcBytesPerSample);
				}
				else
				{
					value = ParseSample(m_buffer1.pData + (j - m_buffer1.index) * m_srcBytesPerFrame + c * m_srcBytesPerSample);
				}

				r_y += r_g * r_w * r_snc * value;
			}

			WriteSample(pDataDst, (uint32_t)abs(r_y));
			pDataDst += m_dstBytesPerSample;
		}

		m_dstFrameIndex++;
		m_srcFrameIndexFloat = m_dstFrameIndex * m_sampleRateRatio;
		m_srcFrameIndex = (int)m_srcFrameIndexFloat;
	}

	return index;
}

uint32_t WaveConverter::ResampleSingle(char *pDataDst, uint32_t frameCount)
{
	const uint32_t maxSrcIndex = m_buffer1.index + m_buffer1.count;
	uint32_t index = 0;
	for (; index < frameCount; ++index)
	{
		const char *pDataSrc = NULL;
		if (m_srcFrameIndex < m_buffer1.index)
		{
			pDataSrc = m_buffer2.pData + (m_srcFrameIndex - m_buffer2.index) * m_srcBytesPerFrame;
		}
		else if (m_srcFrameIndex < maxSrcIndex)
		{
			pDataSrc = m_buffer1.pData + (m_srcFrameIndex - m_buffer1.index) * m_srcBytesPerFrame;
		}
		else
		{
			break;
		}

		ConvertFrame(pDataSrc, pDataDst);

		pDataDst += m_dstBytesPerFrame;

		m_dstFrameIndex++;
		m_srcFrameIndex = (uint32_t)roundf(m_dstFrameIndex * m_sampleRateRatio);
	}
	return index;
}

uint32_t WaveConverter::ResampleDouble(char *pDataDst, uint32_t frameCount)
{
	const uint32_t maxSrcIndex = m_buffer1.index + m_buffer1.count;
	uint32_t srcIndex1 = m_srcFrameIndex;
	uint32_t srcIndex2 = (uint32_t)ceilf(m_srcFrameIndexFloat);
	uint32_t index = 0;

	for (; index < frameCount; ++index)
	{
		const char *pDataSrc1 = NULL;
		if (srcIndex1 < m_buffer1.index) {
			pDataSrc1 = m_buffer2.pData + (srcIndex1 - m_buffer2.index) * m_srcBytesPerFrame;
		}
		else if (srcIndex1 < maxSrcIndex) {
			pDataSrc1 = m_buffer1.pData + (srcIndex1 - m_buffer1.index) * m_srcBytesPerFrame;
		}
		else {
			break;
		}

		if (srcIndex1 != srcIndex2 && srcIndex2 < maxSrcIndex)
		{
			const char *pDataSrc2 = NULL;
			if (srcIndex2 < m_buffer1.index) {
				pDataSrc2 = m_buffer2.pData + (srcIndex2 - m_buffer2.index) * m_srcBytesPerFrame;
			}
			else {
				pDataSrc2 = m_buffer1.pData + (srcIndex2 - m_buffer1.index) * m_srcBytesPerFrame;
			}

			float factor2 = m_srcFrameIndexFloat - srcIndex1;
			float factor1 = 1 - factor2;

			if (factor1 > factor2)
			{
				factor2 *= 0.01f;
				factor1 = 1 - factor2;
			}
			else
			{
				factor1 *= 0.01f;
				factor2 = 1 - factor1;
			}

			ConvertFrame(pDataDst, pDataSrc1, factor1, pDataSrc2, factor2);
		}
		else
		{
			ConvertFrame(pDataSrc1, pDataDst);
		}

		pDataDst += m_dstBytesPerFrame;

		m_dstFrameIndex++;
		m_srcFrameIndexFloat = m_dstFrameIndex * m_sampleRateRatio;
		srcIndex1 = (uint32_t)m_srcFrameIndexFloat;
		srcIndex2 = (uint32_t)ceilf(m_srcFrameIndexFloat);
	}

	m_srcFrameIndex = srcIndex1;
	return index;
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

void WaveConverter::ConvertFrame(char *pDataDst, const char *pDataSrc1, float factor1, const char *pDataSrc2, float factor2)
{
	switch (m_channelConvertType)
	{
	case WaveConverter::ChannelConvertType::CCT_1_1:
	case WaveConverter::ChannelConvertType::CCT_2_1:
		{
			ConvertSample(pDataDst, pDataSrc1, factor1, pDataSrc2, factor2);
		}
		break;
	case WaveConverter::ChannelConvertType::CCT_1_2:
		{
			ConvertSample(pDataDst, pDataSrc1, factor1, pDataSrc2, factor2);
			pDataDst += m_dstBytesPerSample;
			ConvertSample(pDataDst, pDataSrc1, factor1, pDataSrc2, factor2);
		}
		break;
	case WaveConverter::ChannelConvertType::CCT_2_2:
		{
			ConvertSample(pDataDst, pDataSrc1, factor1, pDataSrc2, factor2);
			pDataSrc1 += m_srcBytesPerSample;
			pDataSrc2 += m_srcBytesPerSample;
			pDataDst += m_dstBytesPerSample;
			ConvertSample(pDataDst, pDataSrc1, factor1, pDataSrc2, factor2);
		}
		break;
	default:
		break;
	}
}

void WaveConverter::ConvertSample(char *pDataDst, const char *pDataSrc1, float factor1, const char *pDataSrc2, float factor2)
{
	uint32_t src1 = ParseSample(pDataSrc1);
	uint32_t src2 = ParseSample(pDataSrc2);

	float value = src1 * factor1 + src2 * factor2;
	uint32_t dst = value > m_srcMaxValue ? m_srcMaxValue : (uint32_t)value;;

	WriteSample(pDataDst, dst);
}

static inline uint8_t Parse08BitsValue(const char *pDataSrc)
{
	return *(int8_t*)pDataSrc + 128;
}

static inline void Write08BitsValue(char *pDataDst, uint8_t dst)
{
	*(int8_t*)pDataDst = dst - 128;
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
			memcpy(pDataDst, pDataSrc, m_dstBytesPerFrame);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_08_08:
		{
			uint8_t src = *(uint8_t*)pDataSrc;
			*(uint8_t*)pDataDst = src;
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
			uint32_t dst = (src >> 8);
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
			uint32_t dst = (src >> 16);
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
			uint32_t dst = (src >> 24);
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
			uint32_t src = *(uint32_t*)pDataSrc;
			*(uint32_t*)pDataDst = src;
		}
		break;
	case WaveConverter::BitsConvertType::Bit_32_32f:
		{
			uint32_t src = Parse32BitsValue(pDataSrc, m_wfxSrc.wFormatTag);
			Write32BitsValue(pDataDst, src, m_wfxDst.wFormatTag);
		}
		break;
	case WaveConverter::BitsConvertType::Bit_Undefined:
	default:
		memset(pDataDst, 0, m_dstBytesPerFrame);
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
			if (pwfxSrc->wFormatTag == pwfxDst->wFormatTag)
				return BitsConvertType::Bit_32_32;
			else
				return BitsConvertType::Bit_32_32f;
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

uint32_t WaveConverter::ParseSample(const char *pDataSrc)
{
	switch (m_bitsConvertType)
	{
	case WaveConverter::BitsConvertType::Bit_08_08:
	case WaveConverter::BitsConvertType::Bit_08_16:
	case WaveConverter::BitsConvertType::Bit_08_24:
	case WaveConverter::BitsConvertType::Bit_08_32:
		return Parse08BitsValue(pDataSrc);
	case WaveConverter::BitsConvertType::Bit_16_08:
	case WaveConverter::BitsConvertType::Bit_16_16:
	case WaveConverter::BitsConvertType::Bit_16_24:
	case WaveConverter::BitsConvertType::Bit_16_32:
		return Parse16BitsValue(pDataSrc);
	case WaveConverter::BitsConvertType::Bit_24_08:
	case WaveConverter::BitsConvertType::Bit_24_16:
	case WaveConverter::BitsConvertType::Bit_24_24:
	case WaveConverter::BitsConvertType::Bit_24_32:
		return Parse24BitsValue(pDataSrc);
	case WaveConverter::BitsConvertType::Bit_32_08:
	case WaveConverter::BitsConvertType::Bit_32_16:
	case WaveConverter::BitsConvertType::Bit_32_24:
	case WaveConverter::BitsConvertType::Bit_32_32:
	case WaveConverter::BitsConvertType::Bit_32_32f:
		return Parse32BitsValue(pDataSrc, m_wfxSrc.wFormatTag);
	default:
		assert(false);
		return 0;
	}
}

void WaveConverter::WriteSample(char *pDataDst, uint32_t dst)
{
	switch (m_bitsConvertType)
	{
	case WaveConverter::BitsConvertType::Bit_08_08:
		Write08BitsValue(pDataDst, dst);
		break;
	case WaveConverter::BitsConvertType::Bit_08_16:
		Write16BitsValue(pDataDst, dst << 8);
		break;
	case WaveConverter::BitsConvertType::Bit_08_24:
		Write24BitsValue(pDataDst, dst << 16);
		break;
	case WaveConverter::BitsConvertType::Bit_08_32:
		Write32BitsValue(pDataDst, dst << 24, m_wfxDst.wFormatTag);
		break;
	case WaveConverter::BitsConvertType::Bit_16_08:
		Write08BitsValue(pDataDst, dst >> 8);
		break;
	case WaveConverter::BitsConvertType::Bit_16_16:
		Write16BitsValue(pDataDst, dst);
		break;
	case WaveConverter::BitsConvertType::Bit_16_24:
		Write24BitsValue(pDataDst, dst << 8);
		break;
	case WaveConverter::BitsConvertType::Bit_16_32:
		Write32BitsValue(pDataDst, dst << 16, m_wfxDst.wFormatTag);
		break;
	case WaveConverter::BitsConvertType::Bit_24_08:
		Write08BitsValue(pDataDst, dst >> 16);
		break;
	case WaveConverter::BitsConvertType::Bit_24_16:
		Write16BitsValue(pDataDst, dst >> 8);
		break;
	case WaveConverter::BitsConvertType::Bit_24_24:
		Write24BitsValue(pDataDst, dst);
		break;
	case WaveConverter::BitsConvertType::Bit_24_32:
		Write32BitsValue(pDataDst, dst << 8, m_wfxDst.wFormatTag);
		break;
	case WaveConverter::BitsConvertType::Bit_32_08:
		Write08BitsValue(pDataDst, dst >> 24);
		break;
	case WaveConverter::BitsConvertType::Bit_32_16:
		Write16BitsValue(pDataDst, dst >> 16);
		break;
	case WaveConverter::BitsConvertType::Bit_32_24:
		Write24BitsValue(pDataDst, dst >> 8);
		break;
	case WaveConverter::BitsConvertType::Bit_32_32:
	case WaveConverter::BitsConvertType::Bit_32_32f:
		Write32BitsValue(pDataDst, dst, m_wfxDst.wFormatTag);
		break;
	default:
		assert(false);
		memset(pDataDst, 0, m_dstBytesPerSample);
		break;
	}
}

bool WaveConverter::IsSupport(const WAVEFORMATEX *pwfxSrc, const WAVEFORMATEX *pwfxDst)
{
	auto bct = GetBitsConvertType(pwfxSrc, pwfxDst);
	if (bct == BitsConvertType::Bit_Undefined)
		return false;

	auto cct = GetChannelConvertType(pwfxSrc, pwfxDst);
	if (cct == ChannelConvertType::CCT_GT || cct == ChannelConvertType::CCT_LT)
		return false;

	return true;
}
