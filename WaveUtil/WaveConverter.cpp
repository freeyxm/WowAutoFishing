#include "stdafx.h"
#include "WaveConverter.h"
#include "CommUtil/CommUtil.hpp"
#include <cmath>
#include <assert.h>

using namespace comm_util;

WaveConverter::WaveConverter()
	: m_pwfxSrc(NULL), m_pwfxDst(NULL)
	, m_pSrcBuffer(NULL), m_pSrcPreFrame(NULL)
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
	m_pwfxSrc = pwfxSrc;
	m_pwfxDst = pwfxDst;
	m_sampleRateRatio = (float)m_pwfxSrc->nSamplesPerSec / m_pwfxDst->nSamplesPerSec;

	m_srcBytesPerSample = m_pwfxSrc->wBitsPerSample / 8;
	m_dstBytesPerSample = m_pwfxDst->wBitsPerSample / 8;
	m_srcBytesPerFrame = m_srcBytesPerSample * m_pwfxSrc->nChannels;
	m_dstBytesPerFrame = m_dstBytesPerSample * m_pwfxDst->nChannels;

	if (bufferFrameCount == 0)
	{
		bufferFrameCount = m_pwfxSrc->nSamplesPerSec / 2;
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
	if (m_pwfxSrc->nSamplesPerSec != m_pwfxDst->nSamplesPerSec)
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

		if (step != 0)
		{
			m_srcFrameIndex += step;
			pDataSrc += step * m_srcBytesPerFrame;
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
	if (m_pwfxSrc->nChannels >= m_pwfxDst->nChannels)
	{
		for (int c = 0; c < m_pwfxDst->nChannels; ++c)
		{
			ConvertSample(pDataSrc + c * m_srcBytesPerSample, pDataDst + c * m_dstBytesPerSample);
		}
	}
	else
	{
		for (int index = 0; index < m_pwfxDst->nChannels; index += m_pwfxSrc->nChannels)
		{
			for (int c = 0; c < m_pwfxSrc->nChannels; ++c)
			{
				ConvertSample(pDataSrc + c * m_srcBytesPerSample, pDataDst + (index + c) * m_dstBytesPerSample);
			}
		}
	}
}

// little-endian !!!
void WaveConverter::ConvertSample(const char *pDataSrc, char *pDataDst)
{
	if (m_pwfxSrc->wBitsPerSample == m_pwfxDst->wBitsPerSample)
	{
		memcpy(pDataDst, pDataSrc, m_srcBytesPerSample);
	}
	else
	{
		switch (m_pwfxSrc->wBitsPerSample)
		{
		case 8:
			{
				uint32_t src = *(int8_t*)pDataSrc + 128;
				switch (m_pwfxDst->wBitsPerSample)
				{
				case 16:
					{
						uint16_t dst = src << 8;
						*(uint16_t*)pDataDst = dst;
					}
					break;
				case 24:
					{
						uint32_t dst = src << 16;
						*(uint8_t*)(pDataDst + 0) = (dst) & 0xFF;
						*(uint8_t*)(pDataDst + 1) = (dst >> 8) & 0xFF;
						*(uint8_t*)(pDataDst + 2) = (dst >> 16) & 0xFF;
					}
					break;
				case 32:
					{
						uint32_t dst = src << 24;
						if (m_pwfxDst->wFormatTag == 3)
							*(float*)pDataDst = (float)dst / (1U << 31);
						else
							*(uint32_t*)pDataDst = dst;
					}
					break;
				default:
					assert(false);
					break;
				}
			}
			break;
		case 16:
			{
				uint32_t src = *(uint16_t*)pDataSrc;
				switch (m_pwfxDst->wBitsPerSample)
				{
				case 8:
					{
						int8_t dst = (src >> 8) - 128;
						*(int8_t*)pDataDst = dst;
					}
					break;
				case 24:
					{
						uint32_t dst = src << 8;
						*(uint8_t*)(pDataDst + 0) = (dst) & 0xFF;
						*(uint8_t*)(pDataDst + 1) = (dst >> 8) & 0xFF;
						*(uint8_t*)(pDataDst + 2) = (dst >> 16) & 0xFF;
					}
					break;
				case 32:
					{
						uint32_t dst = src << 16;
						if (m_pwfxDst->wFormatTag == 3)
							*(float*)pDataDst = (float)dst / (1U << 31);
						else
							*(uint32_t*)pDataDst = dst;
					}
					break;
				default:
					assert(false);
					break;
				}
			}
			break;
		case 24:
			{
				uint32_t src = *(uint8_t*)pDataSrc;
				src |= (uint32_t)(*(uint8_t*)(pDataSrc + 1)) << 8;
				src |= (uint32_t)(*(uint8_t*)(pDataSrc + 2)) << 16;

				switch (m_pwfxDst->wBitsPerSample)
				{
				case 8:
					{
						int8_t dst = (src >> 16) - 128;
						*(int8_t*)pDataDst = dst;
					}
					break;
				case 16:
					{
						uint16_t dst = src >> 8;
						*(uint16_t*)pDataDst = dst;
					}
					break;
				case 32:
					{
						uint32_t dst = src << 8;
						if (m_pwfxDst->wFormatTag == 3)
							*(float*)pDataDst = (float)dst / (1U << 31);
						else
							*(uint32_t*)pDataDst = dst;
					}
					break;
				default:
					assert(false);
					break;
				}
			}
			break;
		case 32:
			{
				uint32_t src;
				if (m_pwfxSrc->wFormatTag == 3)
					src = (uint32_t)(*(float*)pDataSrc * (1U << 31));
				else
					src = *(uint32_t*)pDataSrc;

				switch (m_pwfxDst->wBitsPerSample)
				{
				case 8:
					{
						int8_t dst = (src >> 24) - 128;
						*(int8_t*)pDataDst = dst;
					}
					break;
				case 16:
					{
						uint16_t dst = src >> 16;
						*(uint16_t*)pDataDst = dst;
					}
					break;
				case 24:
					{
						uint32_t dst = src >> 8;
						*(uint8_t*)(pDataDst + 0) = (dst) & 0xFF;
						*(uint8_t*)(pDataDst + 1) = (dst >> 8) & 0xFF;
						*(uint8_t*)(pDataDst + 2) = (dst >> 16) & 0xFF;
					}
					break;
				default:
					assert(false);
					break;
				}
			}
			break;
		default:
			assert(false);
			break;
		}
	}
}

