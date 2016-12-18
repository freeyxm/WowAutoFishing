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

uint32_t WaveConverter::GetSrcFrameCount(uint32_t dstFrameCount)
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

uint32_t WaveConverter::Convert(const char *pDataSrc, uint32_t srcFrameCount, char *pDataDst, uint32_t dstFrameCount)
{
	uint32_t srcIndex = 0;
	uint32_t dstIndex = 0;
	for (; dstIndex < dstFrameCount; ++dstIndex)
	{
		// 跳转到采样帧
		if (m_pwfxSrc->nSamplesPerSec != m_pwfxDst->nSamplesPerSec)
		{
			uint32_t srcFrameIndex = DstToSrcFrameIndex(m_dstFrameIndex);
			if (srcFrameIndex > m_srcFrameIndex)
			{
				uint32_t step = srcFrameIndex - m_srcFrameIndex;
				pDataSrc += m_srcBytesPerFrame * step;
				srcIndex += step;
				m_srcFrameIndex = srcFrameIndex;
			}
			else if (srcFrameIndex < m_srcFrameIndex)
			{
				if (dstIndex > 0)
				{
					pDataSrc -= m_srcBytesPerFrame;
					m_srcFrameIndex--;
					srcIndex--;
				}
				else
				{
					// data lost !!!
					assert(false);
				}
			}
		}

		if (srcIndex >= srcFrameCount)
			break;

		// 转换每帧数据
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

		pDataSrc += m_srcBytesPerFrame;
		pDataDst += m_dstBytesPerFrame;
		m_srcFrameIndex++;
		m_dstFrameIndex++;
		srcIndex++;
	}
	return dstIndex;
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

