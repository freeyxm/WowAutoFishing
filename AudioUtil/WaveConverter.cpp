#include "stdafx.h"
#include "WaveConverter.h"
#include <cmath>
#include <assert.h>

#define MAX_U8 255U
#define MAX_U16 65535U
#define MAX_U24 16777215U
#define MAX_U32 4294967295U

WaveConverter::WaveConverter()
{
}

WaveConverter::~WaveConverter()
{
}

void WaveConverter::SetFormat(const WAVEFORMATEX *pwfxSrc, const WAVEFORMATEX *pwfxDst)
{
	m_pwfxSrc = pwfxSrc;
	m_pwfxDst = pwfxDst;
	m_sampleRateRatio = (float)m_pwfxSrc->nSamplesPerSec / m_pwfxDst->nSamplesPerSec;

	m_srcBytesPerSample = m_pwfxSrc->wBitsPerSample / 8;
	m_dstBytesPerSample = m_pwfxDst->wBitsPerSample / 8;
	m_srcBytesPerFrame = m_srcBytesPerSample * m_pwfxSrc->nChannels;
	m_dstBytesPerFrame = m_dstBytesPerSample * m_pwfxDst->nChannels;

	m_srcFrameIndex = 0;
	m_dstFrameIndex = 0;
}

void WaveConverter::SetSrcFrameCount(uint32_t count)
{
	m_srcFrameCount = count;
}

uint32_t WaveConverter::Convert(const char *pDataSrc, char *pDataDst, uint32_t frameCount)
{
	uint32_t index = 0;
	for (; index < frameCount; ++index)
	{
		// 跳转到采样帧
		uint32_t srcFrameIndex = (uint32_t)::roundf(m_dstFrameIndex * m_sampleRateRatio);
		if (srcFrameIndex > m_srcFrameIndex)
		{
			pDataSrc += m_srcBytesPerFrame * (srcFrameIndex - m_srcFrameIndex);
		}
		m_dstFrameIndex++;
		m_srcFrameIndex = srcFrameIndex;
		if (m_srcFrameIndex >= m_srcFrameIndex)
			break;

		// 转换每帧数据
		for (int c = 0; c < m_pwfxSrc->nChannels; ++c)
		{
			if (c >= m_pwfxDst->nChannels)
				break;
			ConvertSample(pDataSrc + c * m_srcBytesPerFrame, pDataDst + c * m_dstBytesPerSample);
		}

		// 填充空声道
		if (m_pwfxSrc->nChannels < m_pwfxDst->nChannels)
		{
			memset(pDataDst + m_pwfxSrc->nChannels * m_dstBytesPerSample, 0, m_dstBytesPerSample * (m_pwfxDst->nChannels - m_pwfxSrc->nChannels));
		}

		pDataSrc += m_srcBytesPerFrame;
		pDataDst += m_dstBytesPerFrame;
	}
	return index;
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
				uint8_t src = *(uint8_t*)pDataSrc;
				switch (m_pwfxDst->wBitsPerSample)
				{
				case 16:
					{
						uint16_t dst = src * (uint16_t)(MAX_U16 / MAX_U8);
						*(uint16_t*)pDataDst = dst;
					}
					break;
				case 24:
					{
						uint32_t dst = src * (uint32_t)(MAX_U24 / MAX_U8);
						*(uint8_t*)(pDataDst + 0) = (dst) & 0xFF;
						*(uint8_t*)(pDataDst + 1) = (dst >> 8) & 0xFF;
						*(uint8_t*)(pDataDst + 2) = (dst >> 16) & 0xFF;
					}
				case 32:
					{
						uint32_t dst = src * (uint32_t)(MAX_U32 / MAX_U8);
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
				uint16_t src = *(uint16_t*)pDataSrc;
				switch (m_pwfxDst->wBitsPerSample)
				{
				case 8:
					{
						uint8_t dst = (src * (uint32_t)MAX_U8) & ~MAX_U16;
						*(uint8_t*)pDataDst = dst;
					}
					break;
				case 24:
					{
						uint32_t dst = (uint32_t)::roundf(src * (float)MAX_U24 / MAX_U16);
						*(uint8_t*)(pDataDst + 0) = (dst) & 0xFF;
						*(uint8_t*)(pDataDst + 1) = (dst >> 8) & 0xFF;
						*(uint8_t*)(pDataDst + 2) = (dst >> 16) & 0xFF;
					}
					break;
				case 32:
					{
						uint32_t dst = src * (uint32_t)(MAX_U32 / MAX_U16);
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
				src <<= 8;
				src |= *(uint8_t*)(pDataSrc + 1);
				src <<= 8;
				src |= *(uint8_t*)(pDataSrc + 2);

				switch (m_pwfxDst->wBitsPerSample)
				{
				case 8:
					{
						uint8_t dst = (src * MAX_U8) & ~MAX_U24;
						*(uint8_t*)pDataDst = dst;
					}
					break;
				case 16:
					{
						uint16_t dst = (uint16_t)::roundf(src * (float)MAX_U16 / MAX_U24);
						*(uint16_t*)pDataDst = dst;
					}
					break;
				case 32:
					{
						uint32_t dst = (uint32_t)::roundf(src * (float)MAX_U32 / MAX_U24);
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
				uint32_t src = *(uint32_t*)pDataSrc;
				switch (m_pwfxDst->wBitsPerSample)
				{
				case 8:
					{
						uint8_t dst = src / (MAX_U32 / MAX_U8);
						*(uint8_t*)pDataDst = dst;
					}
					break;
				case 16:
					{
						uint16_t dst = src / (MAX_U32 / MAX_U16);
						*(uint16_t*)pDataDst = dst;
					}
					break;
				case 24:
					{
						uint32_t dst = (uint32_t)::roundf(src * (float)MAX_U24 / MAX_U32);
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
		default:
			assert(false);
			break;
		}
	}
}

