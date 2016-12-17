#include "stdafx.h"
#include "WaveConverter.h"
#include <cmath>
#include <assert.h>

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
		if (m_pwfxSrc->nSamplesPerSec != m_pwfxDst->nSamplesPerSec)
		{
			uint32_t srcFrameIndex = (uint32_t)::roundf(m_dstFrameIndex * m_sampleRateRatio);
			if (srcFrameIndex > m_srcFrameIndex)
			{
				pDataSrc += m_srcBytesPerFrame * (srcFrameIndex - m_srcFrameIndex);
			}
			m_srcFrameIndex = srcFrameIndex;
		}

		if (m_srcFrameIndex >= m_srcFrameCount)
			break;

		// 转换每帧数据
		for (int c = 0; c < m_pwfxSrc->nChannels; ++c)
		{
			if (c >= m_pwfxDst->nChannels)
				break;
			ConvertSample(pDataSrc + c * m_srcBytesPerSample, pDataDst + c * m_dstBytesPerSample);
		}

		// 填充空声道
		if (m_pwfxSrc->nChannels < m_pwfxDst->nChannels)
		{
			memset(pDataDst + m_pwfxSrc->nChannels * m_dstBytesPerSample, 0, m_dstBytesPerSample * (m_pwfxDst->nChannels - m_pwfxSrc->nChannels));
		}

		pDataSrc += m_srcBytesPerFrame;
		pDataDst += m_dstBytesPerFrame;
		m_srcFrameIndex++;
		m_dstFrameIndex++;
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

